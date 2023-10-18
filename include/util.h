#pragma once

#include <stdbool.h>
#include <ctype.h>

static float bound_max_speed(float max_speed) {
    float bounded_max_speed = 0;

    if (max_speed < 0) {
        bounded_max_speed = 0;
    } else if (max_speed > 100) {
        bounded_max_speed = 100;
    } else {
        bounded_max_speed = max_speed;
    }
    return bounded_max_speed;
}