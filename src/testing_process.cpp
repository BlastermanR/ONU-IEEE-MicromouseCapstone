#include <testing_process.h>

void wait_test(uint seconds)
{
    printf("Waiting..");
    for (int i = seconds / 1000; i > 0; i--) 
    {
        printf("%i..", i);
        sleep_ms(1000);
    }
    printf("\n");
}

void testing_process()
{
    // Timer To Connect
    wait_test(5000);
    
    Robot robot = Robot();
    
    left_motor_set_speed.write(0.1);
    right_motor_set_speed.write(0.1);

    while(true)
    {
        if(sw1.read())
        {
            sleep_ms(1500);
            // Move Motors
            set_motor_speed(MOTOR_LEFT, 0.5);
            set_motor_speed(MOTOR_RIGHT, 0.5);

            // Wait
            sw1.write(1);
            sleep_ms(2000);
            sw1.write(0);

            set_motor_speed(MOTOR_LEFT, 0);
            set_motor_speed(MOTOR_RIGHT, 0); 

            printf("counts: %i and %i\n", left_encoder_count_shared.read(), right_encoder_count_shared.read());
        }

        if(sw2.read())
        {
            sleep_ms(1500);
            robot.move_forward(100, true, NONE);
        }
    

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