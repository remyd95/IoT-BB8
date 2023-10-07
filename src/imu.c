#include "imu.h"

//static const char *TAG = "imu";

calibration_t cal = {
    .mag_offset = {.x = -8.394531, .y = 4.224609, .z = 19.142578},
    .mag_scale = {.x = 1.011759, .y = 1.016019, .z = 0.973342},
    .accel_offset = {.x = 0.021837, .y = 0.043132, .z = -0.144693},
    .accel_scale_lo = {.x = 1.011026, .y = 1.013828, .z = 0.974942},
    .accel_scale_hi = {.x = -0.987007, .y = -0.980326, .z = -1.048787},
    .gyro_bias_offset = {.x = 0.582807, .y = 0.661291, .z = -1.661415}
};

/**
 * Transformation:
 *  - Rotate around Z axis 180 degrees
 *  - Rotate around X axis -90 degrees
 * @param  {object} s {x,y,z} sensor
 * @return {object}   {x,y,z} transformed
 */
static void transform_accel_gyro(vector_t *v) {
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -x;
  v->y = -z;
  v->z = -y;
}

/**
 * Transformation: to get magnetometer aligned
 * @param  {object} s {x,y,z} sensor
 * @return {object}   {x,y,z} transformed
 */
static void transform_mag(vector_t *v) {
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -y;
  v->y = z;
  v->z = -x;
}

static void run_imu(imu_data_t *imu_data) {

  i2c_mpu9250_init(&cal);
  ahrs_init(SAMPLE_FREQ_Hz, BETA);

  while (true) {
    vector_t va, vg, vm;

    // Get the Accelerometer, Gyroscope and Magnetometer values.
    ESP_ERROR_CHECK(get_accel_gyro_mag(&va, &vg, &vm));

    // Transform these values to the orientation of our device.
    transform_accel_gyro(&va);
    transform_accel_gyro(&vg);
    transform_mag(&vm);

    // Apply the AHRS algorithm
    ahrs_update(DEG2RAD(vg.x), DEG2RAD(vg.y), DEG2RAD(vg.z),
                va.x, va.y, va.z,
                vm.x, vm.y, vm.z);

    float temp;
    ESP_ERROR_CHECK(get_temperature_celsius(&temp));
    
    float heading, pitch, roll;
    ahrs_get_euler_in_degrees(&heading, &pitch, &roll);
    //ESP_LOGI(TAG, "heading: %2.3f°, pitch: %2.3f°, roll: %2.3f°, Temp %2.3f°C", heading, pitch, roll, temp);

    imu_data->heading = heading;
    imu_data->pitch = pitch;
    imu_data->roll = roll;
    imu_data->temp = temp;
      
    pause_sample();
  }
}

static void imu_cleanup(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    i2c_driver_delete(I2C_MASTER_NUM);
    vTaskDelete(NULL);
}

void imu_task(void *args) {

    imu_data_t* imu_data = (imu_data_t*)args;    
    #ifdef CONFIG_CALIBRATION_MODE
        calibrate_gyro();
        calibrate_accel();
        calibrate_mag();
    #else
        run_imu(imu_data);
    #endif

    //If IMU stopped, cleanup
    imu_cleanup();
}