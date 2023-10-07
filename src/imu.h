#pragma once

#include "ahrs.h"
#include "mpu9250.h"
#include "calibrate_imu.h"
#include "common.h"
#include <esp_log.h>

#define I2C_MASTER_NUM I2C_NUM_0
//#define CONFIG_CALIBRATION_MODE

typedef struct {
    float temp;
    float heading;
    float pitch;
    float roll;
} imu_data_t;

void imu_task(void *args);