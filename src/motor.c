#include "motor.h"

void forward_motion_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTORA_PINA, 1);
        gpio_set_level(MOTORA_PINB, 0);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTORB_PINA, 1);
        gpio_set_level(MOTORB_PINB, 0);
    }
}

void backward_motion_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTORA_PINA, 0);
        gpio_set_level(MOTORA_PINB, 1);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTORB_PINA, 0);
        gpio_set_level(MOTORB_PINB, 1);
    }
}

void stop_action(motor_action_data_t motor_action_data) {
    if (motor_action_data.motor_id == 1) {
        gpio_set_level(MOTORA_PINA, 0);
        gpio_set_level(MOTORA_PINB, 0);
    } 
    else if (motor_action_data.motor_id == 2) {
        gpio_set_level(MOTORB_PINA, 0);
        gpio_set_level(MOTORB_PINB, 0);
    }
}

void configure_motors(void) {
    gpio_reset_pin(MOTORA_PINA);
    gpio_set_direction(MOTORA_PINA, GPIO_MODE_OUTPUT);
    gpio_reset_pin(MOTORA_PINB);
    gpio_set_direction(MOTORA_PINB, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTORB_PINA);
    gpio_set_direction(MOTORB_PINA, GPIO_MODE_OUTPUT);
    gpio_reset_pin(MOTORB_PINB);
    gpio_set_direction(MOTORB_PINB, GPIO_MODE_OUTPUT);
}