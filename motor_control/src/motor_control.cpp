#include<motor_control.h>

// Calculate correction to ensure robot travels on straight path
float compute_correction()
{
    // Correction to be applied to the robot
    float correction = 0.0;

    // Gyro-based correction is calcualted by finding the horizonal
    // degree acceleration and converting it to degrees.
    gyro_data.get_lock();
    float gyro_error_deg = gyro_data.gz / 131.0;
    gyro_data.release_lock();
    correction += gyro_error_deg * GYRO_CORRECTION_WEIGHT;  // Small weight for gyro correction

    // IR Sensor-based correction
    float ir_correction = 0;
    float left_perp  = left_IR_sensor.read() * 0.707;
    float right_perp = right_IR_sensor.read() * 0.707;

    // If the measured perpendicular widths are greater than the maimum.
    // At least 1 sensor is looking at a gap on at least 1 side
    if (left_perp + right_perp + IR_PLACEMENT_WIDTH_MM > PATH_SIZE_MM)
    {
        // If the minimum is less than than 1/2 the path width we can use 
        // it to calculate correction, else the robot can not use either and
        // must rely on the gyro to correct movement.
        float min_perp = std::min(left_perp, right_perp);
        if (min_perp + IR_PLACEMENT_WIDTH_MM / 2 > PATH_SIZE_MM / 2)
        {
            ir_correction += 0;
        }
        else
        {
            ir_correction += (min_perp - ((PATH_SIZE_MM - IR_PLACEMENT_WIDTH_MM) / 2));
        }
    }    
    else
    {
        ir_correction += (left_perp - right_perp);
    }

    // If not in deadband, correct
    if (std::fabs(ir_correction) > CORRECTION_DEADBAND_MM) {correction += ir_correction * SENSOR_CORRECTION_WEIGHT;}

    return correction;
}

// Set motor speed given a (-1:1) input
void set_motor_speed(uint8_t MOTOR_ID, float speed)
{
    // Clamp input to the range [-1, 1]
    speed = std::clamp(speed, -1.0f, 1.0f);

    float duty1;
    float duty2;
    if (speed > 0) 
    {
        // Forward movement
        duty1 = std::abs(speed);
        duty2 = 0;
    } 
    else if (speed < 0) 
    {
        // Backward movement
        duty1 = 0;
        duty2 = std::abs(speed);
    } 
    else 
    {
        // Brake
        duty1 = 1;  
        duty2 = 1;
    }

    // Set PWM
    switch(MOTOR_ID)
    {
        case 0: 
        {
            pwm_set_chan_level(pwm_gpio_to_slice_num(AIN1), pwm_gpio_to_channel(AIN1), duty1 * PWM_CLOCK_TOP);
            pwm_set_chan_level(pwm_gpio_to_slice_num(AIN2), pwm_gpio_to_channel(AIN2), duty2 * PWM_CLOCK_TOP);
        }
        case 1:
        {
            pwm_set_chan_level(pwm_gpio_to_slice_num(BIN1), pwm_gpio_to_channel(BIN1), duty1 * PWM_CLOCK_TOP);
            pwm_set_chan_level(pwm_gpio_to_slice_num(BIN2), pwm_gpio_to_channel(BIN2), duty2 * PWM_CLOCK_TOP);
        }
        default: {}
    };
}