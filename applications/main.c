#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>

#include "./include/mqtt.h"
#include "./include/task.h"
#include "../driver/include/driver.h"

int main()
{
    DriverInit();        //板载外设初始化
    MqttInit("/recive"); // MQTT初始化
    TaskInit();          //线程初始化
    return 0;
}