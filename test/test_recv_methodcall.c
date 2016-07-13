#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

 
void reply_to_method_call(DBusMessage * msg, DBusConnection * conn){
    DBusMessage * reply;
    DBusMessageIter arg;
    char * param = NULL;
    dbus_bool_t stat = TRUE;
    dbus_uint32_t level = 2010;
    dbus_uint32_t serial = 0;
   
    //��msg�ж�ȡ�������������һ��ѧϰ��ѧ��
   if(!dbus_message_iter_init(msg,&arg))
        printf("Message has noargs\n");
    else if(dbus_message_iter_get_arg_type(&arg)!= DBUS_TYPE_STRING)
        printf("Arg is notstring!\n");
    else
       dbus_message_iter_get_basic(&arg,& param);
    if(param == NULL) return;


    //����������Ϣreply
    reply = dbus_message_new_method_return(msg);
    //�ڷ�����Ϣ�������������������źż�������ķ�ʽ��һ���ġ�������ǽ���������������
    dbus_message_iter_init_append(reply,&arg);
    if(!dbus_message_iter_append_basic(&arg,DBUS_TYPE_BOOLEAN,&stat)){
        printf("Out ofMemory!\n");
        exit(1);
    }
    if(!dbus_message_iter_append_basic(&arg,DBUS_TYPE_UINT32,&level)){
        printf("Out ofMemory!\n");
        exit(1);
    }
  //���ͷ�����Ϣ
      if( !dbus_connection_send(conn, reply,&serial)){
        printf("Out of Memory\n");
        exit(1);
    }
    dbus_connection_flush (conn);
    dbus_message_unref (reply);
}

 
void listen_dbus()
{
    DBusMessage * msg;
    DBusMessageIter arg;
    DBusConnection * connection;
    DBusError err;
    int ret;
    char * sigvalue;

    dbus_error_init(&err);
    //������session D-Bus������
    connection =dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"ConnectionError %s\n",err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL)
        return;
    //����һ��BUS name��test.wei.dest
    ret =dbus_bus_request_name(connection,"test.wei.dest",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Name Error%s\n",err.message);
        dbus_error_free(&err);
    }
    if(ret !=DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        return;

    //Ҫ�����ĳ��singal�����Խӿ�test.signal.Type���ź�
   dbus_bus_add_match(connection,"type='signal',interface='test.signal.Type'",&err);
    dbus_connection_flush(connection);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Match Error%s\n",err.message);
        dbus_error_free(&err);
    }

    while(1){
        dbus_connection_read_write(connection,0);
        msg =dbus_connection_pop_message (connection);

        if(msg == NULL){
            sleep(1);
            continue;
        }

        if(dbus_message_is_signal(msg,"test.signal.Type","Test")){
            if(!dbus_message_iter_init(msg,&arg))
               fprintf(stderr,"Message Has no Param");
            else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING)
                printf("Param isnot string");
            else
               dbus_message_iter_get_basic(&arg,&sigvalue);
        }else if(dbus_message_is_method_call(msg,"test.method.Type","Method")){
            //�����������ȱȽ��˽ӿ����ֺͷ������֣�ʵ����Ӧ���ֱȽ�·��
            if(strcmp(dbus_message_get_path(msg),"/test/method/Object") == 0)
               reply_to_method_call(msg,connection);
        }
        dbus_message_unref(msg);
    }
   
   
}
int main( int argc , char ** argv){
    listen_dbus();
    return 0;
}

