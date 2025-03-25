#include <platform_config.h>

const uint8_t VL53L4CD_ADDRESSES[VL53L4CD_SENSOR_COUNT] = {0x30, 0x31, 0x32};
const uint8_t IR_XSHUT_PINS[VL53L4CD_SENSOR_COUNT] = {0, 1, 2};
const uint8_t IR_GPIO_PINS[VL53L4CD_SENSOR_COUNT] = {3, 6, 7};
const int PWM_PINS[NUM_PWM_PINS] = {16, 17, 18, 19};