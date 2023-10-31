#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mqtt.h"
#include <mqtt_client.h>

#define ACTION_IDLE 0
#define ACTION_MOVETO 1
#define ACTION_FORWARD 2
#define ACTION_BACKWARD 3
#define ACTION_STOP 4
#define ACTION_INIT 5
#define ACTION_DISCONNECTED 6
#define ACTION_TURN_LEFT 7
#define ACTION_TURN_RIGHT 8
#define ACTION_UNDEFINED -1

#define STATE_INTERVAL_TIME_MS 1000


typedef struct {
    float x;
    float y;
    float rotation;
    float pitch;
    float roll;
    float duty_cycle;
    float speed;
    float acceleration;
    int action;
} State;

typedef struct {
    float x;
    float y;
    float duty_cycle;
} Target;


extern volatile State current_state;
extern volatile Target target;

void set_current_coordinates(float x, float y);
void set_current_action(int action);
void set_current_rotation(float rotation);
void set_current_speed(float speed);
void set_current_duty_cycle(float duty_cycle);
void set_current_acceleration(float acceleration);
void set_current_pitch(float pitch);
void set_current_roll(float roll);
void set_target_coordinates(float x, float y);
void set_target_duty_cycle(float duty_cycle);
float get_current_x_pos();
float get_current_y_pos();
float get_current_rotation();
float get_current_duty_cycle();
float get_current_speed();
float get_current_acceleration();
float get_current_pitch();
float get_current_roll();
float get_target_x_pos();
float get_target_y_pos();
float get_target_duty_cycle();
int get_current_action();
State get_current_state();
Target get_target();
void report_state_task(void *args);