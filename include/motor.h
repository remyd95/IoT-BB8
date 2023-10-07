#pragma once

#include <driver/gpio.h>
#include "motor_action_data.h"

// Define motor pins
#define MOTOR1_PIN1 GPIO_NUM_1
#define MOTOR1_PIN2 GPIO_NUM_2
#define MOTOR2_PIN1 GPIO_NUM_3
#define MOTOR2_PIN2 GPIO_NUM_4

void forward_action(motor_action_data_t motor_action_data);
void backward_action(motor_action_data_t motor_action_data);
void stop_action(motor_action_data_t motor_action_data);
void configure_motors(void);