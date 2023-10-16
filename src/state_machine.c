#include "state_machine.h"

volatile float current_x = 0.0;
volatile float current_y = 0.0;
volatile float current_rotation = 0.0;
volatile int current_action = ACTION_IDLE;

void set_current_coordinates(float x, float y) {
    current_x = x;
    current_y = y;
}

void set_current_rotation(float rotation) {
    current_rotation = rotation;
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

void report_state_task(void *args) {
    state_task_data_t *state_task_data = (state_task_data_t*)args;    
    esp_mqtt_client_handle_t mqtt_client = state_task_data->mqtt_client;
    state_machine_data_t *state_machine_data = &(state_task_data->state_machine_data);


    while (1) {

        state_machine_data->current_action = get_current_action();
        state_machine_data->current_x = get_current_x_pos();
        state_machine_data->current_y = get_current_y_pos();
        state_machine_data->rotation = get_current_rotation();

        int max_length = snprintf(NULL, 0, "%f %f %f %d", state_machine_data->current_x, state_machine_data->current_y, state_machine_data->rotation, state_machine_data->current_action);
        char message[max_length + 1]; 
        snprintf(message, max_length + 1, "%f %f %f %d", state_machine_data->current_x, state_machine_data->current_y, state_machine_data->rotation, state_machine_data->current_action);


        mqtt_publish_message(mqtt_client, message);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}