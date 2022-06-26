#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include "./include/mqtt_config.h"
#include "../package/mymqtt/common/mqtt_log.h"
#include "../package/mymqtt/mqttclient/mqttclient.h"

static void topic1_handler(void *client, message_data_t *msg)
{
    (void)client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
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

int main()
{
    int res;
    pthread_t thread1;
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

    mqtt_subscribe(client, "/test", QOS0, topic1_handler);

    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if (res != 0)
    {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    printf("start up\n");

    while (1)
    {
        sleep(100);
    }
    return 0;
}