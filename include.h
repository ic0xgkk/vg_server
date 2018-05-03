#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/mman.h>  //内存文件映射
#include <pthread.h>   //多线程
#include <time.h>   //系统API读取UTC时间
#include <sys/time.h>
#include <malloc.h>
#include <sys/stat.h>    //文件大小判断
#include <fcntl.h>   //文件检查需要用到
#include <sys/ipc.h>
#include <sys/msg.h>   //使用Linux自带的消息队列
#include <errno.h>

//全局定义区域
#define BUFFER_SIZE 1024   //TCP接受的缓冲区大小
#define LISTEN_PORT 6666   //监听端口
#define DEVICE_AMOUNT 32   //最大设备数量上限（即CID上限）
#define QUEUE_DATA_SIZE BUFFER_SIZE+16   //消息队列数据分配空间
#define MESSAGE_QUEUE_LENTH 2048   //消息队列的最大长度

static struct device{
    char ip[16];
    uint32_t cid;
}device_data[DEVICE_AMOUNT];

//Linux MQ
struct mq_buff{
    long mtype;
    char mtext[80];
};

/*
//消息队列
static struct queue_buffer{
    uint8_t status;
    char queue_data[QUEUE_DATA_SIZE];
}msg_queue[MESSAGE_QUEUE_LENTH];
*/


//static uint32_t msgkey=1000;

static uint32_t enqueue_loc=1;  //入列标识
static uint32_t dequeue_loc=1;  //出列标识

static uint32_t qid=0;

static char blank;

//自定义函数区域
void server_func(void *args);
int files_check(void);
int data_manipulation(char buffer[BUFFER_SIZE]);
size_t file_size(char* filename);
int read_device(void);
int before_accept(struct sockaddr_in stSockAddr);
void print_unknow_ori(uint8_t ori);
void enqueue(int qid, int msgtype,char msg[]);
void get_msg(int qid, int msgtype);


int32_t create_queue(uint32_t msgkey);
uint32_t randomizer(void);





