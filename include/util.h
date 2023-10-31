#pragma once

#include <stdbool.h>
#include <ctype.h>

static float bound_max_duty_cycle(float max_duty_cycle) {
    /**
     * Bound the max speed to be between 0 and 100
     * 
     * @param max_duty_cycle The max speed
     * 
     * @return The bounded max speed
    */

    float bounded_max_duty_cycle = 0.0;

    if (max_duty_cycle < 0.0) {
        bounded_max_duty_cycle = 0.0;
    } else if (max_duty_cycle > 100.0) {
        bounded_max_duty_cycle = 100.0;
    } else {
        bounded_max_duty_cycle = max_duty_cycle;
    }
    return bounded_max_duty_cycle;
}