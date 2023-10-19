#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <mqtt_client.h>
#include "state_machine.h"

extern const char *MQTT_TAG, *TAG_ID;
extern const int MQTT_CONNECTED_BIT;

typedef struct {
    EventGroupHandle_t* connection_event_group;
    esp_mqtt_client_handle_t* client;
} mqtt_client_data_t;

void init_mqtt(EventGroupHandle_t *connection_event_group, esp_mqtt_client_handle_t *client, const char* broker_uri);
void mqtt_publish_message(esp_mqtt_client_handle_t client, const char *message);
void identify_ball(esp_mqtt_client_handle_t client, char* event_data);