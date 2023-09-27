#include "pwm_motor.h"


static void pwm_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

static void pwm_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

static void pwm_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

void pwm_forward_action(motor_action_data_t motor_action_data) {
    pwm_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle);
}

void pwm_backward_action(motor_action_data_t motor_action_data) {
    pwm_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle);
}

void pwm_stop_action(motor_action_data_t motor_action_data) {
    pwm_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
}


void pwm_configure_motors(void) {

    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR1_PIN1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR1_PIN2);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR2_PIN1);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, MOTOR2_PIN2);
    
    mcpwm_config_t pwm_config;

    pwm_config.frequency = 1000;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);  

}