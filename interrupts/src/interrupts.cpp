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
bool encoder_read_flag = false;

// Timers
struct repeating_timer correction_timer;
struct repeating_timer gyro_timer;
struct repeating_timer encoder_timer;

// Encoder PWM data
int64_t rotation_count[2] = {0};

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
    if (events == GPIO_IRQ_EDGE_RISE) 
    {
        switch(gpio) 
        {
            case SW1: {sw1_on_flag = true; break;}
            case SW2: {sw2_on_flag = true; break;}
            case SW3: {sw3_on_flag = true; break;}
            default:  {}
        }
    }
    else if (events == GPIO_IRQ_EDGE_FALL)
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

// Define/enable inturrupts
void setup_interrupts() {
    // Enable IR sensor inturrupts
    gpio_set_irq_enabled_with_callback(S1_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);
    gpio_set_irq_enabled_with_callback(S2_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);
    gpio_set_irq_enabled_with_callback(S3_GPIO, GPIO_IRQ_LEVEL_LOW , true, &ir_sensor_irq_handler);

    // Enable Gyro Sensor Inturrupts
    add_repeating_timer_ms(GYRO_TIMER_MS, gyro_sensor_irq_handler, nullptr, &gyro_timer);
    add_repeating_timer_ms(ENCODER_TIMER_MS, encoder_irq_handler, nullptr, &encoder_timer);
    
    // Enable Switch Inturrupts
    gpio_set_irq_enabled_with_callback(SW1, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &switch_irq_handler);
    gpio_set_irq_enabled_with_callback(SW2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &switch_irq_handler);
    gpio_set_irq_enabled_with_callback(SW3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &switch_irq_handler);

    // Setup PWM Inturrupts
    init_quadrature_encoders();
}

// Determine if correction timer should run
void set_correction_timer(bool run_timer)
{
    static bool timer_running = false;
    if (run_timer && !timer_running)
    {
        add_repeating_timer_ms(CORRECTION_TIMER_MS, correction_irq_handler, NULL, &correction_timer);
        timer_running = true;
    }
    else if (timer_running)
    {
        cancel_repeating_timer(&correction_timer);
        timer_running = false;
    }
}

// Set correction flag in motor control
bool correction_irq_handler(struct repeating_timer *t)
{
    motor_correct_flag = true;
    return true;
}

bool encoder_irq_handler(struct repeating_timer *t)
{
    encoder_read_flag = true;
    return true;
}