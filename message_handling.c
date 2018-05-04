#include "include.h"

void message_handling(void)
{
//    uint16_t connection_sum=rows("device");
//    memset( &forward_lock, 0, sizeof(uint8_t)*connection_sum );

    char message[QUEUE_DATA_SIZE];
    dequeue( qid, 1, message);

    char sign;
    memcpy( &sign, message, sizeof(char)*2 );

    switch( sign )
    {
        case 0xB1:
        {

        }

    }



}

void forward_to_supplydepot()
{

}
