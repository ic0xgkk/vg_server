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
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

#define BUFFER_SIZE 1024   //TCP接受的缓冲区大小

void server_func(void *args);

int main(void)
{


  printf("\n---------------------------------------------------------");
  printf("\n                 Vehicle God Data Center                 ");
  printf("\n*********************************************************\n");

  static struct sockaddr_in stSockAddr;
  time_t tNow = 0;
  int ConnectFD=0;
  pthread_t thread;       //创建不同的子线程以区别不同的客户端
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(-1 == SocketFD)
  {
    perror("ERROR:Can not create socket ");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(6666);  //Listen port : 6666/tcp
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if( bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) == -1)
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

  char buffer[256];
  memset(buffer, 0, sizeof(buffer));
  static struct sockaddr_in client_addr;
  socklen_t addr_lenth;
  char host[NI_MAXHOST],service_port[NI_MAXSERV];
  while(1)
  {
    time(&tNow);
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    memset(host, 0, sizeof(host));
    memset(service_port, 0, sizeof(service_port));
    ConnectFD = accept(SocketFD,(struct sockaddr *)&client_addr,&addr_lenth );
    if(ConnectFD < 0)
    {
      perror("ERROR:Error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    if(ConnectFD > 0)
    {
            //由于同一个进程内的所有线程共享内存和变量，因此在传递参数时需作特殊处理，值传递。
            if (getnameinfo( (const struct sockaddr *)&client_addr,16, host, NI_MAXHOST,service_port,NI_MAXSERV, NI_NUMERICSERV) == 0) printf("Thread Created: Accept client from %s:%s \n", host, service_port);
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
    char host_pt[NI_MAXHOST],service_port_pt[NI_MAXSERV];
    int connfd;
    socklen_t addr_lenth_pt;
    struct sockaddr_in cli_addr;
    memcpy(&connfd,args,sizeof(int) );
    memcpy(&cli_addr,args+sizeof(int),sizeof(struct sockaddr) );
    memcpy(&addr_lenth_pt,args+sizeof(int)+sizeof(struct sockaddr) ,sizeof(socklen_t) );

    if( getnameinfo( (struct sockaddr *)&cli_addr,16, host_pt, NI_MAXHOST,service_port_pt,NI_MAXSERV, NI_NUMERICSERV) == 0)
        printf("Reviced %lu bytes from %s:%s \n", (long)recvfrom(connfd, buffer, BUFFER_SIZE, 0,(struct sockaddr *) &cli_addr, &addr_lenth_pt) , host_pt, service_port_pt );


    while(1)
    {
        while( recv(connfd, buffer, sizeof(buffer),0) > 0)
        {
            printf("Buffer data: %s\n", buffer);
        }
    }
}




