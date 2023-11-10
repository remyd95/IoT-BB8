#include "mqtt.h"
#include "objective_handler.h"

#include <stdio.h>
#include <esp_log.h>
#include <time.h>

const char *MQTT_TAG = "MQTT";
const int MQTT_CONNECTED_BIT = BIT1;

char BALL_ID[8];
char BALL_NAME[12];
char OBJECTIVE_TOPIC[30];
char STATE_TOPIC[30];
char ID_TOPIC[2] = "id";

mqtt_client_data_t mqtt_data;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    /**
     * Handles MQTT events. The following events are handled:
     * 1. MQTT_EVENT_CONNECTED: The ball is registered to the base station
     * 2. MQTT_EVENT_DISCONNECTED: The ball is unregistered from the base station
     * 3. MQTT_EVENT_DATA: The ball receives an action from the base station
     * 
     * @param handler_args: pointer to the event group
     * @param base: event base
     * @param event_id: event ID
     * @param event_data: event data
     * 
     * @return void
    */
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    mqtt_client_data_t* mqtt_data = (mqtt_client_data_t*)handler_args;
    esp_mqtt_client_handle_t client = *(mqtt_data->client);
    EventGroupHandle_t connection_event_group = *(mqtt_data->connection_event_group);

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            if (BALL_NAME[0] == 0) {
                srand(time(NULL));
                // Generate a random 6-digit ID
                int random_id = rand() % 900000 + 100000;

                snprintf(BALL_ID, sizeof(BALL_ID), "%d", random_id);
                snprintf(BALL_NAME, sizeof(BALL_NAME), "ball%d", random_id);
            }
  
            // Register ball to base station
            esp_mqtt_client_publish(client, "register", BALL_ID, 0, 0, 0);

            if (OBJECTIVE_TOPIC[0] == 0) {
                snprintf(OBJECTIVE_TOPIC, sizeof(OBJECTIVE_TOPIC), "%s/objective", BALL_NAME); 
            }

            if (STATE_TOPIC[0] == 0) {
                snprintf(STATE_TOPIC, sizeof(STATE_TOPIC), "%s/state", BALL_NAME); 
            }

           // Subscribe to the "ball<ID>/objective" topic
            esp_mqtt_client_subscribe(client, OBJECTIVE_TOPIC, 0);
            esp_mqtt_client_subscribe(client, ID_TOPIC, 0);

            xEventGroupSetBits(connection_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGI(MQTT_TAG, "MQTT connected");
            break;
        
        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(connection_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGI(MQTT_TAG, "MQTT disconnected");
            break;
        
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT data received");

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            if (strncmp(event->topic, OBJECTIVE_TOPIC, strlen(MQTT_TAG)) == 0) {
                process_objective_message(event->data);
            } else if (strncmp(event->topic, ID_TOPIC, 2) == 0) {
                identify_ball(client, event->data);
            }
            break;
        
        default:
            break;
    }
}

void identify_ball(esp_mqtt_client_handle_t client, char* event_data) {
    /**
     * Identifies the ball to the base station. This is in response to
     * a find available ball request from client GUI
     * 
     * @param client: MQTT client handle
     * @param event_data: event data
     * 
     * @return void
    */
    if (strncmp(event_data, "FA", 2) == 0) {
        int client_id;

        if (sscanf(event_data, "FA %d", &client_id) == 1) {
            int max_length = snprintf(NULL, 0, "%s %f %f %f %d", BALL_ID, get_current_x_pos(), get_current_y_pos(), get_current_rotation(), get_current_objective());
            char message[max_length + 1]; 
            snprintf(message, max_length + 1, "%s %f %f %f %d", BALL_ID, get_current_x_pos(), get_current_y_pos(), get_current_rotation(), get_current_objective());

            esp_mqtt_client_publish(client, "register", message, 0, 0, 0);
        }
    }
}

void init_mqtt(EventGroupHandle_t *connection_event_group, esp_mqtt_client_handle_t *client, const char* broker_uri) {
    /**
     * Initializes the MQTT connection
     * 
     * @param connection_event_group: pointer to the event group
     * @param client: MQTT client handle
     * @param broker_uri: URI of the MQTT broker
     * 
     * @return void
    */
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = broker_uri,
        },
    };

    // Initialze mqtt ball identifiers
    BALL_ID[0] = 0;
    BALL_NAME[0] = 0;
    OBJECTIVE_TOPIC[0] = 0;
    STATE_TOPIC[0] = 0;

    mqtt_data.connection_event_group = connection_event_group;
    mqtt_data.client = client;

    *client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(*client, ESP_EVENT_ANY_ID, mqtt_event_handler, &mqtt_data);
    esp_mqtt_client_start(*client);
}

void mqtt_publish_message(esp_mqtt_client_handle_t client, const char *message) {
    /**
     * Publishes a message to the MQTT broker
     * 
     * @param client: MQTT client handle
     * @param message: message to publish
     * 
     * @return void
    */
    esp_mqtt_client_publish(client, STATE_TOPIC, message, 0, 0, 0);
}