#include "mqtt.h"
#include "action_handler.h"

#include <stdio.h>
#include <esp_log.h>
#include <time.h>

const char *MQTT_TAG = "MQTT";

char BALL_NAME[12];
char ACTION_TOPIC[30];

static bool mqtt_client_connected = false;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)handler_args;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT connected");

            mqtt_client_connected = true; 

            srand(time(NULL));
            // Generate a random 6-digit ID
            int random_id = rand() % 900000 + 100000;

            char id_str[8];
            snprintf(id_str, sizeof(id_str), "%d", random_id);

            // Register ball to base station
            esp_mqtt_client_publish(client, "register", id_str, 0, 0, 0);

            snprintf(BALL_NAME, sizeof(BALL_NAME), "ball%d", random_id);

            // Subscribe to the "ball<ID>/action" topic
            snprintf(ACTION_TOPIC, sizeof(ACTION_TOPIC), "%s/action", BALL_NAME);
            esp_mqtt_client_subscribe(client, ACTION_TOPIC, 0);

            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT disconnected");
            mqtt_client_connected = false; 
            break;
        
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT data received");

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            if (strncmp(event->topic, ACTION_TOPIC, strlen(MQTT_TAG)) == 0) {
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

void mqtt_publish_message(esp_mqtt_client_handle_t client, const char *message) {
     if (mqtt_client_connected) {
        char state_topic[30];
        snprintf(state_topic, sizeof(state_topic), "%s/state", BALL_NAME);
        esp_mqtt_client_publish(client, state_topic, message, 0, 0, 0);
    }
}
