#include "include.h"

//随机数生成
uint32_t randomizer(void)
{
    srand( (uint32_t)time(NULL)+rand() );
    return rand();
}
