#include "imu.h"

// calibration data, replace with offsets from calibration mode
calibration_t cal = {
    .mag_offset = {.x = -14.390625, .y = 26.468750, .z = -12.181641},
    .mag_scale = {.x = 1.011759, .y = 1.016019, .z = 0.973342},
    .accel_offset = {.x = 0.021837, .y = 0.043132, .z = -0.144693},
    .accel_scale_lo = {.x = 1.011026, .y = 1.013828, .z = 0.974942},
    .accel_scale_hi = {.x = -0.987007, .y = -0.980326, .z = -1.048787},
    .gyro_bias_offset = {.x = 0.582807, .y = 0.661291, .z = -1.661415}
};

static void transform_accel_gyro(vector_t *v) {
  /**
   * Transformation:
   *  - Rotate around Z axis 180 degrees
   *  - Rotate around X axis -90 degrees
   * @param  {object} s {x,y,z} sensor
   * @return {object}   {x,y,z} transformed
  */
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -x;
  v->y = -z;
  v->z = -y;
}

static void transform_mag(vector_t *v) {
  /**
   * Transformation: to get magnetometer aligned
   * @param  {object} s {x,y,z} sensor
   * @return {object}   {x,y,z} transformed
   */
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -y;
  v->y = z;
  v->z = -x;
}

void pause_sample(void) {
  /**
   * Pause the IMU sampling to match the sample frequency
   * 
   * @return void
   */
  static uint64_t start = 0;
  uint64_t end = xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;

  if (start == 0) {
    start = xTaskGetTickCount() / configTICK_RATE_HZ;
  }

  int32_t elapsed = end - start;
  if (elapsed < SAMPLE_INTERVAL_MS) {
    vTaskDelay((SAMPLE_INTERVAL_MS - elapsed) / portTICK_PERIOD_MS);
  }

  start = xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
}

static void run_imu(imu_data_t *imu_data) {
  /**
   * Run IMU task
   * 
   * @param imu_data: pointer to imu_data_t struct
   * 
   * @return void
   */
  i2c_mpu9250_init(&cal, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
  ahrs_init(SAMPLE_FREQ_Hz, BETA);

  while (1) {
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
    //ESP_LOGI("imu", "heading: %2.3f°, pitch: %2.3f°, roll: %2.3f°, Temp %2.3f°C", heading, pitch, roll, temp);

    imu_data->heading = heading;
    imu_data->pitch = pitch;
    imu_data->roll = roll;
    imu_data->temp = temp;
    imu_data->accelx = va.x;
    imu_data->accely = va.y;

    pause_sample();
  }
}

static void imu_cleanup(void) {
  /**
   * Cleanup function for IMU
   * 
   * @return void
   */
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  i2c_driver_delete(I2C_MASTER_NUM);
  vTaskDelete(NULL);
}

void imu_task(void *args) {
  /**
   * IMU task function for scheduling
   * 
   * @param args 
   */  
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