#pragma once

#include <driver/gpio.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "motor_action_data.h"

// Define motor pins
#define MOTOR1_PIN1 GPIO_NUM_1
#define MOTOR1_PIN2 GPIO_NUM_2
#define MOTOR2_PIN1 GPIO_NUM_3
#define MOTOR2_PIN2 GPIO_NUM_4

void pwm_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void pwm_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void pwm_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);

void pwm_forward_action(motor_action_data_t motor_action_data);
void pwm_backward_action(motor_action_data_t motor_action_data);
void pwm_stop_action(motor_action_data_t motor_action_data);

void pwm_configure_motors(void);