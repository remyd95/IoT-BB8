#include "state_machine.h"

volatile State current_state = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, ACTION_INIT};
volatile Target target = {0.0, 0.0, 0.0};

void set_current_coordinates(float x, float y) {
    /**
     * Set the current coordinates
     * 
     * @param x The current x coordinate
     * @param y The current y coordinate
     * 
     * @return void
    */
    current_state.x = x;
    current_state.y = y;
}

void set_current_rotation(float rotation) {
    /**
     * Set the current rotation
     * 
     * @param rotation The current rotation
     * 
     * @return void
    */
    current_state.rotation = rotation;
}

void set_current_pitch(float pitch) {
    /**
     * Set the current pitch
     * 
     * @param pitch The current pitch
     * 
     * @return void
    */
    current_state.pitch = pitch;
}

void set_current_roll(float roll) {
    /**
     * Set the current roll
     * 
     * @param roll The current roll
     * 
     * @return void
    */
    current_state.roll = roll;
}

void set_current_speed(float speed) {
    /**
     * Set the current speed
     * 
     * @param speed The current speed
     * 
     * @return void
    */
    current_state.speed = speed;
}

void set_current_duty_cycle(float duty_cycle) {
    /**
     * Set the current duty cycle
     * 
     * @param duty_cycle The current duty cycle
     * 
     * @return void
    */
    current_state.duty_cycle = duty_cycle;
}

void set_current_action(int action) {
    /**
     * Set the current action
     * 
     * @param action The current action
     * 
     * @return void
    */
    current_state.action = action;
}

void set_current_acceleration(float acceleration) {
    /**
     * Set the current acceleration
     * 
     * @param acceleration The current acceleration
     * 
     * @return void
    */
    current_state.acceleration = acceleration;
}

void set_target_coordinates(float x, float y) {
    /**
     * Set the target coordinates
     * 
     * @param x The target x coordinate
     * @param y The target y coordinate
     * 
     * @return void
    */
    target.x = x;
    target.y = y;
}

void set_target_duty_cycle(float duty_cycle) {
    /**
     * Set the target duty cycle
     * 
     * @param duty_cycle The target duty cycle
     * 
     * @return void
    */
    target.duty_cycle = duty_cycle;
}

State get_current_state() {
    /**
     * Get the current state
     * 
     * @return The current state
    */
    return current_state;
}

Target get_target() {
    /**
     * Get the target
     * 
     * @return The target
    */
    return target;
}

float get_current_x_pos() {
    /**
     * Get the current x position
     * 
     * @return The current x position
    */
    return current_state.x;
}

float get_current_y_pos() {
    /**
     * Get the current y position
     * 
     * @return The current y position
    */
    return current_state.y;
}

float get_current_rotation() {
    /**
     * Get the current rotation
     * 
     * @return The current rotation
    */
    return current_state.rotation;
}

float get_current_pitch() {
    /**
     * Get the current pitch
     * 
     * @return The current pitch
    */
    return current_state.pitch;
}

float get_current_roll() {
    /**
     * Get the current roll
     * 
     * @return The current roll
    */
    return current_state.roll;
}

int get_current_action() {
    /**
     * Get the current action
     * 
     * @return The current action
    */
    return current_state.action;
}

float get_current_duty_cycle() {
    /**
     * Get the current duty cycle
     * 
     * @return The current duty cycle
    */
    return current_state.duty_cycle;
}

float get_current_speed() {
    /**
     * Get the current speed
     * 
     * @return The current speed
    */
    return current_state.speed;
}

float get_current_acceleration() {
    /**
     * Get the current acceleration
     * 
     * @return The current acceleration
    */
    return current_state.acceleration;
}

float get_target_x_pos() {
    /**
     * Get the target x position
     * 
     * @return The target x position
    */
    return target.x;
}

float get_target_y_pos() {
    /**
     * Get the target y position
     * 
     * @return The target y position
    */
    return target.y;
}

float get_target_duty_cycle() {
    /**
     * Get the target duty cycle
     * 
     * @return The target duty cycle
    */
    return target.duty_cycle;
}

void report_state_task(void *args) {
    /**
     * Report the current state to the MQTT broker
     * 
     * @param args The MQTT client handle
     * 
     * @return void
    */
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