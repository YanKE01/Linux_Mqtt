#ifndef _MQTT_H
#define _MQTT_H


void MqttInit(char *subTopic);
void MqttPublish(char *topic, char *data);

#endif