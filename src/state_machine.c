#include "state_machine.h"

volatile float current_x = 0.0;
volatile float current_y = 0.0;
volatile float current_rotation = 0.0;
volatile float current_speed = 0.0;
volatile int current_action = ACTION_INIT;

void set_current_coordinates(float x, float y) {
    current_x = x;
    current_y = y;
}

void set_current_rotation(float rotation) {
    current_rotation = rotation;
}

void set_current_speed(float speed) {
    current_speed = speed;
}

void set_current_action(int action) {
    current_action = action;
}

float get_current_x_pos() {
    return current_x;
}

float get_current_y_pos() {
    return current_y;
}

float get_current_rotation() {
    return current_rotation;
}

int get_current_action() {
    return current_action;
}

float get_current_speed() {
    return current_speed;
}

void report_state_task(void *args) {
    esp_mqtt_client_handle_t* mqtt_client = (esp_mqtt_client_handle_t*)args;

    while (1) {
        if (current_action != ACTION_INIT) {

            //TODO: Also include IMU data (yaw, pitch, roll, accel/speed?)

            int max_length = snprintf(NULL, 0, "%f %f %f %d", current_x, current_y, current_rotation, current_action);
            char message[max_length + 1]; 
            snprintf(message, max_length + 1, "%f %f %f %d", current_x, current_y, current_rotation, current_action);

            mqtt_publish_message(*mqtt_client, message);

        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}