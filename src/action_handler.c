#include "action_handler.h"
#include "util.h"
#include "state_machine.h"

#include <math.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static MotorActionCallback forward_callback = NULL;
static MotorActionCallback backward_callback = NULL;
static MotorActionCallback stop_callback = NULL;

void set_forward_action_callback(MotorActionCallback callback) {
    forward_callback = callback;
    //TODO add action to priority task queue
}

void set_backward_action_callback(MotorActionCallback callback) {
    backward_callback = callback;
    //TODO add action to priority task queue
}

void set_stop_action_callback(MotorActionCallback callback) {
    stop_callback = callback;
    //TODO add action to priority task queue (HIGH PRIO)
}

void move_to(float x, float y, float max_speed) {
    //TODO add action to priority task queue
}


void process_action(char* event_data){
    motor_action_data_t motor_action_data;

    if (strncmp(event_data, "FW", 2) == 0) {
        float max_speed;

        if (sscanf(event_data, "FW %f", &max_speed) == 1) {
            float bounded_max_speed = bound_max_speed(max_speed);

            motor_action_data.speed = bounded_max_speed;
            motor_action_data.max_speed = bounded_max_speed;
            motor_action_data.motor_id = MOTOR_ALL;

            forward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "BW", 2) == 0) {
        float max_speed;

        if (sscanf(event_data, "BW %f", &max_speed) == 1) {
            float bounded_max_speed = bound_max_speed(max_speed);
            motor_action_data.speed = bounded_max_speed;
            motor_action_data.motor_id = MOTOR_ALL;
            backward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "ST", 2) == 0) {
        motor_action_data.motor_id = MOTOR_ALL;
        stop_callback(motor_action_data);
    } else if (strncmp(event_data, "MT", 2) == 0) {
        float x, y;
        float max_speed;
        if (sscanf(event_data, "MT %f %f %f", &x, &y, &max_speed) == 3) {
            // Change action data to contains these args?
            move_to(x, y, max_speed);
        }
    } else if (strncmp(event_data, "IN", 2) == 0) {
        float x, y;
        if (sscanf(event_data, "IN %f %f", &x, &y) == 2) {
            set_current_coordinates(x, y);
            printf("%f\n", get_current_x_pos());
            printf("%f\n", get_current_y_pos());
        }
    }
}