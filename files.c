#include "include.h"


//文件存在及权限检查函数，检查 参数，数据，设备 文件是否存在，如存在检查权限是否可写，不存在会自动创建
int files_check(void)
{
    if( every_file_check("data") == false ||
        every_file_check("device") == false ||
        every_file_check("parameter") == false||
        every_file_check(CONFIG_FILE) == false  )
         return -1;
    else return 1;
}

bool every_file_check(char path[])
{
    if( access(path,0) != 0 )
    {
        creat(path,0777);
        return true;
    }
    else
    {
        if( access(path,2) != 0 )
        {
            syslog(LOG_ERR,"Cannot write \"%s\" file %s", path, strerror(errno) );
            return false;
        }
        else return true;
    }
}

/*
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
*/

void get_config_file(void)
{
    char buffer[1024];
    FILE *conf=fopen(CONFIG_FILE, "r");
    if( conf == NULL )
    {
        syslog(LOG_ERR, "Config file is blank.");
        exit(EXIT_FAILURE);
    }

    while( !feof(conf) )
    {
        memset(buffer, 0, sizeof(buffer) );
        fgets(buffer, sizeof(buffer)-1, conf);
        syslog(LOG_INFO, "Config file a line: %s", buffer);
        char tmp='#';
        if( memcmp( &tmp, buffer, 1) == 0 ) continue;
        else
        {
            char interface[]="interface";
            if( memcmp(interface,buffer,9) == 0 )
            {
                sscanf(buffer,"interface=%s",config.interface);
            }
            continue;
        }
    }

    if( config.interface == NULL

        )
    {
        syslog(LOG_ERR, "Configuration error.");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Read configuration succeed.");

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




