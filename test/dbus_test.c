#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
** Common codes
** Session BUS && System BUS
*/
void init_dbus_conn(DBusConnection**ret_conn)
{
    DBusError err;
    DBusConnection* conn;
    int ret;

    // initialise the errors
    dbus_error_init(&err);

    // connect to the bus
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err))
    {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if(NULL == conn)
    {
        return(-1);
    }
    // request a name on the bus
    ret = dbus_bus_request_name(conn, "test.method.server",
                                DBUS_NAME_FLAG_REPLACE_EXISTING
                                , &err);
    if(dbus_error_is_set(&err))
    {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if(DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
    {
        return(-1);
    }

    *ret_conn = conn;

    return 0;
}


void close_dbus_conn(DBusConnection* conn)
{
    dbus_connection_close(conn);
}


int send_msg_signal()
{
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal("/test/signal/Object", // object name of the signal
                                  "test.signal.Type", // interface name of the signal
                                  "Test"); // name of the signal
    if(NULL == msg)
    {
        fprintf(stderr, "Message Null\n");
        return(-1);
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return(-1);
    }

    // send the message and flush the connection
    if(!dbus_connection_send(conn, msg, &serial))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return(-1);
    }
    dbus_connection_flush(conn);

    // free the message
    dbus_message_unref(msg);


    return 0;
}


int call_method()
{
    DBusMessage* msg;
    DBusMessageIter args;
    DBusPendingCall* pending;

    msg = dbus_message_new_method_call("test.method.server", // target for the method call
                                       "/test/method/Object", // object to call on
                                       "test.method.Type", // interface to call on
                                       "Method"); // method name
    if(NULL == msg)
    {
        fprintf(stderr, "Message Null\n");
        return(-1);
    }

    // append arguments
    dbus_message_iter_init_append(msg, &args);
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return(-1);
    }

    // send message and get a handle for a reply
    if(!dbus_connection_send_with_reply(conn, msg, &pending, -1))     // -1 is default timeout
    {
        fprintf(stderr, "Out Of Memory!\n");
        return(-1);
    }

    if(NULL == pending)
    {
        fprintf(stderr, "Pending Call Null\n");
        return(-1);
    }

    dbus_connection_flush(conn);

    // free message
    dbus_message_unref(msg);

    return 0;
}


int call_method2()
{
    bool stat;
    dbus_uint32_t level;

    // block until we receive a reply
    dbus_pending_call_block(pending);

    // get the reply message
    msg = dbus_pending_call_steal_reply(pending);
    if(NULL == msg)
    {
        fprintf(stderr, "Reply Null\n");
        exit(1);
    }
    // free the pending message handle
    dbus_pending_call_unref(pending);

    // read the parameters
    if(!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else if(DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Argument is not boolean!\n");
    else
        dbus_message_iter_get_basic(&args, &stat);

    if(!dbus_message_iter_next(&args))
        fprintf(stderr, "Message has too few arguments!\n");
    else if(DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Argument is not int!\n");
    else
        dbus_message_iter_get_basic(&args, &level);

    printf("Got Reply: %d, %d\n", stat, level);

    // free reply and close connection
    dbus_message_unref(msg);


}


int recv_msg_signal()
{
    // add a rule for which messages we want to see
    dbus_bus_add_match(conn,
                       "type='signal',interface='test.signal.Type'",
                       &err); // see signals from the given interface

    dbus_connection_flush(conn);

    if(dbus_error_is_set(&err))
    {
        fprintf(stderr, "Match Error (%s)\n", err.message);
        return -1;
    }

    // loop listening for signals being emmitted
    while(true)
    {

        // non blocking read of the next available message
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        // loop again if we haven't read a message
        if(NULL == msg)
        {
            sleep(1);
            continue;
        }

        // check if the message is a signal from the correct interface and with the correct name
        if(dbus_message_is_signal(msg, "test.signal.Type", "Test"))
        {
            // read the parameters
            if(!dbus_message_iter_init(msg, &args))
                fprintf(stderr, "Message has no arguments!\n");
            else if(DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
                fprintf(stderr, "Argument is not string!\n");
            else
            {
                dbus_message_iter_get_basic(&args, &sigvalue);
                printf("Got Signal with value %s\n", sigvalue);
            }
        }

        // free the message
        dbus_message_unref(msg);
    }


#ifdef USE_TEST
    {
        // loop, testing for new messages
        while(true)
        {
            // non blocking read of the next available message
            dbus_connection_read_write(conn, 0);
            msg = dbus_connection_pop_message(conn);

            // loop again if we haven't got a message
            if(NULL == msg)
            {
                sleep(1);
                continue;
            }

            // check this is a method call for the right interface and method
            if(dbus_message_is_method_call(msg, "test.method.Type", "Method"))
                reply_to_method_call(msg, conn);

            // free the message
            dbus_message_unref(msg);
        }



    }
#endif

    return 0;
}


void reply_to_method_call(DBusMessage* msg, DBusConnection* conn)
{
    DBusMessage* reply;
    DBusMessageIter args;
    DBusConnection* conn;
    bool stat = true;
    dbus_uint32_t level = 21614;
    dbus_uint32_t serial = 0;
    char* param = "";

    // read the arguments
    if(!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else if(DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Argument is not string!\n");
    else
        dbus_message_iter_get_basic(&args, &param);
    printf("Method called with %s\n", param);

    // create a reply from the message
    reply = dbus_message_new_method_return(msg);

    // add the arguments to the reply
    dbus_message_iter_init_append(reply, &args);
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &stat))
    {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    if(!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &level))
    {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    // send the reply && flush the connection
    if(!dbus_connection_send(conn, reply, &serial))
    {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    // free the reply
    dbus_message_unref(reply);
}









