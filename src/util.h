#pragma once

#include <stdbool.h>
#include <ctype.h>

static bool is_valid_integer(const char *str) {
    // Check if the entire string is composed of digits
    while (*str) {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }
    return true;
}