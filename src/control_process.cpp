#include <control_process.h>
// Define globals

// Process handles non-real time, complex, control operations (pathfinding/debugging, control)
void control_process() 
{
    printf("CORE 2: Starting Setup\n");

    printf("CORE 2: Begining Loop\n");
    while (!control_process_exit_signal.read()) 
    {
        // Update State
        current_control_state = next_control_state;
        printf("CORE 2: Entering State: %s\n", current_control_state);
        // Track Status
        int status;
        // Execute
        switch (current_control_state) {
            case MicromouseControlState::INIT:          {status = init(); break;}
            case MicromouseControlState::IDLE:          {status = idle(); break;}
            case MicromouseControlState::EXPLORE:       {status = explore(); break;}
            case MicromouseControlState::SOLVE:         {status = solve(); break;}
            case MicromouseControlState::EXECUTE_PATH:  {status = execute_path(); break;}
            case MicromouseControlState::ERROR:         {status = error(); break;}
        }
    }

    return;
}

bool init()
{
    // TODO:
    return true;
};

bool idle()
{
    // TODO:
    return true;
}

bool explore()
{
    // TODO:
    return true;
}

bool solve()
{
    // TODO:
    return true;
}

bool execute_path()
{
    // TODO:
    return true;
}

bool error()
{
    // TODO:
    return true;
}