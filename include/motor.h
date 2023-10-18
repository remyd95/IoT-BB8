#pragma once

#include <driver/gpio.h>
#include "motor_action_data.h"

// Define motor pins
// #define MOTORA_PINA GPIO_NUM_1
// #define MOTORA_PINB GPIO_NUM_4
// #define MOTORB_PINA GPIO_NUM_3
// #define MOTORB_PINB GPIO_NUM_2

#define MOTORA_PINA GPIO_NUM_32
#define MOTORA_PINB GPIO_NUM_33
#define MOTORB_PINA GPIO_NUM_25
#define MOTORB_PINB GPIO_NUM_26


void forward_action(motor_action_data_t motor_action_data);
void backward_action(motor_action_data_t motor_action_data);
void stop_action(motor_action_data_t motor_action_data);
void configure_motors(void);