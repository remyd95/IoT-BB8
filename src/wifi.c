#include "wifi.h"

const int WIFI_CONNECTED_BIT = BIT0;
const int MAX_RETRIES = 10;
const int MAX_RETRY_INTERVAL = 3600000;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    EventGroupHandle_t connection_event_group = (EventGroupHandle_t)event_handler_arg;

    if (event_id == WIFI_EVENT_STA_START) {
        printf("Connecting...\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("Connection over WIFI established.\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(connection_event_group, WIFI_CONNECTED_BIT);
        printf("Connection has been lost.\n");

        wifi_connect(); // Attempt to reconnect
    }
    else if (event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(connection_event_group, WIFI_CONNECTED_BIT);
        printf("Got IP from router.\n\n");
    }
}

void wifi_connect() {
    int retry_interval = 2000;

    int retry_num = 1;
    int retry_exp_step = 1;

    while (1) {
        vTaskDelay(retry_interval / portTICK_PERIOD_MS);
        printf("Trying to establish outgoing connection... (Attempt #%d)\n", retry_num);

        if (esp_wifi_connect() == ESP_OK) {
            break;
        }

        if (retry_num <= MAX_RETRIES) { // Initial phase
            retry_num++;
        } else if (retry_num > MAX_RETRIES && retry_interval <= MAX_RETRY_INTERVAL) { // Exponential phase
            retry_interval = pow(2, retry_exp_step) * 1000;
        }
    }
}

void init_wifi(EventGroupHandle_t *connection_event_group, const char* ssid , const char* password) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, *connection_event_group);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, *connection_event_group);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
           }
        };

    strcpy((char*)wifi_configuration.sta.ssid, ssid);
    strcpy((char*)wifi_configuration.sta.password, password);    

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_connect();
}