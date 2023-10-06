#pragma once

#include <mqtt_client.h>

extern const char *MQTT_TAG, *TAG_ID;

void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri);
void publish_message(esp_mqtt_client_handle_t client, const char *topic, const char *message);