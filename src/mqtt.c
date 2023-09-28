#include "mqtt.h"
#include "action_handler.h"

#include <stdio.h>
#include <esp_log.h>


const char *TAG = "controller0/motor";


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)handler_args;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");

            esp_mqtt_client_subscribe(client, TAG, 0);

            //TODO: Send your own random controller ID to the base station 

            //TODO: Then receive an update of your current position from the basestation, then wait idle.

            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");
            break;
        
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT data received");

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            if (strncmp(event->topic, TAG, strlen(TAG)) == 0) {
                process_action(event->data);
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