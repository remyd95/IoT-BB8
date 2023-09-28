#pragma once

#include <mqtt_client.h>

extern const char *TAG, *TAG_ID;

void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri);