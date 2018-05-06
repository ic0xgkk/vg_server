#include "include.h"

//创建队列同时进行检查
int32_t create_queue(uint32_t msgkey)
{
    int32_t qid_tmp=msgget(msgkey, IPC_CREAT|IPC_EXCL | 0777);
    if( qid_tmp < 0 )
    {
        syslog(LOG_ERR,"Message Queue Existed %s", strerror(errno) );
        exit(EXIT_FAILURE);
    }
    else
        return qid_tmp;
}

//消息入列
void enqueue(int qid, int msgtype,char enqueue_msg[])
{
    struct mq_buff msg;
    msg.mtype = msgtype;

    snprintf(msg.mtext, sizeof(msg.mtext), "%s", enqueue_msg);

    if (msgsnd(qid, (void *) &msg, sizeof(msg.mtext),IPC_NOWAIT) == -1)
    {
        syslog(LOG_ERR,"Msgsnd error %s", strerror(errno) );
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO,"Message Enqueue success.\n");
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
                syslog(LOG_ERR,"msgrcv error %s", strerror(errno) );
                exit(EXIT_FAILURE);
            }
            continue;
        }
        else
        {
            syslog(LOG_INFO,"message received: %s\n",msg.mtext );
            strcpy( msg_dequeue, msg.mtext );
            break;
        }
    }
}



