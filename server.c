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

int main(void)
{


  printf("\n---------------------------------------------------------");
  printf("\n                 Vehicle God Data Center                 ");
  printf("\n*********************************************************\n");

  static struct sockaddr_in stSockAddr;
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
  /*
    pthread_t thread;       //创建不同的子线程以区别不同的客户端
    client_sockfd = (int *)malloc(sizeof(int));
    client_len = sizeof(client_addr);
    *client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_addr, (socklen_t *)&client_len);
    if(-1==*client_sockfd){
      perror("accept");
      continue;
    }
    if(pthread_create(&thread, NULL, rec_data, client_sockfd)!=0)//创建子线程
    {
      perror("pthread_create");
      break;
    }
   }
       shutdown(*client_sockfd,2);
       shutdown(server_sockfd,2);

*/






    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    memset(host, 0, sizeof(host));
    memset(service_port, 0, sizeof(service_port));
    int ConnectFD = accept(SocketFD,(struct sockaddr *)&client_addr,&addr_lenth );
    if(ConnectFD < 0)
    {
      perror("ERROR:Error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    while(1)
    {
    ssize_t nread = recvfrom(ConnectFD, buffer, 255, 0,(struct sockaddr *) &client_addr, &addr_lenth);
    if (getnameinfo( (struct sockaddr *)&client_addr,16, host, NI_MAXHOST,service_port,NI_MAXSERV, NI_NUMERICSERV) == 0)
            printf("Received %ld bytes from %s:%s : %s\n",
                    (long) nread, host, service_port,buffer);
    }
    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
  }

 close(SocketFD);
  return 0;
}
