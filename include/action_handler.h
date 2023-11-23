#pragma once
#include "pwm_motor.h"
#include "state_machine.h"
#include <math.h>
#include "esp_system.h"

// Timing constants
#define DECISION_INTERVAL_TIME_MS 50

// Turn behavior constants
#define TURN_PULSE_MS 75
#define TURN_INTERVAL_MS 50
#define TURN_STEP_SIZE 100.0
#define TURN_DUTY_CYCLE 20

// Move constants
#define ACCEL_STEP_SIZE 100.0

// Break behavior constants
#define BRAKE_STEP_SIZE 3.0
#define BRAKE_PULSE_DUTY_CYCLE_MULTIPLIER 1
#define BRAKE_STEADY_PERIOD_MS 1000
#define BRAKE_PULSE_PERIOD_MS 200

// Target constants
#define TARGET_OFFSET 20
#define ANGLE_OFFSET 10

// Define positional constants
#define PITCH_STEADY_STATE 0
#define ROLL_STEADY_STATE 90
#define PITCH_OFFSET 0
#define ROLL_OFFSET 50

// Define really small number
#define EPSILON 0.01


extern volatile int stop_counter;
extern volatile int brake_pulse_counter;

// General functions
int process_objective_switch(int previous_objective, int current_objective);
void process_objective(State state, Target target);
void process_action(State state, Target target, TickType_t* last_turn_pulse);

// Helper functions
float calculate_angle_difference(float angle_to_target, float yaw);

// Action functions
void stop_action(State state);
void forward_action(State state, Target target);
void backward_action(State state, Target target);
void do_turn_pulse(State state, TickType_t* last_turn_pulse);
void stop_turn_action(bool final_turn);
void turn_action(State state, int action);
int get_stop_counter();
void increment_stop_counter();
void reset_stop_counter();
int get_brake_pulse_counter();
void increment_brake_pulse_counter();
void reset_brake_pulse_counter();