#include "location.h"

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