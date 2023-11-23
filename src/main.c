// Local Imports
#include "wifi.h"
#include "mqtt.h"
#include "pwm_motor.h"
#include "objective_handler.h"
#include "action_handler.h"
#include "imu.h"
#include "integral.h"
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

#define DEBUG 0

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


static void configure_led(void) {
    /**
     * Configures the LED pin
     * 
     * @return void
    */
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
}

void test_connection_task(void *args) {
    /**
     * Task to test the WiFi connection. 
     * Sets the LED to on when connected, off when not connected.
     * 
     * @param args Unused
     * 
     * @return void
    */
    while (1) {
        // WiFi connected, turn on the LED
        if (xEventGroupGetBits(connection_event_group) & WIFI_CONNECTED_BIT) {
            if (!gpio_get_level(LED_PIN)) {
                gpio_set_level(LED_PIN, 1);
            }
        // WiFi not connected, turn off the LED
        } else {
            gpio_set_level(LED_PIN, 0);

            // Stop the motors when WiFi is disconnected
            //set_current_objective(OBJECTIVE_STOP);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    /**
     * Main function
     * 
     * @return void
    */

    nvs_flash_init();

    configure_led();
    pwm_configure_motors();

    connection_event_group = xEventGroupCreate();

    init_wifi(&connection_event_group, ssid, password);
    init_mqtt(&connection_event_group, &mqtt_client, broker_uri);

    xTaskCreate(imu_task, "imu_task", 4096, &imu_data, 10, NULL);
    xTaskCreate(report_state_task, "state_task", 4096, &mqtt_client, 10, NULL);
    xTaskCreate(test_connection_task, "test_connection_task", 4096, NULL, 10, NULL);

    TickType_t last_wakeup_time = xTaskGetTickCount(); 
    TickType_t last_turn_pulse = xTaskGetTickCount(); 

    vector_t current_compensated_va = imu_data.compensated_va;
    vector_t last_compensated_va = imu_data.compensated_va;
    vector_t last_velocity = {0.0f, 0.0f, 0.0f};

    // Main action loop starts here
    while (1) {
        
        // Get current state and target
        // State current_state = get_current_state();
        // Target target = get_target();

        // Process the objective to determine the next action
        process_objective(get_current_state(), get_target());

        // Process the action to determine the next state
        process_action(get_current_state(), get_target(), &last_turn_pulse);

        // Update the current state after processing the objective and action
        if (current_state.objective != OBJECTIVE_INIT) {
            float new_x = get_current_x_pos();
            float new_y = get_current_y_pos();

            set_current_coordinates(new_x, new_y);
            set_current_rotation(imu_data.heading);
            set_current_pitch(imu_data.pitch);
            set_current_roll(imu_data.roll);

            // Get the compensated acceleration from the IMU
            last_compensated_va = current_compensated_va;
            vector_t current_compensated_va = imu_data.compensated_va;
            
            // Calculate the velocity integrals
            float velocity_x = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_compensated_va.x, current_compensated_va.x);
            float velocity_y = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_compensated_va.y, current_compensated_va.y);
            float velocity_z = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_compensated_va.z, current_compensated_va.z);

            // Calculate the displacement integrals
            float displacement_x = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_velocity.x, velocity_x);
            float displacement_y = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_velocity.y, velocity_y);
            float displacement_z = integrate((float)xTaskGetTickCount(), (float)xTaskGetTickCount()+DECISION_INTERVAL_TIME_MS, last_velocity.z, velocity_z);

            // Update last velocity
            last_velocity.x = velocity_x;
            last_velocity.y = velocity_y;
            last_velocity.z = velocity_z;

            // Calculate new coordinates based on current yaw and displacement
            float total_displacement = sqrt(pow(displacement_x, 2) + pow(displacement_y, 2) + pow(displacement_z, 2));
            float new_x_pos = get_current_x_pos() + total_displacement * cos(DEG2RAD(get_current_rotation()));
            float new_y_pos = get_current_y_pos() + total_displacement * sin(DEG2RAD(get_current_rotation()));

            // Update the current coordinates
            set_current_coordinates(new_x_pos, new_y_pos);

            // Debug output
            printf("Current coordinates: X: %f, Y: %f\n", get_current_x_pos(), get_current_y_pos());
            printf("Total displacement: %f\n", total_displacement);

            TickType_t current_time = xTaskGetTickCount();
            float elapsed_time = (current_time - last_wakeup_time) * portTICK_PERIOD_MS / 1000.0;

            // TODO: Improve debug output, 
            if (elapsed_time >= 1.0) {
                if (DEBUG) {
                    printf("Yaw: %f, X: %f, Y: %f\n", get_current_rotation(), get_current_x_pos(), get_current_x_pos());
                }
                last_wakeup_time = current_time; 
            }
        }

        // Wait for the next decision interval
        vTaskDelay(DECISION_INTERVAL_TIME_MS / portTICK_PERIOD_MS);
    }
}
