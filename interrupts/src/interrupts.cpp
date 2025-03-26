#include <interrupts.h>

// Flags
bool ir_sensor_ready_flag_1 = false;
bool ir_sensor_ready_flag_2 = false;
bool ir_sensor_ready_flag_3 = false;
bool gyro_sensor_ready_flag = false;
bool sw1_on_flag = false;
bool sw2_on_flag = false;
bool sw3_on_flag = false;
bool motor_correct_flag = false;

// Timers
struct repeating_timer correction_timer;
struct repeating_timer gyro_timer;

// Encoder PWM data
uint64_t rotation_count[NUM_PWM_PINS] = {0};
uint32_t pwm_period[NUM_PWM_PINS] = {0};
uint32_t last_wrap_time[NUM_PWM_PINS] = {0};
uint pwm_slice_num[NUM_PWM_PINS] = {0}; 

// Interrupt Handlers
void ir_sensor_irq_handler(uint gpio, uint32_t events) 
{ 
    switch(gpio) 
    {
        case S1_GPIO: {sw1_on_flag = true; break;}
        case S2_GPIO: {sw1_on_flag = true; break;}
        case S3_GPIO: {sw1_on_flag = true; break;}
        default:      {}
    }
}

// Gyro sensor
bool gyro_sensor_irq_handler(struct repeating_timer *t) 
{ 
    gyro_sensor_ready_flag = true; 
    return true;
}

// Switch Inturrupt Toggle
void switch_irq_handler(uint gpio, uint32_t events)
{
    if (events = GPIO_IRQ_LEVEL_HIGH) 
    {
        switch(gpio) 
        {
            case SW1: {sw1_on_flag = true; break;}
            case SW2: {sw2_on_flag = true; break;}
            case SW3: {sw3_on_flag = true; break;}
            default:  {}
        }
    }
    else if (events = GPIO_IRQ_LEVEL_LOW)
    {
        switch(gpio) 
        {
            case SW1: {sw1_on_flag = false; break;}
            case SW2: {sw2_on_flag = false; break;}
            case SW3: {sw3_on_flag = false; break;}
            default:  {}
        }
    }
}

// PWM handler
void pwm_irq_handler() 
{
    uint32_t current_time = time_us_32(); // Get current time in microseconds

    for (int i = 0; i < NUM_PWM_PINS; i++) {
        // Ensure the wrap interrupt corresponds to this PWM pin
        if (pwm_get_irq_status_mask() & (1 << pwm_slice_num[i])) {
            // Calculate the period of the PWM signal
            if (last_wrap_time[i] != 0) {
                pwm_period[i] = current_time - last_wrap_time[i];
            }
            last_wrap_time[i] = current_time;

            // Incrment Rotation Count
            rotation_count[i]++;

            pwm_clear_irq(pwm_slice_num[i]);
        }
    }
}

// Encoder PWM recording
void pwm_capture_setup() 
{
    // Initialize PWM on each pin
    for (int i = 0; i < NUM_PWM_PINS; i++) {
        gpio_set_function(PWM_PINS[i], GPIO_FUNC_PWM);  // Set the pin to PWM function
        
        // Get the PWM slice associated with the pin
        pwm_slice_num[i] = pwm_gpio_to_slice_num(PWM_PINS[i]);
        
        // Set PWM frequency and other parameters
        pwm_set_wrap(pwm_slice_num[i], 16384);  // Set the wrap value (this controls the frequency)
        pwm_set_enabled(pwm_slice_num[i], true);  // Enable the PWM
    }

    // Set up PWM interrupts (wrap event) for each slice
    for (int i = 0; i < NUM_PWM_PINS; i++) {
        pwm_set_irq_enabled(pwm_slice_num[i], true);  // Enable wrap interrupts for each slice
    }
    
    // Attach the interrupt handler to the IRQ
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);  // Set the handler for the wrap interrupt
    irq_set_enabled(PWM_IRQ_WRAP, true);
}

// Define/enable inturrupts
void setup_interrupts() {
    // Enable IR sensor inturrupts
    gpio_set_irq_enabled_with_callback(S1_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);
    gpio_set_irq_enabled_with_callback(S2_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);
    gpio_set_irq_enabled_with_callback(S3_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);

    // Enable Gyro Sensor Inturrupts
    add_repeating_timer_ms(GYRO_TIMER_MS, gyro_sensor_irq_handler, nullptr, &gyro_timer);
    add_repeating_timer_ms(CORRECTION_TIMER_MS, correction_irq_handler, NULL, &correction_timer);

    // Enable Switch Inturrupts
    gpio_set_irq_enabled_with_callback(SW1, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_LEVEL_LOW, true, &switch_irq_handler);
    gpio_set_irq_enabled_with_callback(SW2, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_LEVEL_LOW, true, &switch_irq_handler);
    gpio_set_irq_enabled_with_callback(SW3, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_LEVEL_LOW, true, &switch_irq_handler);

    // Setup PWM Inturrupts
    pwm_capture_setup();
}

bool correction_irq_handler(struct repeating_timer *t)
{
    motor_correct_flag = true;
    return true;
}
