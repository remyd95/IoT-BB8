#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <stdio.h>
#include <string.h>
#include <esp_system.h>
#include <esp_wifi.h>

extern const int CONNECTED_BIT, NUM_RETRIES;

void init_wifi(EventGroupHandle_t *wifi_event_group, const char* ssid , const char* password);
void wifi_connect();