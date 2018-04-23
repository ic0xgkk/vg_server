/* 作者：雪糕 <ic0xgkk@gmail.com>
   首页：www.xuegaogg.com
   关于本程序： 本程序主要用于DJI的RoboMaster的机器人调参使用，传统的蓝牙调参和无线串口调参问题参差不齐，
            无线串口丢包严重，并且无法存储数据，蓝牙调参范围太小，稳定性也不高，故可以使用本程序搭配一个E
            SP8266和STM32和android终端来进行无线调参，还算是方便点吧。
              本程序为服务端，推荐自主编译openwrt或者lede，将其rootfs转移到SD卡分区，然后安装gcc和mak
            e等相关依赖，将服务端启动在嵌入式linux设备上边将会是一个划算又省事的方案。
*/
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

static struct device{
    char ip[16];
    int cid;
}device_data[DEVICE_AMOUNT];


//自定义函数区域
void server_func(void *args);
int files_check(void);
int data_manipulation(char buffer[BUFFER_SIZE]);
size_t file_size(char* filename);
int read_device(void);
int before_accept(struct sockaddr_in stSockAddr);


int main(void)
{
  printf("\n---------------------------------------------------------");
  printf("\n                 Vehicle God Data Center                 ");
  printf("\n                       车神数据中心                        ");
  printf("\n*********************************************************\n");

  if ( files_check() == -1 )
  {
    exit(EXIT_FAILURE);
  }

  if( read_device() != 1 )
  {
    printf("ERROR: Read device failed.\n");
    exit(EXIT_FAILURE);
  }

  time_t tNow = 0;
  int ConnectFD = 0;

  struct sockaddr_in stSockAddr;
  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(LISTEN_PORT);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  pthread_t thread;       //创建不同的子线程以区别不同的客户端

  char buffer[256];
  memset(buffer, 0, sizeof(buffer));

  int SocketFD= before_accept(stSockAddr);

  struct sockaddr_in client_addr;

  socklen_t addr_lenth=16;

  char host[NI_MAXHOST],service_port[NI_MAXSERV];

  while(1)
  {
    time(&tNow);
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    memset(host, 0, sizeof(host));
    memset(service_port, 0, sizeof(service_port));
    ConnectFD = accept(SocketFD,(struct sockaddr *)&client_addr,&addr_lenth);
    if(ConnectFD < 0)
    {
      perror("ERROR:Error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    if(ConnectFD > 0)
    {
            //由于同一个进程内的所有线程共享内存和变量，因此在传递参数时需作特殊处理值传递
            if (getnameinfo( (const struct sockaddr *)&client_addr,16, host, NI_MAXHOST,service_port,NI_MAXSERV, NI_NUMERICSERV) == 0)
                printf("Thread Created: Accept client from %s:%s \n", host, service_port);
            void *args=(void *)malloc( sizeof(int)+sizeof(client_addr)+sizeof(socklen_t) );
            memcpy( &args,&ConnectFD,sizeof(int) );
            memcpy( &args+sizeof(int),&client_addr,sizeof(client_addr) );
            memcpy( &args+sizeof(int)+sizeof(client_addr) ,&addr_lenth,sizeof(addr_lenth) );
            pthread_create(&thread, NULL, (void *)server_func,&args );  //创建线程
            pthread_detach(thread); // 线程分离，结束时自动回收资源
    }
  }
  shutdown(ConnectFD, SHUT_RDWR);
  close(ConnectFD);
  close(SocketFD);
  return 0;
}

void server_func(void *args)
{
    char buffer[BUFFER_SIZE];
//    char host_pt[NI_MAXHOST],service_port_pt[NI_MAXSERV];
    int connfd;
    socklen_t addr_lenth_pt;
    struct sockaddr_in cli_addr;
    memcpy(&connfd,args,sizeof(int) );
    memcpy(&cli_addr,args+sizeof(int),sizeof(struct sockaddr) );
    memcpy(&addr_lenth_pt,args+sizeof(int)+sizeof(struct sockaddr) ,sizeof(socklen_t) );

    //int ss = getnameinfo( (struct sockaddr *)&cli_addr, 64, host_pt, NI_MAXHOST,service_port_pt,NI_MAXSERV, NI_NUMERICSERV);
        //printf("Reviced %lu bytes from %s:%s \n", (long)recvfrom(connfd, buffer, BUFFER_SIZE, 0,(struct sockaddr *) &cli_addr, &addr_lenth_pt) , host_pt, service_port_pt );

    while(1)
    {
        if( recv(connfd, buffer, sizeof(buffer),0) > 0)
        {
            printf("Buffer data: %s\n", buffer);
            if( data_manipulation(buffer) != 1 )
            {
                perror("ERROR: Data manipulat failed ");
            }
        }
        else
        {
            printf("Thread Closed.\n");
            shutdown(connfd, SHUT_RDWR);
            close(connfd);
            break;
        }
    }
}


/* 文件存在及权限检查函数，检查 参数，数据，设备 文件是否存在，如存在检查权限是否可写，不存在会自动创建 */
int files_check(void)
{
    if( access("parameter",0) != 0 )
    {
        creat("parameter",0777);
    }
    else
    {
        if( access("parameter",2) != 0 )
        {
            perror("ERROR: Cannot write parameter file ");
            return -1;
        }
    }

    if( access("data",0) != 0 )
    {
        creat("data",0777);
    }
    else
    {
        if( access("data",2) != 0 )
        {
            perror("ERROR: Cannot write data file ");
            return -1;
        }
    }

    if( access("device",0) != 0 )
    {
        creat("device",0777);
    }
    else
    {
        if( access("device",2) != 0 )
        {
            perror("ERROR: Cannot write device file ");
            return -1;
        }
    }
    return 1;
}

int read_device(void)
{
    FILE *fp=fopen("device","r");
    int i=0;
    while(!feof(fp))
    {
        fscanf(fp,"%s%d",device_data[i].ip, &device_data[i].cid );
        i++;
    }
    fclose(fp);
    return 1;

}

//报文第0位为ORI(方向位)，第1位为CID(设备标识符)，从第2位开始为数据位
int data_manipulation( char buffer[BUFFER_SIZE] )
{
    if( memcmp(buffer,0xB1,1) == 0 )
    {
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




    }

    if( buffer[0] == '2' )
    {
        printf("What the fuck????\n");
        exit(EXIT_FAILURE);
    }

    return 1;
}


/*
//对数据进行分离存储，以及对data文件进行内存映射
int data_manipulation( char buffer[BUFFER_SIZE] )
{
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;                              //对映射区域的更新给其他进程看，也同步到下层的更新
    int fd = open("data", O_RDWR);                  //以读写方式打开文件
    if( fd == -1 )
    {
        perror("ERROR: Failed to open data ");
        return -1;
    }
    size_t fsize=file_size("data");
    void *p = mmap(NULL, fsize+1024, prot, flags, fd, 0);     //从文件开始映射字节，flags为MAP_ANONYMOUS时不进行文件映射
    int aaa=(int)p;
    if(p == MAP_FAILED)
    {
        perror("ERROR: MMAP Failed ");
        return -1;
    }
    strcpy(p,"aaaaaaaaaaaaa");

   if( msync(p,fsize+1024,flags) == -1)
        printf("Fuck!!");
    close(fd);
    munmap(p,fsize+1024);





    return 0;
}

*/


int before_accept(struct sockaddr_in stSockAddr)
{
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(-1 == SocketFD)
  {
    perror("ERROR:Can not create socket ");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if( bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) == -1 )
  {
    perror("ERROR:Error bind failed ");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else printf("- Bind Success.\n");


  if( listen(SocketFD, 128) == -1 )
  {
    perror("ERROR:Error listen failed.\n");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  else printf("- Listen Success.\n");

  return SocketFD;
}

size_t file_size(char filename[])
{
    struct stat statbuf;
    stat(filename,&statbuf);
    size_t size=statbuf.st_size;
    return size;
}




