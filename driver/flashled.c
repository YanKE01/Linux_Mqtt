#include "./include/flashled.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int flashFd = 0;

/**
 * @brief 呼吸灯初始化
 * 
 * @return int 
 */
int FlashLedInit()
{
    flashFd = open("/dev/led", O_RDWR);
    if (flashFd < 0)
    {
        printf("flash led init failed\n");
        return -1;
    }

    return 0;
}


/**
 * @brief 呼吸灯设置
 * 
 * @param value 
 * @return int 
 */
int FlashLedWrite(unsigned char value)
{
    int ret = 0;
    ret = write(flashFd, &value, 1);
    if (ret < 0)
    {
        printf("flash led set error\n");
        close(flashFd);
        return -1;
    }

    return 0;
}