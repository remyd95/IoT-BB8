#include "wifi.h"
#include "mqtt.h"
#include "pwm_motor.h"
#include "action_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <mqtt_client.h>
#include <driver/gpio.h>
#include "nvs_flash.h"
#include "lwip/sys.h"
#include "lwip/err.h"

// Define pins
#define LED_PIN GPIO_NUM_2

// WiFi credentials
const char* ssid = "Phone";
const char* password =  "wifi1234";
 
// WiFi Event Handler
EventGroupHandle_t wifi_event_group;

// MQTT configurations
const char* broker_uri = "mqtt://duijsens.dev";

// MQTT Client initialization
esp_mqtt_client_handle_t mqtt_client;

static void configure_led(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
}

void app_main() {
    nvs_flash_init();

    configure_led();
    pwm_configure_motors();

    init_wifi(&wifi_event_group, ssid, password);

    set_forward_action_callback(pwm_forward_action);
    set_backward_action_callback(pwm_backward_action);
    set_stop_action_callback(pwm_stop_action);

    init_mqtt(&mqtt_client, broker_uri);

   bool connected = false;

    while (1) {
        // do something
        if (xEventGroupGetBits(wifi_event_group) & CONNECTED_BIT) {
            // WiFi connected, turn on the LED
            if (!connected) {
                 connected = true;
                gpio_set_level(LED_PIN, 1);
            }
        } else {
            // WiFi not connected, turn off the LED
            connected = false;
            gpio_set_level(LED_PIN, 0);
        }

        // Delay before processing the next message
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}