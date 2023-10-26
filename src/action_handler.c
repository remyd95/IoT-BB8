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

void move_to(float x, float y, float speed_left, float speed_right) {
    //TODO add action to priority task queue
}


void process_action(char* event_data){
    motor_action_data_t motor_action_data;
    motor_action_data.motor_id = MOTOR_ALL;

    if (strncmp(event_data, "FW", 2) == 0) {
        float speed_left;
        float speed_right;

        if (sscanf(event_data, "FW %f %f", &speed_left, &speed_right) == 2) {
            float bounded_speed_left = bound_max_speed(speed_left);
            float bounded_speed_right = bound_max_speed(speed_right);

            motor_action_data.speed_left = bounded_speed_left;
            motor_action_data.speed_right = bounded_speed_right;

            motor_action_data.speed_left_max = bounded_speed_left;
            motor_action_data.speed_right_max = bounded_speed_right;

            set_current_action(ACTION_FORWARD);
            forward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "BW", 2) == 0) {
        float speed_left;
        float speed_right;

        if (sscanf(event_data, "BW %f %f", &speed_left, &speed_right) == 2) {
            float bounded_speed_left = bound_max_speed(speed_left);
            float bounded_speed_right = bound_max_speed(speed_right);

            motor_action_data.speed_left = bounded_speed_left;
            motor_action_data.speed_right = bounded_speed_right;

            motor_action_data.speed_left_max = bounded_speed_left;
            motor_action_data.speed_right_max = bounded_speed_right;

            set_current_action(ACTION_BACKWARD);
            backward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "TL", 2) == 0) {
        float speed_left;
        float speed_right;

        if (sscanf(event_data, "TL %f %f", &speed_left, &speed_right) == 2) {
            float bounded_speed_left = bound_max_speed(speed_left);
            float bounded_speed_right = bound_max_speed(speed_right);

            motor_action_data_t motor_action_data1;
            motor_action_data_t motor_action_data2;
            
            motor_action_data1.speed_left = bounded_speed_left;
            motor_action_data2.speed_left = bounded_speed_left;
            motor_action_data1.speed_right = bounded_speed_right;
            motor_action_data2.speed_right = bounded_speed_right;

            motor_action_data1.speed_left_max = bounded_speed_left;
            motor_action_data2.speed_left_max = bounded_speed_left;
            motor_action_data1.speed_right_max = bounded_speed_right;
            motor_action_data2.speed_right_max = bounded_speed_right;

            set_current_action(ACTION_TURN_LEFT);
            motor_action_data1.motor_id = MOTOR_LEFT;
            backward_callback(motor_action_data1);
            motor_action_data2.motor_id = MOTOR_RIGHT;
            forward_callback(motor_action_data2);
        }
    } else if (strncmp(event_data, "TR", 2) == 0) {
        float speed_left;
        float speed_right;

        if (sscanf(event_data, "TR %f %f", &speed_left, &speed_right) == 2) {
            float bounded_speed_left = bound_max_speed(speed_left);
            float bounded_speed_right = bound_max_speed(speed_right);

            motor_action_data_t motor_action_data1;
            motor_action_data_t motor_action_data2;
            
            motor_action_data1.speed_left = bounded_speed_left;
            motor_action_data2.speed_left = bounded_speed_left;
            motor_action_data1.speed_right = bounded_speed_right;
            motor_action_data2.speed_right = bounded_speed_right;

            motor_action_data1.speed_left_max = bounded_speed_left;
            motor_action_data2.speed_left_max = bounded_speed_left;
            motor_action_data1.speed_right_max = bounded_speed_right;
            motor_action_data2.speed_right_max = bounded_speed_right;
            
            set_current_action(ACTION_TURN_RIGHT);
            motor_action_data1.motor_id = MOTOR_LEFT;
            forward_callback(motor_action_data1);
            motor_action_data2.motor_id = MOTOR_RIGHT;
            backward_callback(motor_action_data2);
        }
    } else if (strncmp(event_data, "ST", 2) == 0) {
        set_current_action(ACTION_STOP);
        stop_callback(motor_action_data);
        set_current_action(ACTION_IDLE); // TODO: Only set to action idle if we know the velocity is 0

    } else if (strncmp(event_data, "MT", 2) == 0) {
        float x, y;

        float speed_left;
        float speed_right;
        if (sscanf(event_data, "MT %f %f %f %f", &x, &y, &speed_left, &speed_right) == 4) {
            // Change action data to contains these args?
            float bounded_speed_left = bound_max_speed(speed_left);
            float bounded_speed_right = bound_max_speed(speed_right);
            set_current_action(ACTION_MOVETO);
            move_to(x, y, speed_left, speed_right);
        }
    } else if (strncmp(event_data, "IN", 2) == 0) {
        float x, y;
        if (sscanf(event_data, "IN %f %f", &x, &y) == 2) {
            set_current_coordinates(x, y);
            set_current_action(ACTION_IDLE); // After initialization set to idle
        }
    } else if (strncmp(event_data, "FA", 2) == 0) {
        int client_id;
        if (sscanf(event_data, "FA %d", &client_id) == 2) {
            //mqtt_publish_message(mqtt_client, message);
        }
    } else if (strncmp(event_data, "RB", 2) == 0) {
        esp_restart();
    }
}