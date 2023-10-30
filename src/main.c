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
#include <math.h>

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

#define LED_PIN GPIO_NUM_21 // ONBOARD LED PIN FOR ESP32-S3
//#define LED_PIN GPIO_NUM_2 // ONBOARD LED PIN FOR ESP32

// WiFi
const char* ssid = "Phone";
const char* password =  "wifi1234";

// MQTT
const char* broker_uri = "mqtt://duijsens.dev";
esp_mqtt_client_handle_t mqtt_client;

// Flags
EventGroupHandle_t connection_event_group;

// IMU
imu_data_t imu_data;

// State TEMP
const float timeStep = 0.05;

float displacement_x = 0.0;
float displacement_y = 0.0;

const float ball_radius = 0.06; 


static void configure_led(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
}

void test_connection_task(void *args) {

    while (1) {
        // WiFi connected, turn on the LED
        if (xEventGroupGetBits(connection_event_group) & WIFI_CONNECTED_BIT) {
            if (!gpio_get_level(LED_PIN)) {
                gpio_set_level(LED_PIN, 1);
            }
        // WiFi not connected, turn off the LED
        } else {
            gpio_set_level(LED_PIN, 0);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    nvs_flash_init();

    configure_led();
    pwm_configure_motors();

    connection_event_group = xEventGroupCreate();

    init_wifi(&connection_event_group, ssid, password);
    init_mqtt(&connection_event_group, &mqtt_client, broker_uri);

    //xTaskCreate(imu_task, "imu_task", 4096, &imu_data, 10, NULL);
    xTaskCreate(report_state_task, "state_task", 4096, &mqtt_client, 10, NULL);
    xTaskCreate(test_connection_task, "test_connection_task", 4096, NULL, 10, NULL);

    TickType_t last_wakeup_time = xTaskGetTickCount(); 


  while (1) {


    if (get_current_action() != ACTION_INIT) {
        float yaw = imu_data.heading;
        float pitch = imu_data.pitch;
        float roll = imu_data.roll;

        float acceleration_y = imu_data.accely ;
        float acceleration_x = imu_data.accelx ;

        displacement_x = 0.5 * acceleration_x * pow(timeStep, 2);
        displacement_y = 0.5 * acceleration_y * pow(timeStep, 2);

        float new_x = get_current_x_pos() + (displacement_x * 100 * cos(yaw * (M_PI / 180.0)));
        float new_y = get_current_y_pos() + (displacement_y * 100 * sin(yaw * (M_PI / 180.0)));

        set_current_coordinates(new_x, new_y);
        set_current_rotation(yaw);

        TickType_t current_time = xTaskGetTickCount();
        float elapsed_time = (current_time - last_wakeup_time) * portTICK_PERIOD_MS / 1000.0;

        if (elapsed_time >= 1.0) {
            printf("Yaw: ");
            printf("%f\n", imu_data.heading);
            printf("Displacement X: ");
            printf("%f\n", displacement_x);
            printf("   Displacement Y: ");
            printf("%f\n", displacement_y);
            printf("   Pos X: ");
            printf("%f\n", get_current_x_pos());
            printf("   Pos Y: ");
            printf("%f\n", get_current_y_pos());
            last_wakeup_time = current_time; 
        }
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}