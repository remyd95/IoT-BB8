#include "mqtt.h"
#include "action_handler.h"

#include <stdio.h>
#include <esp_log.h>
#include <time.h>

const char *MQTT_TAG = "MQTT";
const int MQTT_CONNECTED_BIT = BIT1;

char BALL_ID[8];
char BALL_NAME[12];
char ACTION_TOPIC[30];
char STATE_TOPIC[30];

mqtt_client_data_t mqtt_data;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    mqtt_client_data_t* mqtt_data = (mqtt_client_data_t*)handler_args;

    esp_mqtt_client_handle_t client = *(mqtt_data->client);
    //EventGroupHandle_t connection_event_group = *(mqtt_data->connection_event_group);

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT connected");

            if (BALL_NAME[0] == 0) {
                srand(time(NULL));
                // Generate a random 6-digit ID
                int random_id = rand() % 900000 + 100000;

                snprintf(BALL_ID, sizeof(BALL_ID), "%d", random_id);
                snprintf(BALL_NAME, sizeof(BALL_NAME), "ball%d", random_id);
            }
            ESP_LOGI(MQTT_TAG, "MQTT connected s2");
            
            // Register ball to base station
            esp_mqtt_client_publish(client, "register", BALL_ID, 0, 0, 0);

            ESP_LOGI(MQTT_TAG, "MQTT connected 3");
            if (ACTION_TOPIC[0] == 0) {
                snprintf(ACTION_TOPIC, sizeof(ACTION_TOPIC), "%s/action", BALL_NAME); 
            }

            if (STATE_TOPIC[0] == 0) {
                snprintf(STATE_TOPIC, sizeof(STATE_TOPIC), "%s/state", BALL_NAME); 
            }
             ESP_LOGI(MQTT_TAG, "MQTT connected 4");
            
            // Subscribe to the "ball<ID>/action" topic
            esp_mqtt_client_subscribe(client, ACTION_TOPIC, 0);

            //xEventGroupSetBits(connection_event_group, MQTT_CONNECTED_BIT);
            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT disconnected");
            //xEventGroupClearBits(connection_event_group, MQTT_CONNECTED_BIT);
            break;
        
        case MQTT_EVENT_DATA:
            //if (xEventGroupGetBits(connection_event_group) & MQTT_CONNECTED_BIT) {
                ESP_LOGI(MQTT_TAG, "MQTT data received");
                
                printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
                printf("DATA=%.*s\r\n", event->data_len, event->data);

                if (strncmp(event->topic, ACTION_TOPIC, strlen(MQTT_TAG)) == 0) {
                    process_action(event->data);
                }
            //}
            break;
        
        default:
            break;
    }
}

void init_mqtt(EventGroupHandle_t *connection_event_group, esp_mqtt_client_handle_t *client, const char* broker_uri) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = broker_uri,
        },
    };

    // Initialze mqtt ball identifiers
    BALL_ID[0] = 0;
    BALL_NAME[0] = 0;
    ACTION_TOPIC[0] = 0;
    STATE_TOPIC[0] = 0;

    mqtt_data.connection_event_group = connection_event_group;
    mqtt_data.client = client;

    *client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(*client, ESP_EVENT_ANY_ID, mqtt_event_handler, &mqtt_data);
    esp_mqtt_client_start(*client);
}

void mqtt_publish_message(esp_mqtt_client_handle_t client, const char *message) {
    esp_mqtt_client_publish(client, STATE_TOPIC, message, 0, 0, 0);
}