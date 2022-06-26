#include "./include/mqtt.h"
#include "./include/mqtt_config.h"
#include "../package/mymqtt/common/mqtt_log.h"
#include "../package/mymqtt/mqttclient/mqttclient.h"
#include "../package/cjson/cJSON.h"

static mqtt_client_t *client = NULL;
mqtt_message_t msg;
/**
 * @brief 主题回调
 *
 * @param client
 * @param msg
 */
static void TopicHandler(void *client, message_data_t *msg)
{
    (void)client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%s\r\n", msg->topic_name, (char *)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void MqttInit(char *subTopic)
{

    char client_id[32] = "imx6ull";
    char user_name[32] = "yanke1";
    char password[32] = "123456";
    mqtt_log_init();
    client = mqtt_lease();

    //设置MQTT参数
    mqtt_set_host(client, "101.43.5.182");
    mqtt_set_port(client, "1883");
    mqtt_set_client_id(client, client_id);
    mqtt_set_user_name(client, user_name);
    mqtt_set_password(client, password);
    mqtt_set_clean_session(client, 1);

    //连接主机与订阅
    mqtt_connect(client);
    mqtt_subscribe(client, subTopic, QOS0, TopicHandler);

    memset(&msg, 0, sizeof(msg));
}

/**
 * @brief MQTT发送数据
 *
 * @param topic
 * @param data
 */
void MqttPublish(char *topic, char *data)
{
    msg.payload = (void *)data;
    mqtt_publish(client, topic, &msg);
}