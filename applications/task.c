#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>

#include "./include/task.h"
#include "./include/mqtt.h"
#include "../driver/include/driver.h"

/**************线程参数设置*******************/
pthread_t mqttPubThread, flashLedThread;
struct sched_param mqttPubParam, flashLedParam;
pthread_attr_t mqttPubAttr, flashLedAttr;

/**
 * @brief MQTT发送线程
 *
 * @param arg
 */
void *MqttPublishThread_Entry(void *arg)
{
    while (1)
    {
        MqttPublish("/test", "hello world");
        sleep(4);
    }
}

void *FlashLedThread_Entry(void *arg)
{
    uint8_t status = 0;
    while (1)
    {
        status = !status;
        FlashLedWrite(status);
        sleep(1);
    }
}

int TaskInit()
{
    int res;
    // Mqtt发送线程
    pthread_attr_init(&mqttPubAttr);
    mqttPubParam.sched_priority = 10; // mqtt发送的线程优先级为10
    pthread_attr_setinheritsched(&mqttPubAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&mqttPubAttr, SCHED_RR); //时间片轮询
    pthread_attr_setstacksize(&mqttPubAttr, 2048);       //设置栈大小

    if (!pthread_attr_setschedparam(&mqttPubAttr, &mqttPubParam))
    {
        printf("mqttPubThread Set Success\n");
    }

    res = pthread_create(&mqttPubThread, &mqttPubAttr, MqttPublishThread_Entry, NULL);
    if (res != 0)
    {
        printf("create mqtt thread thread fail\r\n");
        return -1;
    }

    //呼吸灯线程
    pthread_attr_init(&flashLedAttr);
    flashLedParam.sched_priority = 5; // 呼吸灯线程为5
    pthread_attr_setinheritsched(&flashLedAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&flashLedAttr, SCHED_RR); //时间片轮询
    pthread_attr_setstacksize(&flashLedAttr, 1024);       //设置栈大小

    if (!pthread_attr_setschedparam(&flashLedAttr, &flashLedParam))
    {
        printf("flashLed thread Set Success\n");
    }

    res = pthread_create(&flashLedThread, &flashLedAttr, FlashLedThread_Entry, NULL);
    if (res != 0)
    {
        printf("create flashled thread thread fail\r\n");
        return -1;
    }

    pthread_join(mqttPubThread, NULL);
    pthread_join(flashLedThread, NULL);
    pthread_attr_destroy(&mqttPubAttr);
    pthread_attr_destroy(&flashLedAttr);

    return 0;
}