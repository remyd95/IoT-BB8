#pragma once
#include "pwm_motor.h"
#include "state_machine.h"
#include <math.h>
#include "esp_system.h"

// Action constants
#define ACCEL_STEP_SIZE 25.0
#define TURN_STEP_SIZE 25.0
#define BRAKE_STEP_SIZE 2.0

// Currently broken: stop only implemented for one direction
#define TURN_BRAKE_STEP_SIZE 100.0

#define DECISION_INTERVAL_TIME_MS 50
#define TURN_PULSE_MS 200
#define TURN_INTERVAL_MS 1000

#define TARGET_OFFSET 20
#define ANGLE_OFFSET 10

#define TURN_DUTY_CYCLE 50

// Define really small number
#define EPSILON 0.001

// General functions
int process_objective_switch(int previous_objective, int current_objective);
void process_objective(State state, Target target, int previous_objective);
void process_action(State state, Target target, TickType_t* last_turn_pulse);

// Helper functions
float calculate_angle_difference(float angle_to_target, float yaw);

// Action functions
void stop_action(State state);
void forward_action(State state, Target target);
void backward_action(State state, Target target);
void do_turn_pulse(State state, TickType_t* last_turn_pulse);
void stop_turn_action(State state, int action, bool set_none);
void turn_action(State state, int action);