#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

void listen_signal()
{
    DBusMessage * msg;
    DBusMessageIter arg;
    DBusConnection * connection;
    DBusError err;
    int ret;
    char * sigvalue;

     //����1:������D-Bus��̨������
    dbus_error_init(&err);
    connection =dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"ConnectionError %s\n",err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL)
        return;

   //����2:������������һ���ɼ�������test.singal.dest��ΪBus name��������費�Ǳ����,���Ƽ���������
    ret =dbus_bus_request_name(connection,"test.singal.dest",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Name Error%s\n",err.message);
        dbus_error_free(&err);
    }
    if(ret !=DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        return;

    //����3:֪ͨD-Bus daemon��ϣ���������нӿ�test.signal.Type���ź�
    dbus_bus_add_match(connection,"type='signal',interface='test.signal.Type'",&err);
    //ʵ����Ҫ���Ͷ�����daemon��֪ͨϣ�����������ݣ�������Ҫflush
    dbus_connection_flush(connection);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Match Error%s\n",err.message);
        dbus_error_free(&err);
    }
   
    //����4:��ѭ���м�����ÿ����1�룬��ȥ��ͼ�Լ��������л�ȡ����źš�������������������л�ȡ�κ���Ϣ�ķ�ʽ�����Ի�ȡ��ȥ���һ�������Ϣ�Ƿ������������źţ�����ȡ���ݡ�����Ҳ����ͨ�������ʽ����ȡmethod call��Ϣ��
    while(1){
        dbus_connection_read_write(connection,0);
        msg =dbus_connection_pop_message (connection);
        if(msg == NULL){
            sleep(1);
            continue;
        }
   
        if(dbus_message_is_signal(msg,"test.signal.Type","Test")){
            if(!dbus_message_iter_init(msg,&arg))
                fprintf(stderr,"MessageHas no Param");
            else if(dbus_message_iter_get_arg_type(&arg)!= DBUS_TYPE_STRING)
                printf("Param isnot string");
            else
                dbus_message_iter_get_basic(&arg,&sigvalue);
            printf("Got Singal withvalue : %s\n",sigvalue);
        }
        dbus_message_unref(msg);
    }//End of while
       
}

int main( int argc , char ** argv){
    listen_signal();
    return 0;
}

