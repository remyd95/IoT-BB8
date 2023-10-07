// Local Imports
#include "wifi.h"
#include "mqtt.h"
#include "pwm_motor.h"
#include "action_handler.h"
#include "imu.h"

// Std
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ESP
#include <esp_log.h>

// Drivers
#include <driver/gpio.h>
#include "driver/i2c.h"

// Other
#include <mqtt_client.h>
#include "nvs_flash.h"
#include "lwip/sys.h"
#include "lwip/err.h"

//#define LED_PIN GPIO_NUM_21 // ONBOARD LED PIN FOR ESP32-S3
#define LED_PIN GPIO_NUM_2 // ONBOARD LED PIN FOR ESP32-S3


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
  //pwm_configure_motors();

  init_wifi(&wifi_event_group, ssid, password);

  set_forward_action_callback(pwm_forward_action);
  set_backward_action_callback(pwm_backward_action);
  set_stop_action_callback(pwm_stop_action);

  init_mqtt(&mqtt_client, broker_uri);

  imu_data_t imu_data;
  xTaskCreate(imu_task, "imu_task", 4096, &imu_data, 10, NULL);

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
      printf("IMU DATA: H %f P %f R %f T %f\n", imu_data.heading, imu_data.pitch, imu_data.roll, imu_data.temp);

      // Delay before processing the next message
      vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}