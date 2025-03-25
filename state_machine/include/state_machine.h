#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// STD Libraries
#include <string_view>

enum class MicromouseControlState {
    INIT,           // Initial setup and calibration
    IDLE,           // Waiting for a start signal
    EXPLORE,        // Maze exploration phase
    SOLVE,          // Pathfinding and solving
    EXECUTE_PATH,   // Running the optimal path
    ERROR           // Handling any unexpected issues
};

// Define current state variable
extern MicromouseControlState current_control_state;
extern MicromouseControlState next_control_state;

// Converts MicromouseControlState to a string
constexpr std::string_view stateToString(MicromouseControlState state) {
    switch (state) {
        case MicromouseControlState::INIT: return "INIT";
        case MicromouseControlState::IDLE: return "IDLE";
        case MicromouseControlState::EXPLORE: return "EXPLORE";
        case MicromouseControlState::SOLVE: return "SOLVE";
        case MicromouseControlState::EXECUTE_PATH: return "EXECUTE_PATH";
        case MicromouseControlState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

#endif