#include "state_machine.h"

volatile float current_x = 0.0;
volatile float current_y = 0.0;
volatile int current_action = ACTION_IDLE;

void set_current_action(int action) {
    current_action = action;
}

void set_current_coordinates(float x, float y) {
    current_x = x;
    current_y = y;
}

float get_current_x_pos() {
    return current_x;
}

float get_current_y_pos() {
    return current_y;
}

int get_current_action() {
    return current_action;
}