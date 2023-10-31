#include "pwm_motor.h"

void pwm_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle) {
    /**
     * Set the motor to go forward
     * 
     * @param mcpwm_num The MCPWM unit number
     * @param timer_num The MCPWM timer number
     * @param duty_cycle The duty cycle
     * 
     * @return void
    */
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

void pwm_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle) {
    /**
     * Set the motor to go backward
     * 
     * @param mcpwm_num The MCPWM unit number
     * @param timer_num The MCPWM timer number
     * @param duty_cycle The duty cycle
     * 
     * @return void
    */
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

void pwm_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num) {
    /**
     * Stop the motor
     * 
     * @param mcpwm_num The MCPWM unit number
     * @param timer_num The MCPWM timer number
     * 
     * @return void
    */
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

void pwm_forward_action(motor_action_data_t motor_action_data) {
    /**
     * Set the motor to go forward. This function is called from the state machine.
     * 
     * @param motor_action_data The motor action data
     * 
     * @return void
    */
    if (motor_action_data.motor_id == MOTOR_LEFT) {
        pwm_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle_left);
    } else if (motor_action_data.motor_id == MOTOR_RIGHT) {
        pwm_motor_forward(MCPWM_UNIT_1, MCPWM_TIMER_0, motor_action_data.duty_cycle_right);
    } else if (motor_action_data.motor_id == MOTOR_ALL) {
        pwm_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle_left);
        pwm_motor_forward(MCPWM_UNIT_1, MCPWM_TIMER_0, motor_action_data.duty_cycle_right);
    }
}

void pwm_backward_action(motor_action_data_t motor_action_data) {
    /**
     * Set the motor to go backward. This function is called from the state machine.
     * 
     * @param motor_action_data The motor action data
     * 
     * @return void
    */
    if (motor_action_data.motor_id == MOTOR_LEFT) {
        pwm_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle_right);
    } else if (motor_action_data.motor_id == MOTOR_RIGHT) {
        pwm_motor_backward(MCPWM_UNIT_1, MCPWM_TIMER_0, motor_action_data.duty_cycle_left);
    } else if (motor_action_data.motor_id == MOTOR_ALL) {
        pwm_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, motor_action_data.duty_cycle_left);
        pwm_motor_backward(MCPWM_UNIT_1, MCPWM_TIMER_0, motor_action_data.duty_cycle_right);
    }
}

void pwm_stop_action(motor_action_data_t motor_action_data) {
    /**
     * Stop the motor. This function is called from the state machine.
     * 
     * @param motor_action_data The motor action data
     * 
     * @return void
    */
    if (motor_action_data.motor_id == MOTOR_LEFT) {
        pwm_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    } else if (motor_action_data.motor_id == MOTOR_RIGHT) {
        pwm_motor_stop(MCPWM_UNIT_1, MCPWM_TIMER_0);
    } else if (motor_action_data.motor_id == MOTOR_ALL) {
        pwm_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
        pwm_motor_stop(MCPWM_UNIT_1, MCPWM_TIMER_0);
    }
}

void pwm_configure_motors(void) {
    /**
     * Configure the motorrs. ESP32 has 2 MCPWM units, each with 3 timers.
     * The timers are used to control the motors. Each motor has 2 pins.
     * The pins are used to control the direction of the motor. The frequency
     * is set to 1000 Hz. The default duty cycle is set to 0.
     * 
     * @return void
    */
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTORA_PINA);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTORA_PINB);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, MOTORB_PINA);
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, MOTORB_PINB);
    
    mcpwm_config_t pwm_config;

    pwm_config.frequency = 1000;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); 
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config); 
}