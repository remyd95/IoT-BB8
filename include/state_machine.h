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
    float speed;
    float acceleration;
    int action;
} State;

extern volatile State current_state;

void set_current_coordinates(float x, float y);
void set_current_action(int action);
void set_current_rotation(float rotation);
void set_current_speed(float speed);
float get_current_x_pos();
float get_current_y_pos();
float get_current_rotation();
float get_current_speed();
int get_current_action();
void report_state_task(void *args);