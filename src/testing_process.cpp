#include <testing_process.h>

void testing_process()
{
    Robot robot = Robot();
    
    left_motor_set_speed.write(0.1);
    right_motor_set_speed.write(0.1);

    bool run = false;

    while(true)
    {
        if(sw1.read())
        {
            sleep_ms(1500);
            printf("counts_Bef: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
            robot.move_forward(200, true, NONE);
            run = true;
            printf("counts_Aft: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
            sleep_ms(1500);
        }

        if(sw2.read())
        {
            sleep_ms(1500);
            printf("counts_Bef: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
            robot.move_forward(1000, true, NONE);
            run = true;
            printf("counts_Aft: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
            sleep_ms(1500);
        }
    
        printf("counts: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
        sleep_ms(1000);
    }
 

    /*
    // Distance Testing
    sw1.write(1);
    wait_test(7000);
    sw1.write(0);

    left_motor_set_speed.write(.2);
    right_motor_set_speed.write(.2);

    printf("Simple movement test:\n");

    Robot robot;
    robot.move_forward(200, true, NONE);

    sw1.write(1);
    wait_test(2000);
    sw1.write(0);
    printf("Correction movement test:\n");

    robot.move_forward(400, true, STRAIGHT);

    return;
    */
}