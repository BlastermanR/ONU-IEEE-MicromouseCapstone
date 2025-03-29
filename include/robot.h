#ifndef ROBOT_H
#define ROBOT_H

// STD Libraries
#include <cmath>

// PICO SDK
#include <pico/stdlib.h>

// Custom Libraries
#include <platform_config.h>
#include <shared_data.h>
#include <state_machine.h>
#include <maze.h>

enum CorrectionTypes 
{
    NONE,
    STRAIGHT,
    DIAGONAL
};

// Struct containing important robot data
struct Robot 
{
    // Estimated robot position
    int estimated_x_mm, estimated_y_mm;

    // Rotation
    int theta_actual = 0;

    Robot();

    // Function used to move the robot forward a given ammount of milimeters
    // Specifies the type of correction to be applied and whether the function is blocking
    void move_forward(float distance_mm, bool block, CorrectionTypes correction = STRAIGHT);

    // Function used to turn the robot clockwise
    // Specifies if the function is blocking
    void turn(int direction_cw, bool block);

    // Updates Sensor/Maze data for estimated X,Y, and theta
    void calibrate_position();
};

#endif