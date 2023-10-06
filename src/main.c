// Local Imports
#include "wifi.h"
#include "mqtt.h"
#include "pwm_motor.h"
#include "action_handler.h"

// Std
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ESP
#include <esp_log.h>

// Drivers
#include <driver/gpio.h>
#include "driver/i2c.h"

// Other
#include <mqtt_client.h>
#include "nvs_flash.h"
#include "lwip/sys.h"
#include "lwip/err.h"

#include "ahrs.h"
#include "mpu9250.h"
#include "calibrate.h"
#include "common.h"

//#define LED_PIN GPIO_NUM_21 // ONBOARD LED PIN FOR ESP32-S3
#define LED_PIN GPIO_NUM_2 // ONBOARD LED PIN FOR ESP32-S3
#define I2C_MASTER_NUM I2C_NUM_0
//#define CONFIG_CALIBRATION_MODE 1

static const char *TAG = "main";
calibration_t cal = {
    .mag_offset = {.x = -17.388672, .y = 12.673828, .z = 3.480469},
    .mag_scale = {.x = 0.994285, .y = 1.033268, .z = 0.974233},
    .accel_offset = {.x = 0.020900, .y = 0.014688, .z = -0.002580},
    .accel_scale_lo = {.x = -0.992052, .y = -0.990010, .z = -1.011147},
    .accel_scale_hi = {.x = 1.013558, .y = 1.011903, .z = 1.019645},

    .gyro_bias_offset = {.x = 0.303956, .y = -1.049768, .z = -0.403782}};
    
// WiFi credentials
const char* ssid = "Phone";
const char* password =  "wifi1234";
 
// WiFi Event Handler
EventGroupHandle_t wifi_event_group;

// MQTT configurations
const char* broker_uri = "mqtt://duijsens.dev";

// MQTT Client initialization
esp_mqtt_client_handle_t mqtt_client;

static void configure_led(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_INPUT_OUTPUT);
}

/**
 * Transformation:
 *  - Rotate around Z axis 180 degrees
 *  - Rotate around X axis -90 degrees
 * @param  {object} s {x,y,z} sensor
 * @return {object}   {x,y,z} transformed
 */
static void transform_accel_gyro(vector_t *v)
{
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
static void transform_mag(vector_t *v)
{
  float x = v->x;
  float y = v->y;
  float z = v->z;

  v->x = -y;
  v->y = z;
  v->z = -x;
}

void run_imu(void)
{

  i2c_mpu9250_init(&cal);
  ahrs_init(SAMPLE_FREQ_Hz, 0.8);

  while (true)
  {
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
    ESP_LOGI(TAG, "heading: %2.3f°, pitch: %2.3f°, roll: %2.3f°, Temp %2.3f°C", heading, pitch, roll, temp);

    // Make the WDT happy
    vTaskDelay(0);

    pause_sample();
  }
}



static void imu_task(void *arg) {

  #ifdef CONFIG_CALIBRATION_MODE
    calibrate_mag();
    calibrate_gyro();
    calibrate_accel();
  #else
    run_imu();
  #endif

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  i2c_driver_delete(I2C_MASTER_NUM);

  vTaskDelete(NULL);
}

void app_main() {
  nvs_flash_init();

  configure_led();
  //pwm_configure_motors();

  init_wifi(&wifi_event_group, ssid, password);

  set_forward_action_callback(pwm_forward_action);
  set_backward_action_callback(pwm_backward_action);
  set_stop_action_callback(pwm_stop_action);

  init_mqtt(&mqtt_client, broker_uri);

  xTaskCreate(imu_task, "imu_task", 4096, NULL, 10, NULL);

  bool connected = false;

  while (1) {
      // do something
      if (xEventGroupGetBits(wifi_event_group) & CONNECTED_BIT) {
          // WiFi connected, turn on the LED
          if (!connected) {
                connected = true;
              gpio_set_level(LED_PIN, 1);
          }
      } else {
          // WiFi not connected, turn off the LED
          connected = false;
          gpio_set_level(LED_PIN, 0);
      }

      // Delay before processing the next message
      vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}