#pragma once
#include "pwm_motor.h"
#include "state_machine.h"
#include <math.h>

// Action constants
#define ACCEL_STEP_SIZE 1.0
#define TURN_STEP_SIZE 1.0
#define BRAKE_STEP_SIZE 2.0

#define DECISION_INTERVAL_TIME_MS 50
#define TURN_PULSE_MS 20

#define TARGET_OFFSET 10

// Define really small number
#define EPSILON 0.001

// General functions
int process_objective_switch(int previous_objective, int current_objective);
void process_objective(State state, Target target, int previous_objective);
void process_action(State state, Target target);

// Action functions
void stop_action(State state);
void forward_action(State state, Target target);
void backward_action(State state, Target target);
void turn_left_action(State state, Target target);
void turn_right_action(State state, Target target);