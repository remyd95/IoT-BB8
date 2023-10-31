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

#define DEBUG 0

#define LED_PIN GPIO_NUM_21 // ONBOARD LED PIN FOR ESP32-S3
//#define LED_PIN GPIO_NUM_2 // ONBOARD LED PIN FOR ESP32

// Action constants
#define ACCEL_STEP_SIZE 1.0
#define TURN_STEP_SIZE 1.0
#define BRAKE_STEP_SIZE 2.0
#define DECISION_INTERVAL_TIME_MS 50
#define TARGET_OFFSET 10

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

    //xTaskCreate(imu_task, "imu_task", 4096, &imu_data, 10, NULL);
    xTaskCreate(report_state_task, "state_task", 4096, &mqtt_client, 10, NULL);
    xTaskCreate(test_connection_task, "test_connection_task", 4096, NULL, 10, NULL);

    TickType_t last_wakeup_time = xTaskGetTickCount(); 

    // Main action loop starts here
    while (1) {
        
        // Get current state and target
        State current_state = get_current_state();
        Target target = get_target();

        // When action = forward or backward, step-wise increase duty cycle to target duty cycle
        if (current_state.action == ACTION_FORWARD || current_state.action == ACTION_BACKWARD) {

            motor_action_data_t motor_action_data;
            motor_action_data.motor_id = MOTOR_ALL;

            float adjusted_duty_cycle = current_state.duty_cycle;

            if (current_state.duty_cycle < target.duty_cycle - ACCEL_STEP_SIZE) {
                adjusted_duty_cycle += ACCEL_STEP_SIZE;
            } else {
                adjusted_duty_cycle = target.duty_cycle;
            }

            motor_action_data.duty_cycle_left = adjusted_duty_cycle;
            motor_action_data.duty_cycle_right = adjusted_duty_cycle;

            if (current_state.action == ACTION_FORWARD) {
                pwm_forward_action(motor_action_data);
            } else if (current_state.action == ACTION_BACKWARD) {
                pwm_backward_action(motor_action_data);
            }
            
            set_current_duty_cycle(adjusted_duty_cycle);
        }

        if (current_state.action == ACTION_TURN_LEFT || current_state.action == ACTION_TURN_RIGHT) {

            motor_action_data_t motor_action_data_left;
            motor_action_data_t motor_action_data_right;

            float adjusted_duty_cycle = current_state.duty_cycle;

            if (current_state.duty_cycle < target.duty_cycle - TURN_STEP_SIZE) {
                adjusted_duty_cycle += TURN_STEP_SIZE;
            } else {
                adjusted_duty_cycle = target.duty_cycle;
            }

            motor_action_data_left.duty_cycle_left = adjusted_duty_cycle;
            motor_action_data_left.duty_cycle_right = adjusted_duty_cycle;
            motor_action_data_right.duty_cycle_left = adjusted_duty_cycle;
            motor_action_data_right.duty_cycle_right = adjusted_duty_cycle;

            if (current_state.action == ACTION_TURN_LEFT) {
                motor_action_data_left.motor_id = MOTOR_LEFT;
                pwm_backward_action(motor_action_data_left);
                motor_action_data_right.motor_id = MOTOR_RIGHT;
                pwm_forward_action(motor_action_data_right);
            } else if (current_state.action == ACTION_TURN_RIGHT) {
                motor_action_data_left.motor_id = MOTOR_LEFT;
                pwm_forward_action(motor_action_data_left);
                motor_action_data_right.motor_id = MOTOR_RIGHT;
                pwm_backward_action(motor_action_data_right);
            }
            
            set_current_duty_cycle(adjusted_duty_cycle);
        }

        if (current_state.action == ACTION_MOVETO) {
            float target_x = target.x;
            float target_y = target.y;

            float current_x = get_current_x_pos();
            float current_y = get_current_y_pos();

            float current_yaw = get_current_rotation();
            float current_duty_cycle = get_current_duty_cycle();

            float distance_to_target = sqrt(pow(target_x - current_x, 2) + pow(target_y - current_y, 2));
            float angle_to_target = atan2(target_y - current_y, target_x - current_x) * (180.0 / M_PI);
            float angle_difference = angle_to_target - current_yaw;

            if (angle_difference > 180.0) {
                angle_difference -= 360.0;
            } else if (angle_difference < -180.0) {
                angle_difference += 360.0;
            }

            float angle_difference_abs = fabs(angle_difference);

            // TODO: Rotate to target if angle difference is large enough

            // TODO: If angle difference is small enough, move forward to target

            // TODO: If distance to target is small enough, stop
        }

        // When action = stop, step-wise decrease duty cycle to 0
        if (current_state.action == ACTION_STOP) {

            if (current_state.duty_cycle == target.duty_cycle) {
                set_current_action(ACTION_IDLE);
                continue;
            }
                
            motor_action_data_t motor_action_data;
            motor_action_data.motor_id = MOTOR_ALL;

            float adjusted_duty_cycle = current_state.duty_cycle;

            if (current_state.duty_cycle > target.duty_cycle + BRAKE_STEP_SIZE) {
                adjusted_duty_cycle -= BRAKE_STEP_SIZE;
            } else {
                adjusted_duty_cycle = target.duty_cycle;
            }

            motor_action_data.duty_cycle_left = adjusted_duty_cycle;
            motor_action_data.duty_cycle_right = adjusted_duty_cycle;
            pwm_forward_action(motor_action_data);
            set_current_duty_cycle(adjusted_duty_cycle);
        }

        // When action != init, calculate displacement and update current coordinates
        if (current_state.action != ACTION_INIT) {
            float yaw = imu_data.heading;
            float pitch = imu_data.pitch;
            float roll = imu_data.roll;

            // TODO: Verify that the acceleration is correct
            float acceleration_y = imu_data.accely ;
            float acceleration_x = imu_data.accelx ;

            // TODO: Calculate displacement correctly
            displacement_x = 0.5 * acceleration_x * pow(timeStep, 2);
            displacement_y = 0.5 * acceleration_y * pow(timeStep, 2);
            float new_x = get_current_x_pos() + (displacement_x * 100 * cos(yaw * (M_PI / 180.0)));
            float new_y = get_current_y_pos() + (displacement_y * 100 * sin(yaw * (M_PI / 180.0)));

            set_current_coordinates(new_x, new_y);
            set_current_rotation(yaw);

            TickType_t current_time = xTaskGetTickCount();
            float elapsed_time = (current_time - last_wakeup_time) * portTICK_PERIOD_MS / 1000.0;

            // TODO: Improve debug output, 
            if (elapsed_time >= 1.0) {
                if (DEBUG) {
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
                }
                last_wakeup_time = current_time; 
            }
        }

        vTaskDelay(DECISION_INTERVAL_TIME_MS / portTICK_PERIOD_MS);
    }
}
