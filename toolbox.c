#include "include.h"

//随机数生成
uint32_t randomizer(void)
{
    srand( (uint32_t)time(NULL)+rand() );
    return rand();
}

//文件行数读取
int16_t rows(char file_name[])
{
    FILE *pf = fopen( file_name, "r"); //打开文件
    char buf[4096];
    int16_t lineCnt = 0;
    if (!pf) // 判断是否打开成功
    {
        syslog(LOG_ERR, "Can not open file %s\n",strerror(errno));
        return -1;
    }
    while (fgets(buf, 4096, pf)) // fgets循环读取，直到文件最后，才会返回NULL
        lineCnt++; // 累计行数
    fclose(pf);
    return lineCnt;
}

size_t file_size(char filename[])
{
    struct stat statbuf;
    stat(filename,&statbuf);
    size_t size=statbuf.st_size;
    return size;
}

/*
int Storage(char msg[])
{

}
*/
