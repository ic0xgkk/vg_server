#include "include.h"


//文件存在及权限检查函数，检查 参数，数据，设备 文件是否存在，如存在检查权限是否可写，不存在会自动创建
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
            syslog(LOG_ERR,"Cannot write \"parameter\" file %s", strerror(errno) );
            perror("ERROR: ");
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
            syslog(LOG_ERR,"Cannot write \"data\" file %s", strerror(errno) );
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
            syslog(LOG_ERR,"Cannot write \"device\" file %s", strerror(errno) );
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




