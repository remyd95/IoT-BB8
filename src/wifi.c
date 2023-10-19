#include "wifi.h"

const int CONNECTED_BIT = BIT0;
const int NUM_RETRIES = 0;
int retry_num = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    EventGroupHandle_t wifi_event_group = (EventGroupHandle_t)event_handler_arg;

    if (event_id == WIFI_EVENT_STA_START) {
        printf("Connecting...\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("Connection over WIFI established.\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        printf("Connection has been lost.\n");

        //TODO: Stop ANY action when we lose connection 

        wifi_connect(); // Attempt to reconnect
    }
    else if (event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        printf("Got IP from router.\n\n");
    }
}

void wifi_connect() {
    while (retry_num < 10) { // Stop after some retries because this drains the battery!
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        printf("Trying to establish outgoing connection...\n");

        if (esp_wifi_connect() == ESP_OK) {
            break;
        }
        retry_num++;
    }
}

void init_wifi(EventGroupHandle_t *wifi_event_group, const char* ssid , const char* password) {
    *wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&wifi_initiation);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, *wifi_event_group);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, *wifi_event_group);

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