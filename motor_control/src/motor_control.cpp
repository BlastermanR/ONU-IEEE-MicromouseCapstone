#include<motor_control.h>

// Local global variables for motor actions
bool motor_action_in_progress = false;
volatile int64_t target_left_motor_rotation_steps = 0;
volatile int64_t target_right_motor_rotation_steps = 0;
float temp_left_motor_set_speed = 0;
float temp_right_motor_set_speed = 0;
bool turning = false;

// Calculate correction to ensure robot travels on straight path
float compute_correction()
{
    const float Kp_c =  1.0; // Proportional gain
    const float Ki_c = 0.1;  // Integral gain
    const float Kd_c = 0.05; // Derivative gain
        
    // PID variables
    static float previous_error = 0.0;
    static float integral = 0.0;

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
    correction = (Kp_c * error) + (Ki_c * integral) + (Kd_c * derivative);
    
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

    // Get Slices
    uint AIN1_SLICE = pwm_gpio_to_slice_num(AIN1);
    uint AIN2_SLICE = pwm_gpio_to_slice_num(AIN2);
    uint BIN1_SLICE = pwm_gpio_to_slice_num(BIN1);
    uint BIN2_SLICE = pwm_gpio_to_slice_num(BIN2);

    if (speed == 0)
    {
        // Disable PWM and Enable Braking
        switch(MOTOR_ID)
        {
            case MOTOR_LEFT: 
            {
                pwm_set_enabled(AIN1_SLICE, false);
                pwm_set_enabled(AIN2_SLICE, false);
                gpio_set_function(AIN1, GPIO_FUNC_SIO);
                gpio_set_function(AIN2, GPIO_FUNC_SIO);
                gpio_put(AIN1, 1);
                gpio_put(AIN2, 1);
                break;
            }
            case MOTOR_RIGHT:
            {
                pwm_set_enabled(BIN1_SLICE, false);
                pwm_set_enabled(BIN2_SLICE, false);
                gpio_set_function(BIN1, GPIO_FUNC_SIO);
                gpio_set_function(BIN2, GPIO_FUNC_SIO);
                gpio_put(BIN1, 1);
                gpio_put(BIN2, 1);
                break;
            }
            default: {}
        };
    }    
    else
    {
        // Set Duty Cycle
        float duty1 = (speed < 0) ? std::abs(speed) : 0;
        float duty2 = (speed > 0) ? std::abs(speed) : 0;

        // Enable / Set PWM
        switch(MOTOR_ID)
        {
            case MOTOR_LEFT: 
            {
                gpio_set_function(AIN1, GPIO_FUNC_PWM);
                gpio_set_function(AIN2, GPIO_FUNC_PWM);
                pwm_set_enabled(AIN1_SLICE, true);
                pwm_set_enabled(AIN2_SLICE, true);
                pwm_set_chan_level(AIN1_SLICE, pwm_gpio_to_channel(AIN1), duty1 * PWM_CLOCK_TOP);
                pwm_set_chan_level(AIN2_SLICE, pwm_gpio_to_channel(AIN2), duty2 * PWM_CLOCK_TOP);
                break;
            }
            case MOTOR_RIGHT:
            {
                gpio_set_function(BIN1, GPIO_FUNC_PWM);
                gpio_set_function(BIN2, GPIO_FUNC_PWM);
                pwm_set_enabled(BIN1_SLICE, true);
                pwm_set_enabled(BIN2_SLICE, true);
                pwm_set_chan_level(BIN1_SLICE, pwm_gpio_to_channel(BIN1), duty1 * PWM_CLOCK_TOP);
                pwm_set_chan_level(BIN2_SLICE, pwm_gpio_to_channel(BIN2), duty2 * PWM_CLOCK_TOP);
                break;
            }
            default: {}
        };
    }
}

// Velocity Update
void update_velocity()
{
    static int64_t old_left_count = 0;
    static int64_t old_right_count = 0;
    int64_t left_count = left_encoder_count_shared.read();
    int64_t right_count = right_encoder_count_shared.read();

    // Calculate the difference in encoder counts since the last update
    int64_t left_diff = left_count - old_left_count;
    int64_t right_diff = right_count - old_right_count;

    // Convert encoder count difference to velocity in mm/s
    // Formula: velocity (mm/s) = (avg_diff * WHEEL_CIRCUMFERENCE_MM * 1000) / (ENCODER_NUM_LINES_ROTATION * VELOCITY_TIMER_MS)
    float velocity_left = (float)(left_diff * WHEEL_CIRCUMFERENCE_MM * 1000) / (ENCODER_NUM_LINES_ROTATION * VELOCITY_TIMER_MS);
    float velocity_right = (float)(right_diff * WHEEL_CIRCUMFERENCE_MM * 1000) / (ENCODER_NUM_LINES_ROTATION * VELOCITY_TIMER_MS);

    current_velocity_left_mms_shared.write(velocity_left);
    current_velocity_right_mms_shared.write(velocity_right);

    old_left_count = left_count;
    old_right_count = right_count;
}

// Tracks Motor Speed
float speed_pid(uint8_t motor_id, bool reset)
{
    // PID constants (tune these values)
    static constexpr float Kp_v = 2.0f;  // Proportional gain
    static constexpr float Ki_v = 0.2f;  // Integral gain
    static constexpr float Kd_v = 0.05f; // Derivative gain

    // Static variables for maintaining state across function calls
    static float previous_error_left = 0.0f;
    static float integral_left = 0.0f;
    static float previous_error_right = 0.0f;
    static float integral_right = 0.0f;

    if (reset)
    {
        if (motor_id == MOTOR_LEFT)
        {
            previous_error_left = 0.0f;
            integral_left = 0.0f;
        }
        else
        {
            previous_error_right = 0.0f;
            integral_right = 0.0f; 
        } 
    }

    // Get current & target velocity for the specific motor (left or right)
    float current_velocity_mms = 0.0f;
    float target_velocity_mms = 0.0f;
    if (motor_id == 0) // Left motor
    {
        current_velocity_mms = current_velocity_left_mms_shared.read();
        target_velocity_mms = target_velocity_left_mms_shared.read();
    }
    else if (motor_id == 1) // Right motor
    {
        current_velocity_mms = current_velocity_right_mms_shared.read();
        target_velocity_mms = target_velocity_right_mms_shared.read();
    }

    // Calculate error (difference between target and current speed)
    float error = target_velocity_mms - current_velocity_mms;

    // Proportional term
    float P_term = Kp_v * error;

    // Integral term (accumulates error over time)
    constexpr float INTEGRAL_LIMIT = 100.0f; // Adjust based on system behavior
    if (motor_id == 0) // Left motor
    {
        integral_left += error;
        // Prevent integral windup (clamp to prevent excessive accumulation)
        integral_left = std::clamp(integral_left, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    }
    else // Right motor
    {
        integral_right += error;
        // Prevent integral windup (clamp to prevent excessive accumulation)
        integral_right = std::clamp(integral_right, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    }

    float I_term = Ki_v * (motor_id == 0 ? integral_left : integral_right);

    // Derivative term (rate of error change)
    float D_term = Kd_v * (error - (motor_id == 0 ? previous_error_left : previous_error_right));

    // Compute the PID output
    float output = P_term + I_term + D_term;

    // Update previous error
    if (motor_id == 0)
    {
        previous_error_left = error;
    }
    else
    {
        previous_error_right = error;
    }

    // Return the PID output, which will be used to adjust the motor PWM
    return output;
}

// Tracks Motor Movements
void motor_action_tracking(bool &motor_correction)
{
    int64_t total_left_encoder_count = left_encoder_count_shared.read();
    int64_t total_right_encoder_count = right_encoder_count_shared.read();

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
        else
        {
            // Smooth Acceleration / Deceleration
            // Read current target/desired velocity (mm/s)
            float target_velocity_left = target_velocity_left_mms_shared.read();
            float target_velocity_right = target_velocity_right_mms_shared.read();
            float desired_velocity_left = desired_velocity_left_mms_shared.read();
            float desired_velocity_right = desired_velocity_right_mms_shared.read();   

            // Check distance remaining
            int64_t left_distance_remaining = abs(target_left_motor_rotation_steps - total_left_encoder_count);
            int64_t right_distance_remaining = abs(target_right_motor_rotation_steps - total_right_encoder_count);
            int64_t min_distance_remaining = std::min(left_distance_remaining, right_distance_remaining);

            // Calculate Deceleration Threshold
            float deceleration_Threshold_mm = (target_velocity_left * target_velocity_left) / (2 * ACCELERATION_STEP);

            // Acceleration phase: Increase speed until desired velocity is reached
            if (target_velocity_left < desired_velocity_left && min_distance_remaining > deceleration_Threshold_mm) 
            {
                target_velocity_left = std::min(target_velocity_left + ACCELERATION_STEP, desired_velocity_left);
            }
            else if (min_distance_remaining <= deceleration_Threshold_mm) 
            {
                float decel_factor = static_cast<float>(min_distance_remaining) / deceleration_Threshold_mm;
                target_velocity_left = std::max(target_velocity_left * decel_factor, 5.0f);
            }

            if (target_velocity_right < desired_velocity_right && min_distance_remaining > deceleration_Threshold_mm) 
            {
                target_velocity_right = std::min(target_velocity_right + ACCELERATION_STEP, desired_velocity_right);
            }
            else if (min_distance_remaining <= deceleration_Threshold_mm) 
            {
                float decel_factor = static_cast<float>(min_distance_remaining) / deceleration_Threshold_mm;
                target_velocity_right = std::max(target_velocity_right * decel_factor, 5.0f);
            }

            target_velocity_left_mms_shared.write(target_velocity_left);
            target_velocity_right_mms_shared.write(target_velocity_right);

            // Compute PID control output for velocity for each motor independently
            float left_pwm = speed_pid(MOTOR_LEFT, false);
            float right_pwm = speed_pid(MOTOR_RIGHT, false);

            // Calculate path correction if needed
            if (motor_correction)
            {
                // Find correction
                float correction = compute_correction();
                float correction_scaler = 0.5;
                left_pwm -= (correction / temp_left_motor_set_speed) * correction_scaler;
                right_pwm += (correction / temp_right_motor_set_speed) * correction_scaler;
                motor_correction = false;
            }  

            // Apply final adjusted speeds
            set_motor_speed(MOTOR_LEFT, left_pwm);
            set_motor_speed(MOTOR_RIGHT, right_pwm);
        }
    }   
    
    // Start Motor if action flag in shared data is high and motor is not already running
    else if (motor_action.read())
    {
        // Store final rotation count
        int64_t left_demand = left_encoder_rotation_demand.read();
        int64_t right_demand = right_encoder_rotation_demand.read();

        target_left_motor_rotation_steps = total_left_encoder_count + left_demand;
        target_right_motor_rotation_steps = total_right_encoder_count + right_demand;

        // Set target speed to 0
        target_velocity_left_mms_shared.write(0);
        target_velocity_right_mms_shared.write(0);

        // Compute initial motor speed using PID
        float left_motor_pwm = speed_pid(MOTOR_LEFT, true);
        float right_motor_pwm = speed_pid(MOTOR_RIGHT, false);

        // Activate motors with PID-controlled speed
        set_motor_speed(MOTOR_LEFT, left_motor_pwm);
        set_motor_speed(MOTOR_RIGHT, right_motor_pwm);
        
        // Set in progress flag
        motor_action_in_progress = true;
    }
}