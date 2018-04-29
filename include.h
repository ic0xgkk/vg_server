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


//全局定义区域
#define BUFFER_SIZE 1024   //TCP接受的缓冲区大小
#define LISTEN_PORT 6666
#define DEVICE_AMOUNT 32
