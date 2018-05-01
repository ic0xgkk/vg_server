#include "include.h"

//初始化消息队列的缓冲区
void initialize_queue(void)
{
    for( uint32_t i=0;i<MESSAGE_QUEUE_LENTH;i++ )
    {
        msg_queue[i].status=0;
        memset( msg_queue , 0, sizeof(struct queue_buffer)*MESSAGE_QUEUE_LENTH );
    }

    //int tmp_size=sizeof(blank);
    memset( &blank, 0xFF, 1);
    //printf("%u,%u,%u\naaaaa",queue_location[1],queue_location[2],queue_location[0]);
}

//消息入列
void queue_enqueue(char msg[])
{
    uint32_t enqueue_loc_old = enqueue_loc;
    uint32_t cid;
    memcpy( &cid, msg+1, sizeof(uint32_t) );
    if( enqueue_loc == MESSAGE_QUEUE_LENTH-1 ) enqueue_loc = 0;
    else enqueue_loc++;

    while(1)
    {
        if( memcmp( &msg_queue[enqueue_loc].queue_data, &blank, 1) == 0 ) break;
        else
        {
            enqueue_loc++;
            if( enqueue_loc == enqueue_loc_old )
            {
                printf("ERROR: Queue Buffer Overflow. \n");
            }
        }
    }
    strcpy(msg_queue[enqueue_loc].queue_data,msg);
}

uint8_t queue_dequeue(char *msg)
{

}
