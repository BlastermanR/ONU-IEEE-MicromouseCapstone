#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <platform_config.h>

// Initializes PIO
void init_quadrature_encoders();

// Retrieves up to date encoder line count
void update_encoder_count(int64_t &left_encoder_count, int64_t &right_encoder_count);