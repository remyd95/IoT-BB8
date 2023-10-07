#pragma once

#include "ahrs.h"
#include "mpu9250.h"
#include "calibrate_imu.h"
#include "common.h"
#include <esp_log.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define I2C_MASTER_SCL_IO 22     /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21     /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0
//#define CONFIG_CALIBRATION_MODE

#define SAMPLE_FREQ_Hz 500
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_FREQ_Hz) // Sample Rate in milliseconds
#define BETA 0.7
#define DEG2RAD(deg) (deg * M_PI / 180.0f)

typedef struct {
    float temp;
    float heading;
    float pitch;
    float roll;
} imu_data_t;

void imu_task(void *args);

void pause_sample(void);