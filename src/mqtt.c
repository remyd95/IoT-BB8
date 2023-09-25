#include "mqtt.h"

#include <stdio.h>
#include <string.h>
#include <esp_log.h>

const char *TAG = "controller0/motor";

static MotorActionCallback forward_callback = NULL;
static MotorActionCallback backward_callback = NULL;
static MotorActionCallback stop_callback = NULL;

void set_motor_callback(MotorActionCallback callback, const char* action) {
    if (strcmp(action, "forward") == 0) {
        forward_callback = callback;
    } else if (strcmp(action, "backward") == 0) {
        backward_callback = callback;
    } else if (strcmp(action, "stop") == 0) {
        stop_callback = callback;
    } 
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)handler_args;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");

            esp_mqtt_client_subscribe(client, TAG, 0);

            // RESET SOME FLAG TO RESET LED BLINK
            //xEventGroupSetBits(mqtt_event_group, CONNECTED_BIT);

            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");

            // MAKE LED BLINK TO WARN FOR MQTT FAILURE

            //xEventGroupClearBits(mqtt_event_group, CONNECTED_BIT);

            break;
        
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT data received");

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            if (strncmp(event->topic, TAG, strlen(TAG)) == 0) {
                if (strncmp((char *)event->data, "FW", event->data_len) == 0) {
                    forward_callback(1);
                    forward_callback(2);
                }
                else if (strncmp((char *)event->data, "BW", event->data_len) == 0) {
                    backward_callback(1);
                    backward_callback(2);
                } else if (strncmp((char *)event->data, "ST", event->data_len) == 0) {
                    stop_callback(1);
                    stop_callback(2);
                }
            }

            break;
        
        default:
            break;
    }
}


void init_mqtt(esp_mqtt_client_handle_t *client, const char* broker_uri) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = broker_uri,
        },
    };
    *client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(*client, ESP_EVENT_ANY_ID, mqtt_event_handler, *client);
    esp_mqtt_client_start(*client);
}