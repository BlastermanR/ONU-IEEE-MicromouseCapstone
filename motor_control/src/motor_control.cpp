#include<motor_control.h>

// Local global variables for motor actions
bool motor_action_in_progress = false;
volatile int64_t target_left_motor_rotation_steps = 0;
volatile int64_t target_right_motor_rotation_steps = 0;
float temp_left_motor_set_speed = 0;
float temp_right_motor_set_speed = 0;

// PID variables
float previous_error = 0.0;
float integral = 0.0;

// Calculate correction to ensure robot travels on straight path
float compute_correction()
{
    float correction = 0.0;

    // Step 1: Compute Error
    // Gyro Data
    gyro_data.get_lock();
    float gyro_error_deg = gyro_data.gz / 131.0;  // Current gyro error
    gyro_data.release_lock();

    // IR Data

    // TODO: alter control logic to handle non straight path lines / edge cases
    float left_perp  = left_IR_sensor.read() * 0.707;
    float right_perp = right_IR_sensor.read() * 0.707;
    float ir_error;

    // Straight Path IR Correction
    // If the measured perpendicular widths are greater than the maimum.
    // At least 1 sensor is looking at a gap on at least 1 side
    if (left_perp + right_perp + IR_PLACEMENT_WIDTH_MM > PATH_SIZE_MM)
    {
        // If the minimum is less than than 1/2 the path width we can use 
        // it to calculate correction, else the robot can not use either and
        // must rely on the gyro to correct movement.
        float min_perp = std::min(left_perp, right_perp);
        if (min_perp + IR_PLACEMENT_WIDTH_MM / 2 > PATH_SIZE_MM / 2) // Ignore IR Correction
        {
            ir_error = 0;
        }
        else if (left_perp > right_perp) // Use right wall
        {
            ir_error = (min_perp - ((PATH_SIZE_MM - IR_PLACEMENT_WIDTH_MM) / 2));
        }
        else // Use left wall
        {
            ir_error = (((PATH_SIZE_MM - IR_PLACEMENT_WIDTH_MM) / 2) - min_perp);
        }
    }
    else
    {
        ir_error = (left_perp - right_perp) / 2;
    }

    // Combine both errors (weighted sum)
    float error = (gyro_error_deg * GYRO_CORRECTION_WEIGHT) + (ir_error * SENSOR_CORRECTION_WEIGHT); 

    // Deadband
    if (std::fabs(error) < CORRECTION_DEADBAND) {
        error = 0;
    }

    // Step 2: Compute PID Terms
    integral += error;                              // Accumulate error (I term)
    // Clamp integral to prevent windup
    float max_integral = 100.0;  // Adjust as needed
    integral = std::clamp(integral, -max_integral, max_integral);

    float derivative = (error - previous_error);    // Rate of change of error (D term)

    // Step 3: Compute Final Correction
    correction = (Kp * error) + (Ki * integral) + (Kd * derivative);
    
    // Step 4: Save Previous Error for Next Iteration
    previous_error = error;

    // Step 5: Clamp to scale useable by motor
    // Convert mm-based correction into a usable motor speed adjustment (0 to 1 scale)
    const float max_error_mm = (PATH_SIZE_MM) / 2 - IR_PLACEMENT_WIDTH_MM; // Assume max error is half the path width - width of sensors
    float correction_speed = std::clamp(correction / max_error_mm, -1.0f, 1.0f);
    return correction_speed;
}

// Set motor speed given a (-1:1) input
void set_motor_speed(uint8_t MOTOR_ID, float speed)
{
    // Clamp input to the range [-1, 1]
    speed = std::clamp(speed, -1.0f, 1.0f);

    float duty1;
    float duty2;
    if (speed < 0) 
    {
        // Backward movement
        duty1 = std::abs(speed);
        duty2 = 0;
    } 
    else if (speed > 0) 
    {
        // Forward movement
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

void motor_action_tracking(bool &motor_correction, int64_t total_left_encoder_count, int64_t total_right_encoder_count)
{
    total_left_encoder_count = right_encoder_count_shared.read();
    total_right_encoder_count = left_encoder_count_shared.read();
    if (motor_action_in_progress)
    {       
        if (((total_left_encoder_count >= target_left_motor_rotation_steps && target_left_motor_rotation_steps >= 0) || 
             (total_left_encoder_count <= target_left_motor_rotation_steps && target_left_motor_rotation_steps < 0)) &&
            ((total_right_encoder_count >= target_right_motor_rotation_steps && target_right_motor_rotation_steps >= 0) || 
             (total_right_encoder_count <= target_right_motor_rotation_steps && target_right_motor_rotation_steps < 0)))
        {
            std::cout << "Done: " << total_left_encoder_count << " & " << total_right_encoder_count << std::endl;
            // Disable motors
            set_motor_speed(MOTOR_LEFT, 0);
            set_motor_speed(MOTOR_RIGHT, 0);
            
            // Set flags
            motor_action_in_progress = false;
            motor_action.write(false);
        }

        // Calculate correction if needed
        else if (motor_correction)
        {
            // Find correction
            float correction = compute_correction();

            // Set motor
            float scaler = 0.5;
            set_motor_speed(MOTOR_LEFT, temp_left_motor_set_speed - (correction / temp_left_motor_set_speed) * scaler);
            set_motor_speed(MOTOR_RIGHT, temp_right_motor_set_speed + (correction / temp_right_motor_set_speed) * scaler);

            // Reset flag
            motor_correction = false;
        }  
    }   
    
    // Start Motor if action flag in shared data is high and motor is not already running
    else if (motor_action.read())
    {
        // Store final rotation count
        int64_t left_demand = left_encoder_rotation_demand.read();
        int64_t right_demand = right_encoder_rotation_demand.read();

        target_left_motor_rotation_steps = 100000;//total_left_encoder_count + left_demand;
        target_right_motor_rotation_steps = 100000;//total_right_encoder_count + right_demand;

        // Store set rotation speed factor locally
        temp_left_motor_set_speed = left_motor_set_speed.read();
        temp_right_motor_set_speed = right_motor_set_speed.read();

        // Activate motors
        set_motor_speed(MOTOR_LEFT, temp_left_motor_set_speed);
        set_motor_speed(MOTOR_RIGHT, temp_right_motor_set_speed);
        
        // Set in progress flag
        motor_action_in_progress = true;
    }
}