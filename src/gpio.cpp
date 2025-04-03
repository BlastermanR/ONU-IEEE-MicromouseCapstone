#include <gpio.h>

void define_gpio() {
    // Initialize USB Serial for debug output
    stdio_init_all();

    // Setup GPIO
    // XSHUT
    gpio_init(S1_XSHUT);
    gpio_set_dir(S1_XSHUT, GPIO_OUT);
    gpio_pull_up(S1_XSHUT);

    gpio_init(S2_XSHUT);
    gpio_set_dir(S2_XSHUT, GPIO_OUT);
    gpio_pull_up(S2_XSHUT);

    gpio_init(S3_XSHUT);
    gpio_set_dir(S3_XSHUT, GPIO_OUT);
    gpio_pull_up(S3_XSHUT);

    // GPIO
    gpio_init(S1_GPIO);
    gpio_set_dir(S1_GPIO, GPIO_IN);
    gpio_pull_up(S1_GPIO);

    gpio_init(S2_GPIO);
    gpio_set_dir(S2_GPIO, GPIO_IN);
    gpio_pull_up(S2_GPIO);

    gpio_init(S3_GPIO);
    gpio_set_dir(S3_GPIO, GPIO_IN);
    gpio_pull_up(S3_GPIO);

    // LED
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);

    gpio_init(LED2);
    gpio_set_dir(LED2, GPIO_OUT);
    
    gpio_init(LED3);
    gpio_set_dir(LED3, GPIO_OUT);

    // Switches
    gpio_init(SW1);
    gpio_set_dir(SW1, GPIO_IN);
    gpio_pull_down(SW1);

    gpio_init(SW2);
    gpio_set_dir(SW2, GPIO_IN);
    gpio_pull_down(SW2);

    gpio_init(SW3);
    gpio_set_dir(SW3, GPIO_IN);
    gpio_pull_down(SW3);

    // Motor Control
    gpio_init(AIN1);
    gpio_set_dir(AIN1, GPIO_OUT);
    gpio_set_function(AIN1, GPIO_FUNC_PWM);

    gpio_init(AIN2);
    gpio_set_dir(AIN2, GPIO_OUT);
    gpio_set_function(AIN2, GPIO_FUNC_PWM);
    
    gpio_init(BIN1);
    gpio_set_dir(BIN1, GPIO_OUT);
    gpio_set_function(BIN1, GPIO_FUNC_PWM);
    
    gpio_init(BIN2);
    gpio_set_dir(BIN2, GPIO_OUT);
    gpio_set_function(BIN2, GPIO_FUNC_PWM);

    // Motor Input
    gpio_init(CAL_LOGIC);
    gpio_set_dir(CAL_LOGIC, GPIO_IN); 

    gpio_init(CBL_LOGIC);
    gpio_set_dir(CBL_LOGIC, GPIO_IN); 

    gpio_init(CAR_LOGIC);
    gpio_set_dir(CAR_LOGIC, GPIO_IN); 

    gpio_init(CBR_LOGIC);
    gpio_set_dir(CBR_LOGIC, GPIO_IN); 
    printf("GPIO configured\n");

    // Setup PWM
    // Set GPIO Mode
    gpio_set_function(AIN1, GPIO_FUNC_SIO);
    gpio_set_function(AIN2, GPIO_FUNC_SIO);
    gpio_set_function(BIN1, GPIO_FUNC_SIO);
    gpio_set_function(BIN2, GPIO_FUNC_SIO);

    // Set HIGH to brake
    gpio_put(AIN1, 1);
    gpio_put(AIN2, 1);
    gpio_put(BIN1, 1);
    gpio_put(BIN2, 1);

    // Set PWM frequency
    uint AIN1_SLICE = pwm_gpio_to_slice_num(AIN1); 
    uint AIN2_SLICE = pwm_gpio_to_slice_num(AIN2);
    uint BIN1_SLICE = pwm_gpio_to_slice_num(BIN1);
    uint BIN2_SLICE = pwm_gpio_to_slice_num(BIN2);
    
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, PWM_CLOCK_DIVIDER); // Adjust clock divisor
    pwm_config_set_wrap(&config, PWM_CLOCK_TOP);  // Sets the PWM frequency
    
    // Initialize PWM
    pwm_init(AIN1_SLICE, &config, false);
    pwm_init(AIN2_SLICE, &config, false);
    pwm_init(BIN1_SLICE, &config, false);
    pwm_init(BIN2_SLICE, &config, false);
}

void configure_i2c()
{
    // Set GPIO
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    i2c_init(I2C_PORT, I2C_CLOCK);
}

bool configure_ir_address()
{
    VL53L4CD_Version_t* software_info;
    int status = VL53L4CD_GetSWVersion(software_info);
    if (status != 0)
    {
        printf("Failed to get VL53L4CD Driver Info");
    }
    else 
    {
        printf("VL53L4CD Driver Info:\n");
        printf("Major Version: %d\n", software_info->major);
        printf("Minor Version: %d\n", software_info->minor);
        printf("Build Version: %d\n", software_info->build);
        printf("Revision: %d\n", software_info->revision);
    }

    // Lower All Pins
    gpio_put(S1_XSHUT, 0);
    gpio_put(S2_XSHUT, 0);
    gpio_put(S3_XSHUT, 0);

    // Set Address for each XSHUT Pin
    for(int i = 0; i < VL53L4CD_SENSOR_COUNT; i++) {
        sleep_ms(1);
        printf("SETTING SENSOR %d ADDRESS:\n", i);
        gpio_put(IR_XSHUT_PINS[i], 1); // Rasie XSHUT Pin

        // Test Connection
        if(!is_i2c_valid(VL53L4CD_ID))
        {
            printf("    No Device at address 0x%02X\n", VL53L4CD_ID);
            return false;
        }
        printf("    Setting Sensor %d address is valid!\n", i);

        // Attempt to change address if default
        int status = 0;
        uint16_t sensor_id;
        if(status = VL53L4CD_SetI2CAddress(VL53L4CD_ID, VL53L4CD_ADDRESSES[i] << 1)) // Shift left because function looks at top 7 bits
        {
            printf("    Failed to change device ID to 0x%02X --- %d\n", VL53L4CD_ADDRESSES[i], status);
            return false;
        }

        uint8_t status1 = VL53L4CD_GetSensorId(VL53L4CD_ADDRESSES[i], &sensor_id);
        if(status1 || (sensor_id != 0xEBAA))
        {
            printf("    VL53L4CD not detected at new address\n");
            return false;
        }

        printf("    Sensor %d address set!\n", i);
    }
    printf("All sensor addresses changed sucessfully\n");
    return true;
}

void i2c_scan() {
    printf("Scanning I2C bus...\n");
    for (uint8_t address = 0x08; address <= 0x77; address++) 
    {   // Valid I2C addresses
        if (is_i2c_valid(address)) { printf("Device found at 0x%02X\n", address); }
    }
    printf("Scan complete.\n");
}

bool is_i2c_valid(uint8_t address) 
{
    sleep_ms(1);
    uint8_t dummy;
    int ret = i2c_read_blocking(I2C_PORT, address, &dummy, 1, false);
    if (ret >= 0) { return  true; }
    else { return false; }
}

int read_VL53L4CD(uint8_t address, uint16_t &data)
{
    VL53L4CD_ResultsData_t* results;
    int status = VL53L4CD_GetResult(address, results);
    status |= VL53L4CD_ClearInterrupt(address);
    data = results->distance_mm;
    if (data > MAX_IR_RANGE_MM) {data = MAX_IR_RANGE_MM;}
    delete(results);
    return status;
}

bool VL53L4CD_setup()
{
    // Initialize Sensors
    printf("Initializing VL53L4CD Sensors\n");
    int status = 0;
    for(int i = 0; i < VL53L4CD_SENSOR_COUNT; i++)
    {
        status |= VL53L4CD_SensorInit(VL53L4CD_ADDRESSES[i]);
    }
	if(status)
	{
		printf("VL53L4CD ULDs loading failed with status %d\n", status);
		return false;
	}
    printf("VL53L4CD ULDs ready !\n");

    // Start Ranging
    for(int i = 0; i < VL53L4CD_SENSOR_COUNT; i++)
    {
        status |= VL53L4CD_StartRanging(VL53L4CD_ADDRESSES[i]);
    }
	if(status)
	{
		printf("VL53L4CD ranging failed\n");
		return false;
	}

    return true;
}