#pragma once
#include "pwm_motor.h"
#include "state_machine.h"
#include <math.h>
#include "esp_system.h"

// Action constants
#define ACCEL_STEP_SIZE 5.0
#define TURN_STEP_SIZE 5.0
#define BRAKE_STEP_SIZE 2.0

#define DECISION_INTERVAL_TIME_MS 50
#define TURN_PULSE_MS 100
#define TURN_INTERVAL_MS 1000

#define TARGET_OFFSET 20
#define ANGLE_OFFSET 10

#define TURN_DUTY_CYCLE 35

// Define really small number
#define EPSILON 0.001

// General functions
int process_objective_switch(int previous_objective, int current_objective);
void process_objective(State state, Target target, int previous_objective);
void process_action(State state, Target target);

// Helper functions
float calculate_angle_difference(float angle_to_target, float yaw);

// Action functions
void stop_action(State state);
void forward_action(State state, Target target);
void backward_action(State state, Target target);
void turn_left_action(State state);
void turn_right_action(State state);