#include "include.h"

//创建队列同时进行检查
int32_t create_queue(uint32_t msgkey)
{
    int32_t qid_tmp=msgget(msgkey, IPC_CREAT|IPC_EXCL | 0777);
    if( qid_tmp < 0 )
    {
        printf("ERROR: Message Queue Existed.\n");
        exit(EXIT_FAILURE);
    }
    else
        return qid_tmp;
    //return msgget(msgkey, IPC_CREAT | 0777);
    //pause();
}

//消息入列
void enqueue(int qid, int msgtype,char enqueue_msg[])
{
    struct mq_buff msg;
//    time_t t;

    msg.mtype = msgtype;

//    time(&t);
    snprintf(msg.mtext, sizeof(msg.mtext), "%s", enqueue_msg);


    if (msgsnd(qid, (void *) &msg, sizeof(msg.mtext),IPC_NOWAIT) == -1)
    {
        perror("ERROR: msgsnd error ");
        exit(EXIT_FAILURE);
    }
    printf("Message Enqueue success.\n");
}

//消息出列
void dequeue(int qid, int msgtype, char *msg_dequeue)
{
    struct mq_buff msg;

    while(1)
    {
        if (msgrcv(qid, (void *) &msg, sizeof(msg.mtext), msgtype,MSG_NOERROR | IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                perror("ERROR: msgrcv");
                exit(EXIT_FAILURE);
            }
            //printf("No message available for msgrcv()\n");
            continue;
        }
        else
        {
            printf("message received: %s\n", msg.mtext);

            strcpy( msg_dequeue, msg.mtext );
            //memcpy( msg_dequeue, msg.mtext, sizeof(msg.mtext));
            break;
        }
    }
}



