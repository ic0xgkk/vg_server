#include "include.h"

uint32_t com_qid;
int32_t sd_connfd;
struct device device_data[DEVICE_AMOUNT];
void *data;

int main(void)
{
  openlog ("vg_server", LOG_PERROR|LOG_CONS, 0);

  syslog (LOG_INFO, "\n---------------------------------------------------------");
  syslog (LOG_INFO, "\n                    Vehicle God Server                   ");
  syslog (LOG_INFO, "\n                           车神                           ");
  syslog (LOG_INFO, "\n*********************************************************");
  syslog (LOG_INFO, "\n[*]vg_server Starting... \n");

  //创建消息队列
  com_qid=create_queue( (randomizer() )%65535 );

  data=(void *)malloc(DATA_BUFFER_SIZE*1024);

  if ( files_check() == -1 )
  {
    syslog (LOG_ERR, "文件无法访问\n");
    exit(EXIT_FAILURE);
  }

  if( read_device() != 1 )
  {
    syslog (LOG_ERR, "设备信息表读取错误\n");
    exit(EXIT_FAILURE);
  }

  create_socket();
  return 0;
}


