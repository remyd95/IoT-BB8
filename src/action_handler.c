#include "action_handler.h"
#include "util.h"
#include "location.h"

#include <math.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static MotorActionCallback forward_callback = NULL;
static MotorActionCallback backward_callback = NULL;
static MotorActionCallback stop_callback = NULL;


void set_forward_action_callback(MotorActionCallback callback) {
    forward_callback = callback;
}

void set_backward_action_callback(MotorActionCallback callback) {
    backward_callback = callback;
}

void set_stop_action_callback(MotorActionCallback callback) {
    stop_callback = callback;
}

void move_to(float x, float y) {
    const float BALL_DIAMETER = 0.12; // m
    const float BALL_RADIUS = BALL_DIAMETER / 2.0; // m
    const float DECELERATION_RATE = 1.0;  // m/s^2 
    const float RPM = 200; // 200RPM on 4.5V see specification

    float current_x = get_current_x_pos();
    float current_y = get_current_y_pos();
    
    float target_y = y;

    float velocity = (2.0 * M_PI * BALL_RADIUS * RPM) / 60.0;
    float stop_time = velocity / DECELERATION_RATE;
    float deceleration_distance = (velocity * stop_time) - (0.5 * DECELERATION_RATE * stop_time * stop_time);
    float distance_to_travel = fabs(target_y - current_y - deceleration_distance);
    float travel_time = distance_to_travel / velocity;

    motor_action_data_t motor_action_data = {.duty_cycle = 100};

    if (target_y > current_y) {
        forward_callback(motor_action_data);
    } else {
        backward_callback(motor_action_data);
    }

    vTaskDelay(travel_time*1000 / portTICK_PERIOD_MS);

    stop_callback(motor_action_data);
    
    set_current_coordinates(current_x, target_y);
}


void process_action(char* event_data){

    motor_action_data_t motor_action_data;
    if (strncmp(event_data, "FW", 2) == 0) {
        float duty_cycle;

        if (sscanf(event_data, "FW %f", &duty_cycle) == 1) {
            float bounded_duty_cycle = clip_duty_cycle(duty_cycle);
            motor_action_data.duty_cycle = bounded_duty_cycle;
            forward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "BW", 2) == 0) {
        float duty_cycle;

        if (sscanf(event_data, "BW %f", &duty_cycle) == 1) {
            float bounded_duty_cycle = clip_duty_cycle(duty_cycle);
            motor_action_data.duty_cycle = bounded_duty_cycle;
            backward_callback(motor_action_data);
        }
    } else if (strncmp(event_data, "ST", 2) == 0) {
        stop_callback(motor_action_data);
    } else if (strncmp(event_data, "MT", 2) == 0) {
        float x, y;
        if (sscanf(event_data, "MT %f %f", &x, &y) == 2) {
            move_to(x, y);
        }
    }

}