#include "include.h"

int main(void)
{
  printf("\n---------------------------------------------------------");
  printf("\n                 Vehicle God Data Center                 ");
  printf("\n                       车神数据中心                        ");
  printf("\n*********************************************************\n");


  qid=create_queue( (randomizer() )%65535 );
  for(int i=0;i<1000;i++)
  {
    enqueue(qid,1,"najsnajsajdgashjdgashjgdjkasgsdklhfgsdjkhfgjkshfgksdhgjksdbgjksdbjkfgbsbfsjkdbfjksdbgjksdjksaghdjk");
    //qid++;
  }









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
  pthread_t mq_forward;
  char buffer[256];
  memset(buffer, 0, sizeof(buffer));

  int SocketFD= before_accept(stSockAddr);

  struct sockaddr_in client_addr;

  socklen_t addr_lenth=16;

  char host[NI_MAXHOST],service_port[NI_MAXSERV];

  //分出来一个线程去处理消息队列
  pthread_create(&mq_forward, NULL, (void *)message_handling, NULL );

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

            //重头戏：消息入列
            enqueue(qid, 1, buffer);

            //uint8_t status=data_manipulation(buffer);

            //判断帧结构是否符合定义和处理状态
            /*
            if( status != 1 )
            {
                if( status == -2 ) continue;
            }
            */

            printf("Enqueue Done.\n");

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
    //标志位分离
    uint8_t orientation_sign=(uint8_t)buffer[0];

    switch( orientation_sign )
    {
        case 0xB1: //转发到补给站
        {
            break;
        }

        default:
        {
            print_unknow_ori( orientation_sign );
            return -2;
        }

    }

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
void print_unknow_ori(uint8_t ori)
{
    printf("WARNING: Unknow ORI sign: 0x%x.Please check the data format.\n",ori);
}

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




