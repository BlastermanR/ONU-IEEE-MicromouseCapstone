#ifndef ROBOT_H
#define ROBOT_H

// Custom Libraries
#include <platform_config.h>
#include <shared_data.h>
#include <state_machine.h>
#include <maze.h>

// Define Enums
enum TurnTypes
{
    FRONT_RIGHT = 45,
    RIGHT = 90,
    BACK_RIGHT = 135,
    BACK = 180,
    BACK_LEFT = 225,
    TURN_LEFT = 270,
    FRONT_LEFT = 315
};

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
    void move_forward(float distance_mm, CorrectionTypes correction, bool block);

    // Function used to turn the robot clockwise
    // Specifies if the function is blocking
    void turn(TurnTypes direction, bool block);

    // Updates Sensor/Maze data for estimated X,Y, and theta
    void calibrate_position();
};

#endif