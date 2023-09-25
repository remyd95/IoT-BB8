#pragma once

#include <mqtt_client.h>

extern const char *TAG, *TAG_ID;

typedef void (*MotorActionCallback)(int); 

void set_motor_callback(MotorActionCallback callback, const char* action);
void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri);