#pragma once

#include "ahrs.h"
#include "mpu9250.h"
#include <driver/gpio.h>
#include "calibrate_imu.h"
#include "common.h"
#include <esp_log.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ESP32
// #define I2C_MASTER_SCL_IO 22     /*!< gpio number for I2C master clock */
// #define I2C_MASTER_SDA_IO 21     /*!< gpio number for I2C master data  */

//ESP32-S3
#define I2C_MASTER_SCL_IO GPIO_NUM_6     /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_5     /*!< gpio number for I2C master data  */

#define I2C_MASTER_NUM I2C_NUM_0
// #define CONFIG_CALIBRATION_MODE

#define SAMPLE_FREQ_Hz 200.0f
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_FREQ_Hz) // Sample Rate in milliseconds
#define BETA 0.2f
// #define DEG2RAD(deg) (deg * M_PI / 180.0f)

#define GRAVITY 1.00f

typedef struct {
    float temp;
    float heading;
    float pitch;
    float roll;
    float accelx;
    float accely;
    float accelz;
    vector_t compensated_va; 
} imu_data_t;

void imu_task(void *args);
void pause_sample(void);
void compensateGravity(vector_t acc, vector_t *compensated_va);
float degrees_to_radians(float degrees);