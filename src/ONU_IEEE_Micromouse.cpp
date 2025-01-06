#include <iostream>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include <basicSensorTest.h>

//#include <sensor_api.h>
//#include <platform.h>
#include <platform_config.h>

void reset_all_gpio() {
    for (int pin = 0; pin <= 29; ++pin) {
        gpio_deinit(pin);
    }
}

int initializeSensorTest() {
    int status = 0;
    
    printf("IC2 Setup\n");
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    printf("XSHUT Setup\n");
    // Configure XSHUT pin as output
    gpio_init(VL53L4CD_2R_XSHUT_PIN);
    gpio_set_dir(VL53L4CD_2R_XSHUT_PIN, GPIO_OUT);
    gpio_pull_up(VL53L4CD_2R_XSHUT_PIN);

    // Set XSHUT to LOW to enable the sensor
    gpio_put(VL53L4CD_2R_XSHUT_PIN, false);

    printf("GPIO Setup\n");
    // Configure GPIO pin as input
    gpio_init(VL53L4CD_2R_GPIO_PIN);
    gpio_set_dir(VL53L4CD_2R_GPIO_PIN, GPIO_IN);
    gpio_pull_up(VL53L4CD_2R_GPIO_PIN);

    return status; 
}



int main()
{
    stdio_init_all();

    int x = 0;
    while(x < 5) {
        sleep_ms(1000);
        printf("WAIT\n");
        x+=1;
    }

    initializeSensorTest();
	int status = example1(); // SensorTest 1
    reset_all_gpio();

    while (true) {
        printf(status + "\n");
        sleep_ms(1000);
    }
}
