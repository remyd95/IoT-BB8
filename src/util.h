#pragma once

#include <stdbool.h>
#include <ctype.h>

static float clip_duty_cycle(float duty_cycle) {
    float bounded_duty_cycle = 0;

    if (duty_cycle < 0) {
        bounded_duty_cycle = 0;
    } else if (duty_cycle >= 100) {
        bounded_duty_cycle = 99;
    } else {
        bounded_duty_cycle = duty_cycle;
    }
    return bounded_duty_cycle;
}