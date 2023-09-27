#pragma once

#include <mqtt_client.h>
#include "motor_action_data.h"

extern const char *TAG, *TAG_ID;

typedef void (*MotorActionCallback)(motor_action_data_t); 

static float clip_duty_cycle(float duty_cycle);
void set_motor_callback(MotorActionCallback callback, const char* action);
void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri);