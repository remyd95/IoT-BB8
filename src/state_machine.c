#include "state_machine.h"

volatile State current_state = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, ACTION_INIT};

void set_current_coordinates(float x, float y) {
    current_state.x = x;
    current_state.y = y;
}

void set_current_rotation(float rotation) {
    current_state.rotation = rotation;
}

void set_current_pitch(float pitch) {
    current_state.pitch = pitch;
}

void set_current_roll(float roll) {
    current_state.roll = roll;
}

void set_current_speed(float speed) {
    current_state.speed = speed;
}

void set_current_action(int action) {
    current_state.action = action;
}

void set_current_acceleration(float acceleration) {
    current_state.acceleration = acceleration;
}

float get_current_x_pos() {
    return current_state.x;
}

float get_current_y_pos() {
    return current_state.y;
}

float get_current_rotation() {
    return current_state.rotation;
}

float get_current_pitch() {
    return current_state.pitch;
}

float get_current_roll() {
    return current_state.roll;
}

int get_current_action() {
    return current_state.action;
}

float get_current_speed() {
    return current_state.speed;
}

float get_current_acceleration() {
    return current_state.acceleration;
}


void report_state_task(void *args) {
    esp_mqtt_client_handle_t* mqtt_client = (esp_mqtt_client_handle_t*)args;

    while (1) {
        if (current_state.action != ACTION_INIT) {

            int max_length = snprintf(NULL, 0, "%f %f %f %d", current_state.x, current_state.y, current_state.rotation, current_state.action);
            char message[max_length + 1]; 
            snprintf(message, max_length + 1, "%f %f %f %d", current_state.x, current_state.y, current_state.rotation, current_state.action);

            mqtt_publish_message(*mqtt_client, message);

        }
        vTaskDelay(STATE_INTERVAL_TIME_MS / portTICK_PERIOD_MS);
    }

}