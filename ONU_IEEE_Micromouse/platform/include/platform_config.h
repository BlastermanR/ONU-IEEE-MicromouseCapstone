#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

// Define default sensor address
#define VL53L4CD_ID 0x52

// Define sensors
#define VL53L4CD_2R_XSHUT_PIN 0
#define VL53L4CD_2R_GPIO_PIN  5

#endif // PLATFORM_CONFIG_H