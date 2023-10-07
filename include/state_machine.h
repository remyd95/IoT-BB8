#pragma once

#define ACTION_IDLE 0
#define ACTION_MOVETO 1
#define ACTION_FORWARD 2
#define ACTION_BACKWARD 3
#define ACTION_STOP 4
#define ACTION_INIT 5
#define ACTION_DISCONNECT 6
#define ACTION_UNDEFINED -1

extern volatile float current_x;
extern volatile float current_y;
extern volatile int current_action;

void set_current_coordinates(float x, float y);
void set_current_action(int action);
float get_current_x_pos();
float get_current_y_pos();
int get_current_action();