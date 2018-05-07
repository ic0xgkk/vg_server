#include "include.h"

void message_handling(void)
{
    while(1)
    {
        char message[QUEUE_DATA_SIZE];
        memset(message, 0, QUEUE_DATA_SIZE);

        dequeue( com_qid, 1, message);
        syslog(LOG_INFO,"Message \"%s\" in Handling... ",message);
        char sign1;
        memset(&sign1,0xB1,sizeof(char) );  //a

        if( memcmp(message,&sign1,sizeof(char) ) == 0 )
        {
            syslog(LOG_INFO,"Message ready to send to SupplyDeport...\n");
            forward_to_supplydepot( message );
        }
        else
        {
            syslog(LOG_INFO,"Message is no use.\n");
        }

    }
}

void forward_to_supplydepot( char message[] )
{
    char send_buf[BUFFER_SIZE];
    memset(send_buf,0,BUFFER_SIZE);
    memcpy(send_buf, message+(sizeof(char)*3), BUFFER_SIZE-(sizeof(char)*3) );
    syslog(LOG_INFO,"-SupplyDeport Send Buffer:\"%s\"  \n",send_buf);
    send( sd_connfd, send_buf, BUFFER_SIZE-(sizeof(char)*3), 0 );
    syslog(LOG_INFO,"Sended to SupplyDeport: %s \n",send_buf);
}



/*
//报文第0位为ORI(方向位)，第1位为CID(设备标识符)，从第2位开始为数据位
int data_manipulation( char buffer[BUFFER_SIZE] )
{
    //标志位分离
    uint8_t orientation_sign=(uint8_t)buffer[0];

    switch( orientation_sign )
    {
        case 0xB1: //转发到补给站
        {
            break;
        }

        default:
        {
            print_unknow_ori( orientation_sign );
            return -2;
        }

    }

        int cid=0;
        for( int i=0;i<DEVICE_AMOUNT;i++ )
        {
            if( device_data[i].cid == 50 )
            {
                cid = i;
            }
        }

        struct sockaddr_in forward_addr;
        if( inet_aton(device_data[cid].ip,&forward_addr.sin_addr) == 0 )
        {
            perror("ERROR: INET_ATON ERROR:");
            return -1;
        }

        int forward_tcp_status=0;

        if( forward_tcp_status == 0 )
        {


        }


    if( buffer[0] == '2' )
    {
        printf("What the fuck????\n");
        exit(EXIT_FAILURE);
    }

    return 1;
}
*/
