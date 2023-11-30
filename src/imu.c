#include "imu.h"

// calibration data, replace with offsets from calibration mode
calibration_t cal = {
    // .mag_offset = {.x = -8.394531, .y = 4.224609, .z = 19.142578},
    // .mag_scale = {.x = 1.011759, .y = 1.016019, .z = 0.973342},
    // // .accel_offset = {.x = 0.021837, .y = 0.043132, .z = -0.144693},
    // // .accel_scale_lo = {.x = 1.011026, .y = 1.013828, .z = 0.974942},
    // // .accel_scale_hi = {.x = -0.987007, .y = -0.980326, .z = -1.048787},
    // .accel_offset = {.x = 0.021837, .y =  0.043132, .z = 0.9}, //mine
    // .accel_scale_lo = {.x = 1.011026, .y = 1.013828, .z = 0.974942}, //mine
    // .accel_scale_hi = {.x = -0.987007, .y = -0.980326, .z = -1.048787}, //mine
    // .gyro_bias_offset = {.x = 0.582807, .y = 0.661291, .z = -1.661415},

    // .mag_offset = {.x = -19.787109, .y = 71.585938, .z = -80.050781},
    // .mag_scale = {.x = 1.234878, .y = 1.413219, .z = 0.674491},
    // .accel_offset = {.x = -0.016534, .y = 0.097984, .z = -0.111284},
    // .accel_scale_lo = {.x = 0.997406, .y = 1.042805, .z = 0.955728},
    // .accel_scale_hi = {.x = -1.007638, .y = -0.957327, .z = -1.050301},
    // .gyro_bias_offset = {.x = 1.229639, .y = -1.236117, .z = 0.032385}   

    // .mag_offset = {.x = -19.787109, .y = 71.585938, .z = -80.050781},
    // .mag_scale = {.x = 1.234878, .y = 1.413219, .z = 0.674491},
    // .accel_offset = {.x = -0.016534, .y = 0.097984, .z = -0.111284},
    // .accel_scale_lo = {.x = 0.997406, .y = 1.10, .z = 0.955728},
    // .accel_scale_hi = {.x = -1.007638, .y = -1.0, .z = -1.15}, //Changed manually
    // .gyro_bias_offset = {.x = 1.229639, .y = -1.236117, .z = 0.032385}   

    // .accel_offset = {.x = 0.004978, .y = 0.096250, .z = -0.053031},
    // .accel_scale_lo = {.x = 1.000143, .y = 1.044014, .z = 0.957282},
    // .accel_scale_hi = {.x = -0.989888, .y = -0.953421, .z = -1.042077},
    // .gyro_bias_offset = {.x = 1.719505, .y = -0.868046, .z = -0.795749},
    // .mag_offset = {.x = -91.140625, .y = 46.921875, .z = -392.132812},
    // .mag_scale = {.x = 1.002172, .y = 1.085780, .z = 0.924924},

    .accel_offset = {.x = 0.004978, .y = 0.096250, .z = -0.053031},
    .accel_scale_lo = {.x = 1.000143, .y = 1.044014, .z = 0.957282},
    .accel_scale_hi = {.x = -0.989888, .y = -0.953421, .z = -1.042077},
    .gyro_bias_offset = {.x = 1.719505, .y = -0.868046, .z = -0.795749},
    .mag_offset = {.x = -91.140625, .y = 46.921875, .z = -392.132812},
    .mag_scale = {.x = 1.002172, .y = 1.085780, .z = 0.924924},
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
    ESP_ERROR_CHECK_WITHOUT_ABORT(get_accel_gyro_mag(&va, &vg, &vm));

    // Transform these values to the orientation of our device.
    transform_accel_gyro(&va);
    transform_accel_gyro(&vg);
    transform_mag(&vm);

    // Apply the AHRS algorithm
    ahrs_update(0.5*degrees_to_radians(vg.x), 0.5*degrees_to_radians(vg.y), 0.5*degrees_to_radians(vg.z),
                va.x, va.y, va.z,
                0.0f, 0.0f, 0.0f);

    vector_t compensated_va;
    compensateGravity(va, &compensated_va);

    float temp;
    ESP_ERROR_CHECK_WITHOUT_ABORT(get_temperature_celsius(&temp));
    
    float heading, pitch, roll;
    ahrs_get_euler_in_degrees(&heading, &pitch, &roll);
    //ESP_LOGI("imu", "heading: %2.3f°, pitch: %2.3f°, roll: %2.3f°, Temp %2.3f°C", heading, pitch, roll, temp);

    imu_data->heading = heading;
    imu_data->pitch = pitch;
    imu_data->roll = roll;
    imu_data->temp = temp;
    imu_data->accelx = va.x;
    imu_data->accely = va.y;
    imu_data->accelz = va.z;
    imu_data->compensated_va = compensated_va;

    pause_sample();
  }
}

void compensateGravity(vector_t acc, vector_t *compensated_va) {
  /**
   * Compensate accelerometer for gravity
  */
  vector_t g[3];
  g->x = 2 * (q1 * q3 - q0 * q2) * GRAVITY;
  g->y = 2 * (q0 * q1 + q2 * q3) * GRAVITY;
  g->z = (q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) * GRAVITY;

  compensated_va->x = acc.x - g->x;
  compensated_va->y = acc.y - g->y;
  compensated_va->z = acc.z - g->z; 
}

float degrees_to_radians(float degrees) {
  /**
   * Convert degrees to radians
   * 
   * @param degrees: degrees to convert
   * 
   * @return radians
   */
  return degrees * M_PI / 180.0f;
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