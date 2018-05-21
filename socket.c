#include "include.h"

//socket创建函数
void create_socket(void)
{
  int ConnectFD = 0;

  struct sockaddr_in stSockAddr;
  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(LISTEN_PORT);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  pthread_t thread;       //创建子线程以处理不同的客户端
  pthread_t mq_forward;
  char cli_ip[INET_ADDRSTRLEN];
  int SocketFD= before_accept(stSockAddr);
  struct sockaddr_in client_addr;
  socklen_t addr_lenth=32;
  char host[NI_MAXHOST],service_port[NI_MAXSERV];

  //分出来一个线程去处理消息队列
  pthread_create(&mq_forward, NULL, (void *)message_handling, NULL );
  syslog(LOG_INFO,"[*]Message Queue Thread Created.\n");

  while(1)
  {
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    memset(host, 0, sizeof(host));
    memset(service_port, 0, sizeof(service_port));
    memset(cli_ip, 0, sizeof(cli_ip));
    ConnectFD = accept(SocketFD,(struct sockaddr *)&client_addr,&addr_lenth);
    if(ConnectFD < 0)
    {
      syslog(LOG_ERR,"Error accept failed: %s", strerror(errno) );
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    if(ConnectFD > 0)
    {
            //由于同一个进程内的所有线程共享内存和变量，因此在传递参数时需作特殊处理值传递
            if (getnameinfo( (const struct sockaddr *)&client_addr,32, host, NI_MAXHOST,service_port,NI_MAXSERV, NI_NUMERICSERV) == 0)
                syslog(LOG_INFO,"Thread Created: Accept client from %s:%s \n", host, service_port);

//            if( client_addr.sin_addr.s_addr == 838969536 )     //补给站IP需要为192.168.1.50
//                sd_connfd = ConnectFD;

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
}


void print_unknow_ori(uint8_t ori)
{
    syslog(LOG_WARNING,"Unknow ORI sign: 0x%x.Please check the data format.\n",ori);
}

int before_accept(struct sockaddr_in stSockAddr)
{
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if(-1 == SocketFD)
  {
    syslog(LOG_ERR,"Can not create socket %s \n", strerror(errno));
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if( bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) == -1 )
  {
    syslog(LOG_ERR,"Error bind failed %s \n", strerror(errno));
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else syslog(LOG_INFO,"[*]Bind Success.\n");


  if( listen(SocketFD, 128) == -1 )
  {
    syslog(LOG_ERR,"Error listen failed %s \n", strerror(errno));
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  else syslog(LOG_INFO,"[*]Listen Success.\n");
  return SocketFD;
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
            syslog(LOG_INFO, "Received data: %s \n", buffer );

            //重头戏：消息入列
            enqueue(com_qid, 1, buffer);

            syslog(LOG_INFO, "Enqueue Done.\n");
        }
        else
        {
            syslog(LOG_INFO, "Thread Closed.\n");
            shutdown(connfd, SHUT_RDWR);
            close(connfd);
            break;
        }
    }
}

