#pragma once

#include "motor_action_data.h"

typedef void (*MotorActionCallback)(motor_action_data_t); 

void set_forward_action_callback(MotorActionCallback callback);
void set_backward_action_callback(MotorActionCallback callback);
void set_stop_action_callback(MotorActionCallback callback);
void move_to(float x, float y, float max_speed);
void process_action(char* event_data);