#include <stdio.h>
#include "./include/driver.h"

/**
 * @brief 板载外设初始化
 *
 */
void DriverInit()
{
    int ret;
    ret = FlashLedInit();
    if (ret < 0)
    {
        printf("flashLed init failed\r\n");
    }
}