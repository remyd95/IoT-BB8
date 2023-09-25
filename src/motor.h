#pragma once

#include <driver/gpio.h>

// Define motor pins
#define MOTOR1_PIN1 GPIO_NUM_25
#define MOTOR1_PIN2 GPIO_NUM_26
#define MOTOR2_PIN1 GPIO_NUM_32
#define MOTOR2_PIN2 GPIO_NUM_33

void forward_motion(int motor_id);
void backward_motion(int motor_id);
void stop(int motor_id);
void configure_motors(void);