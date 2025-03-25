#include <state_machine.h>

// State Definitions
MicromouseControlState current_control_state = MicromouseControlState::INIT;
MicromouseControlState next_control_state = MicromouseControlState::INIT;