#include <quadrature_encoder.pio.h>
#include <quadrature_encoder.h>
PIO pio_left = pio0;
PIO pio_right = pio1;
const uint sm_left = 0;
const uint sm_right = 0;
int32_t last_left_encoder_count = 0;
int32_t last_right_encoder_count = 0;


// Create a function to iitialize PIO for quadrature decoding
void init_quadrature_encoders()
{
    int offset;
    offset = pio_add_program(pio_left, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_left, sm_left, CBL_LOGIC, 0);

    if (offset == -1) {
        printf("Failed to load left PIO program!\n");
    }

    offset = pio_add_program(pio_right, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_right, sm_right, CBR_LOGIC, 0);

    if (offset == -1) {
        printf("Failed to load left PIO program!\n");
    }
}

void update_encoder_count(int64_t &left_encoder_count, int64_t &right_encoder_count) 
{
    int32_t new_left_count = quadrature_encoder_get_count(pio_left, sm_left);
    int32_t new_right_count = -quadrature_encoder_get_count(pio_right, sm_right);

    //printf("counts: %i and %i\n", new_left_count, new_right_count);

    // Left_Motor
    // Detect overflow or underflow
    int32_t diff = new_left_count - last_left_encoder_count;

    if (diff > (1 << 30)) {
        // Overflow: new_count jumped from small to large (e.g., 0x7FFFFFFF to 0x80000000)
        left_encoder_count += (1LL << 32);  // Add 2^32
    } else if (diff < -(1 << 30)) {
        // Underflow: new_count jumped from large to small (e.g., 0x80000000 to 0x7FFFFFFF)
        left_encoder_count -= (1LL << 32);  // Subtract 2^32
    }

    // Update lower 32 bits
    left_encoder_count = (left_encoder_count & ~0xFFFFFFFFULL) | (uint32_t)new_left_count;
    last_left_encoder_count = new_left_count;

    // Right Motor
    // Detect overflow or underflow
    diff = new_right_count - last_right_encoder_count;
    if (diff > (1 << 30)) {
        // Overflow: new_count jumped from small to large (e.g., 0x7FFFFFFF to 0x80000000)
        right_encoder_count += (1LL << 32);  // Add 2^32
    } else if (diff < -(1 << 30)) {
        // Underflow: new_count jumped from large to small (e.g., 0x80000000 to 0x7FFFFFFF)
        right_encoder_count -= (1LL << 32);  // Subtract 2^32
    }

    // Update lower 32 bits
    right_encoder_count = (right_encoder_count & ~0xFFFFFFFFULL) | (uint32_t)new_right_count;
    last_right_encoder_count = new_right_count;
}
