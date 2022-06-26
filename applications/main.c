#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#include "./include/mqtt_config.h"
#include "../package/mymqtt/common/mqtt_log.h"
#include "../package/mymqtt/mqttclient/mqttclient.h"
#include "../package/cjson/cJSON.h"
#include "../driver/include/flashled.h"
#include "../driver/include/serial.h"

static void topic1_handler(void *client, message_data_t *msg)
{
    (void)client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");

    cJSON *cjson = cJSON_Parse((char *)(msg->message->payload));
    if (cjson != NULL)
    {
        int isStart = cJSON_GetObjectItem(cjson, "isStart")->valueint; //获取isStart
        MQTT_LOG_I("isStart:%d\n", isStart);
    }
    else
    {
        MQTT_LOG_I("Json Formate Error\n");
    }

    cJSON_Delete(cjson); //释放cjson
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    mqtt_client_t *client = (mqtt_client_t *)arg;

    char buf[100] = {0};
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    sleep(2);

    mqtt_list_subscribe_topic(client);

    msg.payload = (void *)buf;

    while (1)
    {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());

        msg.qos = 0;
        mqtt_publish(client, "/test", &msg);

        sleep(4);
    }
}

void *led_flash_thread(void *arg)
{
    uint8_t status = 0;
    while (1)
    {
        status = !status;
        FlashLedWrite(status);
        SerialWrite("hello world\n");
        sleep(1);
    }
}
int main()
{
    int res;
    pthread_t mqttThread;
    pthread_t flashThread;
    struct sched_param mqttParam, flashParam;
    pthread_attr_t mqttAttr, flashAttr;
    mqtt_client_t *client = NULL;

    char client_id[32] = "yanke pc";
    char user_name[32] = "yanke1";
    char password[32] = "123456";
    printf("\nwelcome to mqttclient test...\n");
    mqtt_log_init();
    client = mqtt_lease();

    //设置MQTT参数
    mqtt_set_host(client, "101.43.5.182");
    mqtt_set_port(client, "1883");
    mqtt_set_client_id(client, client_id);
    mqtt_set_user_name(client, user_name);
    mqtt_set_password(client, password);
    mqtt_set_clean_session(client, 1);

    //连接
    mqtt_connect(client);

    mqtt_subscribe(client, "/recive", QOS0, topic1_handler);

    // led初始化
    res = FlashLedInit();
    if (res < 0)
    {
        printf("led init failed\n");
    }

    //串口初始化
    res = SerialInit();
    if (res < 0)
    {
        printf("serial init failed\n");
    }

    // MQTT线程
    pthread_attr_init(&mqttAttr);
    mqttParam.sched_priority = 10;
    pthread_attr_setinheritsched(&mqttAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&mqttAttr, SCHED_RR); //时间片轮询
    if (pthread_attr_setschedparam(&mqttAttr, &mqttParam) == 0)
    {
        printf("set success\n");
    }
    res = pthread_create(&mqttThread, &mqttAttr, mqtt_publish_thread, client);
    if (res != 0)
    {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    //呼吸灯线程
    pthread_attr_init(&flashAttr);
    pthread_attr_setstacksize(&flashAttr, 1024); //设置栈大小
    flashParam.sched_priority = 5;
    pthread_attr_setinheritsched(&flashAttr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&flashAttr, SCHED_RR); //时间片轮询
    if (pthread_attr_setschedparam(&flashAttr, &flashParam) == 0)
    {
        printf("set success\n");
    }
    res = pthread_create(&flashThread, &flashAttr, led_flash_thread, NULL);
    if (res != 0)
    {
        MQTT_LOG_E("create flash led thread fail");
        exit(res);
    }

    printf("start up\n");

    pthread_join(flashThread, NULL);
    pthread_join(mqttThread, NULL);
    pthread_attr_destroy(&mqttAttr);
    pthread_attr_destroy(&flashAttr);
    return 0;
}