#include "action_handler.h"
#include "util.h"
#include "state_machine.h"

#include <math.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


void process_action(char* event_data){
    if (strncmp(event_data, "FW", 2) == 0) {
        float target_speed;

        if (sscanf(event_data, "FW %f", &target_speed) == 1) {
            float bounded_target_speed = bound_max_speed(target_speed);
            set_current_action(ACTION_FORWARD);
            set_target_speed(bounded_target_speed);
        }
    } else if (strncmp(event_data, "BW", 2) == 0) {
        float target_speed;

        if (sscanf(event_data, "BW %f", &target_speed) == 1) {
            float bounded_target_speed = bound_max_speed(target_speed);
            set_current_action(ACTION_BACKWARD);
            set_target_speed(bounded_target_speed);
        }
    } else if (strncmp(event_data, "TL", 2) == 0) {
        float target_speed;

        if (sscanf(event_data, "TL %f", &target_speed) == 1) {
            float bounded_target_speed = bound_max_speed(target_speed);
            set_current_action(ACTION_TURN_LEFT);
            set_target_speed(bounded_target_speed);

            // OLD CODE - Keep for reference
            //motor_action_data1.motor_id = MOTOR_LEFT;
            //backward_callback(motor_action_data1);
            //motor_action_data2.motor_id = MOTOR_RIGHT;
            //forward_callback(motor_action_data2);
        }
    } else if (strncmp(event_data, "TR", 2) == 0) {
        float target_speed;

        if (sscanf(event_data, "TR %f", &target_speed) == 1) {
            float bounded_target_speed = bound_max_speed(target_speed);
            set_current_action(ACTION_TURN_RIGHT);
            set_target_speed(bounded_target_speed);

            // OLD CODE - Keep for reference
            //motor_action_data1.motor_id = MOTOR_LEFT;
            //forward_callback(motor_action_data1);
            //motor_action_data2.motor_id = MOTOR_RIGHT;
            //backward_callback(motor_action_data2);
        }
    } else if (strncmp(event_data, "ST", 2) == 0) {
        set_current_action(ACTION_STOP);
        set_target_speed(0.0);
    } else if (strncmp(event_data, "MT", 2) == 0) {
        float x, y, target_speed;

        if (sscanf(event_data, "MT %f %f %f", &x, &y, &target_speed) == 3) {
            // Change action data to contains these args?
            float bounded_target_speed = bound_max_speed(target_speed);
            set_current_action(ACTION_MOVETO);
            set_target_coordinates(x, y);
            set_target_speed(bounded_target_speed);
        }
    } else if (strncmp(event_data, "IN", 2) == 0) {
        float x, y;
        if (sscanf(event_data, "IN %f %f", &x, &y) == 2) {
            set_current_coordinates(x, y);
            set_current_action(ACTION_IDLE); // After initialization set to idle
        }
    } else if (strncmp(event_data, "RB", 2) == 0) {
        esp_restart();
    }
}
