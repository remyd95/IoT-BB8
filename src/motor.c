#include "motor.h"


void forward_motion_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTOR1_PIN1, 1);
        gpio_set_level(MOTOR1_PIN2, 0);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTOR2_PIN1, 1);
        gpio_set_level(MOTOR2_PIN2, 0);
    }
}

void backward_motion_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTOR1_PIN1, 0);
        gpio_set_level(MOTOR1_PIN2, 1);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTOR2_PIN1, 0);
        gpio_set_level(MOTOR2_PIN2, 1);
    }
}

void stop_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTOR1_PIN1, 0);
        gpio_set_level(MOTOR1_PIN2, 0);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTOR2_PIN1, 0);
        gpio_set_level(MOTOR2_PIN2, 0);
    }
}

void configure_motors(void) {
    gpio_reset_pin(MOTOR1_PIN1);
    gpio_set_direction(MOTOR1_PIN1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(MOTOR1_PIN2);
    gpio_set_direction(MOTOR1_PIN2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR2_PIN1);
    gpio_set_direction(MOTOR2_PIN1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(MOTOR2_PIN2);
    gpio_set_direction(MOTOR2_PIN2, GPIO_MODE_OUTPUT);
}