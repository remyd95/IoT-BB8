#pragma once

#include <mqtt_client.h>

extern const char *MQTT_TAG, *TAG_ID;

void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri);
void mqtt_publish_message(esp_mqtt_client_handle_t client, const char *message);