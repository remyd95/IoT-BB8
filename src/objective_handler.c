#include "objective_handler.h"
#include "util.h"
#include "state_machine.h"

#include <math.h>
#include <string.h>


void process_objective_message(char* event_data) {
    /**
     * Process the objective received from the MQTT broker
     * 
     * @param event_data The data received from the MQTT broker
     * 
     * @return void
    */
    if (strncmp(event_data, "FW", 2) == 0) {
        float max_duty_cycle;

        if (sscanf(event_data, "FW %f", &max_duty_cycle) == 1) {
            float bounded_max_duty_cycle = bound_max_duty_cycle(max_duty_cycle);
            set_current_objective(OBJECTIVE_FORWARD);
            set_target_duty_cycle(bounded_max_duty_cycle);
        }
    } else if (strncmp(event_data, "BW", 2) == 0) {
        float max_duty_cycle;

        if (sscanf(event_data, "BW %f", &max_duty_cycle) == 1) {
            float bounded_max_duty_cycle = bound_max_duty_cycle(max_duty_cycle);
            set_current_objective(OBJECTIVE_BACKWARD);
            set_target_duty_cycle(bounded_max_duty_cycle);
        }
    } else if (strncmp(event_data, "TL", 2) == 0) {
        float max_duty_cycle;

        if (sscanf(event_data, "TL %f", &max_duty_cycle) == 1) {
            float bounded_max_duty_cycle = bound_max_duty_cycle(max_duty_cycle);
            set_current_objective(OBJECTIVE_TURN_LEFT);
            set_target_duty_cycle(bounded_max_duty_cycle);
        }
    } else if (strncmp(event_data, "TR", 2) == 0) {
        float max_duty_cycle;

        if (sscanf(event_data, "TR %f", &max_duty_cycle) == 1) {
            float bounded_max_duty_cycle = bound_max_duty_cycle(max_duty_cycle);
            set_current_objective(OBJECTIVE_TURN_RIGHT);
            set_target_duty_cycle(bounded_max_duty_cycle);
        }
    } else if (strncmp(event_data, "ST", 2) == 0) {
        set_current_objective(OBJECTIVE_STOP);
        set_target_duty_cycle(0.0);
    } else if (strncmp(event_data, "MT", 2) == 0) {
        float x, y, max_duty_cycle;

        if (sscanf(event_data, "MT %f %f %f", &x, &y, &max_duty_cycle) == 3) {
            float bounded_max_duty_cycle = bound_max_duty_cycle(max_duty_cycle);
            set_current_objective(OBJECTIVE_MOVETO);
            set_target_coordinates(x, y);
            set_target_duty_cycle(bounded_max_duty_cycle);
        }
    } else if (strncmp(event_data, "IN", 2) == 0) {
        float x, y;
        if (sscanf(event_data, "IN %f %f", &x, &y) == 2) {
            set_current_coordinates(x, y);
            set_current_objective(OBJECTIVE_NONE);
        }
    } else if (strncmp(event_data, "RB", 2) == 0) {
        set_current_objective(OBJECTIVE_REBOOT);
    }
}
