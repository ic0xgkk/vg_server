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
        perror("ERROR: Can not open file: ");
        return -1;
    }
    while (fgets(buf, 4096, pf)) // fgets循环读取，直到文件最后，才会返回NULL
        lineCnt++; // 累计行数
    fclose(pf);
    printf("file line count = %d\n", lineCnt);
    return lineCnt;
}
