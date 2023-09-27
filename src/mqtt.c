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

static float clip_duty_cycle(float duty_cycle) {
    float bounded_duty_cycle = 0;

    if (duty_cycle < 0) {
        bounded_duty_cycle = 0;
    } else if (duty_cycle > 100) {
        bounded_duty_cycle = 100;
    } else {
        bounded_duty_cycle = duty_cycle;
    }
    return bounded_duty_cycle;
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

                motor_action_data_t motor_action_data;
                if (strncmp((char *)event->data, "FW", 2) == 0) {
                    float duty_cycle;

                    if (sscanf((char *)event->data, "FW %f", &duty_cycle) == 1) {
                        float bounded_duty_cycle = clip_duty_cycle(duty_cycle);
                        motor_action_data.duty_cycle = bounded_duty_cycle;
                        forward_callback(motor_action_data);
                    }
                } else if (strncmp((char *)event->data, "BW", 2) == 0) {
                    float duty_cycle;

                    if (sscanf((char *)event->data, "BW %f", &duty_cycle) == 1) {
                        float bounded_duty_cycle = clip_duty_cycle(duty_cycle);
                        motor_action_data.duty_cycle = bounded_duty_cycle;
                        backward_callback(motor_action_data);
                    }
                } else if (strncmp((char *)event->data, "ST", 2) == 0) {
                    stop_callback(motor_action_data);
                } else if (strncmp((char *)event->data, "POS", 3) == 0) {
                    float x, y;
                    if (sscanf((char *)event->data, "POS %f %f", &x, &y) == 2) {
                        //move_to(x, y);
                    }
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