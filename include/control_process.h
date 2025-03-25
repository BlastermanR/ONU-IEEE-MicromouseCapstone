#ifndef CONTROL_PROCESSES
#define CONTROL_PROCESSES

// STD Libraries
#include<iostream>

// Pico Libraries
#include <pico/stdlib.h>

// Custom Libraries
#include <platform_config.h>
#include <shared_data.h>
#include <maze.h>
#include <state_machine.h>
#include <robot.h>

// CONTROL PROCESS
// Define the assistant process
void control_process();

// STATE FUNCTIONS
// Init state function
bool init();

// Idle state function
bool idle();

// Explore state function
bool explore();

// Solves Maze
bool solve();

// Executes computed path
bool execute_path();

// Error State
bool error();

#endif