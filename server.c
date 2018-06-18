#include "include.h"

uint32_t com_qid;
int32_t sd_connfd;
struct device device_data[DEVICE_AMOUNT];
void *data;
struct read_config config;

int main(void)
{
  openlog ("vg_server", LOG_PERROR|LOG_CONS, 0);
  syslog (LOG_INFO, "Version 2.0.1 ( https://zhbit.robomaster.team )");
  syslog (LOG_INFO, "Server is starting.");

  com_qid=create_queue( (randomizer() )%65535 );     //创建消息队列
  data=(void *)malloc(DATA_BUFFER_SIZE*1024);
  get_config_file();

  if ( files_check() == -1 )
  {
    syslog (LOG_ERR, "Failed to read file.");
    exit(EXIT_FAILURE);
  }

/*
  if( read_device() != 1 )
  {
    syslog (LOG_ERR, "Failed to read device table.");
    exit(EXIT_FAILURE);
  }
*/

  create_socket();
  return 0;
}


