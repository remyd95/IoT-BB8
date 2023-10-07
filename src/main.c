// Local Imports
#include "wifi.h"
#include "mqtt.h"
#include "pwm_motor.h"
#include "action_handler.h"
#include "imu.h"
#include "state_machine.h"

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

// WiFi
const char* ssid = "Phone";
const char* password =  "wifi1234";
EventGroupHandle_t wifi_event_group;

// MQTT
const char* broker_uri = "mqtt://duijsens.dev";
esp_mqtt_client_handle_t mqtt_client;

// IMU
imu_data_t imu_data;
state_machine_data_t state_machine_data;
state_task_data_t state_task_data;

static void configure_led(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
}

void test_wifi_connection() {
    // WiFi connected, turn on the LED
    if (xEventGroupGetBits(wifi_event_group) & CONNECTED_BIT) {
        if (!gpio_get_level(LED_PIN)) {
            gpio_set_level(LED_PIN, 1);
        }
    // WiFi not connected, turn off the LED
    } else {
        gpio_set_level(LED_PIN, 0);
    }
}

void init_state_structures() {
    state_machine_data.current_x = 0.0;
    state_machine_data.current_y = 0;
    state_machine_data.rotation = 0.0;
    state_machine_data.current_action = 1;

    state_task_data.mqtt_client = mqtt_client;
    state_task_data.state_machine_data = state_machine_data;
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

  init_state_structures();

  xTaskCreate(imu_task, "imu_task", 4096, &imu_data, 10, NULL);
  xTaskCreate(report_state_task, "state_task", 4096, &state_task_data, 10, NULL);

  while (1) {
    test_wifi_connection();

    printf("IMU DATA: H %f P %f R %f T %f\n", imu_data.heading, imu_data.pitch, imu_data.roll, imu_data.temp);

    // TODO: Implement decision loop 
    // TODO: Implement priority queue for specific high priority items 
    //       Idea: Use preemption from RTOS?
    
    // Delay before processing the next message
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}