#include "include.h"

void message_handling(void)
{
//    uint16_t connection_sum=rows("device");
//    memset( &forward_lock, 0, sizeof(uint8_t)*connection_sum );

    while(1)
    {
        char message[QUEUE_DATA_SIZE];
        memset(message, 0, QUEUE_DATA_SIZE);

        dequeue( com_qid, 1, message);

        char sign;
        memset(&sign,0xB1,sizeof(char) );  //a

        char sign_2;
        memset(&sign,0x61,sizeof(char) );

        if( memcmp(message,&sign,sizeof(char) ) == 0 )
            forward_to_supplydepot( message );
        if( memcmp(message,&sign_2,sizeof(char) ) == 0 )
            test( message );

    }





}

void forward_to_supplydepot( char message[] )
{
    char send_buf[BUFFER_SIZE];
    memset(send_buf,0,BUFFER_SIZE);
    memcpy( send_buf, message+(sizeof(char)*3), BUFFER_SIZE );
    send( sd_connfd, send_buf, BUFFER_SIZE-3, 0 );
    printf("Sended to SupplyDeport: %s",send_buf);
}

void test( char message[] )
{
    char send_buf[BUFFER_SIZE];
    memset(send_buf,0,BUFFER_SIZE);
    memcpy( send_buf, message+(sizeof(char)*3), BUFFER_SIZE );
    send( sd_connfd, send_buf, BUFFER_SIZE-3, 0 );
    printf("Sended to test: %s",send_buf);
}

