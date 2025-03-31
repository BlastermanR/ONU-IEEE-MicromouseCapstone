# include <maze.h>

// Check if a specific wall exists
bool Cell::hasWall(uint8_t wall) const 
{
    return cell_data & wall;
}

// Add a wall
void Cell::addWall(uint8_t wall) 
{
    cell_data |= (wall & 0x0F);
}

// Remove a wall
void Cell::removeWall(uint8_t wall) 
{
    cell_data &= ((~wall) | 0xF0);
}

// Print walls for debugging
void Cell::printWalls() const 
{
    std::cout << "North: " << hasWall(WALL_NORTH)
              << ", East: " << hasWall(WALL_EAST)
              << ", South: " << hasWall(WALL_SOUTH)
              << ", West: " << hasWall(WALL_WEST) << "\n";
}

// INITIALIZATION FUNCTIONS
// Initialize the maze
Maze::Maze() 
{
    resetMaze();
    // TODO: Finish if needed
}

// Reset maze data
void Maze::resetMaze() 
{
    for (int i = 0; i < MAZE_WIDTH; i++) {
        for (int j = 0; j < MAZE_LENGTH; j++) {
            // Set Edges
            uint8_t walls = 0x00;
            if (i == 0)               {walls |= WALL_WEST; }
            if (i == MAZE_WIDTH - 1)  {walls |= WALL_EAST; }
            if (j == 0)               {walls |= WALL_SOUTH;}
            if (j == MAZE_LENGTH - 1) {walls |= WALL_NORTH;}

            set_cell_walls(i, j, walls);
        }
    }
}

// NAVIGATION FUNCTIONS
// Get cell information
const Cell* Maze::getCell(int x, int y) const 
{
    if (is_valid_coordinate(x, y)) {
        return &mazeLayout[x][y];
    } else {
        return nullptr;
    }
}

// MANIPULATION FUNCTIONS
// Update the cell exploration status
void Maze::set_explored(int x, int y) 
{
    if (is_valid_coordinate(x, y)) {mazeLayout[x][y].cell_data |= VISIT_FLAG;}
}

// Update the pathfinding status
void Maze::set_done(int x, int y) 
{
    if (is_valid_coordinate(x, y)) {mazeLayout[x][y].cell_data |= DONE_FLAG;}
}

// Update the cell's walls, does not propogate
void Maze::set_cell_walls(int x, int y, uint8_t walls) 
{
    if (is_valid_coordinate(x,y)) {mazeLayout[x][y].cell_data |= (walls & 0x0F);}
}

// Update walls, propogates
void Maze::update_cell_walls(int x, int y, uint8_t walls) 
{
    if (is_valid_coordinate(x,y)) {
        mazeLayout[x][y].addWall(walls);
        if (walls & WALL_NORTH && is_valid_coordinate(x, y + 1)) {mazeLayout[x][y + 1].addWall(WALL_NORTH);}
        if (walls & WALL_EAST  && is_valid_coordinate(x + 1, y)) {mazeLayout[x + 1][y].addWall(WALL_EAST );}
        if (walls & WALL_SOUTH && is_valid_coordinate(x, y - 1)) {mazeLayout[x][y - 1].addWall(WALL_SOUTH);}
        if (walls & WALL_WEST  && is_valid_coordinate(x - 1, y)) {mazeLayout[x - 1][y].addWall(WALL_WEST );}
    }
}

// UTILITY FUNCTIONS
// Check if coordinates are valid
bool Maze::is_valid_coordinate(int x, int y) const 
{  
    if (x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_LENGTH) {return true;}
    else {return false;}
}

// Return a list of adjacent coordinates
std::queue<std::pair<int, int>> Maze::get_adjacent_cells(int x, int y) const 
{
    std::queue<std::pair<int, int>> cells;
    if(is_valid_coordinate(x, y + 1)) {cells.push(std::pair<int, int>(x, y + 1));}
    if(is_valid_coordinate(x + 1, y)) {cells.push(std::pair<int, int>(x + 1, y));}
    if(is_valid_coordinate(x, y - 1)) {cells.push(std::pair<int, int>(x, y - 1));}
    if(is_valid_coordinate(x - 1, y)) {cells.push(std::pair<int, int>(x - 1, y));}
    return cells;
}

// Is there a wall that way
bool Maze::has_wall_in_direction(int x, int y, int dx, int dy) const 
{
    uint8_t walls = mazeLayout[x][y].cell_data;
    if (dx == -1 && dy == 0) return (walls & WALL_NORTH); // North
    if (dx == 1 && dy == 0) return (walls & WALL_SOUTH); // South
    if (dx == 0 && dy == -1) return (walls & WALL_WEST);  // West
    if (dx == 0 && dy == 1) return (walls & WALL_EAST);  // East
    return false;
}

// Returns if coordinate is goal or not, sets goal var for future refrence
bool Maze::is_goal(int x, int y)
{
    // Center of maze is 4 squares in middle this 1/2 width/length plus minus 1 index on either side (16 x 16 is 7, 8)
    if ((x == (MAZE_WIDTH / 2 - 1) || x == (MAZE_WIDTH / 2)) && (y == (MAZE_LENGTH / 2 - 1) || y == (MAZE_LENGTH / 2)))
    {
        goal.first = x;
        goal.second = y;
        return true;
    }
    return false;
}

// Returns true if goal is set successfully 
bool Maze::set_goal(int x, int y)
{
    if(is_valid_coordinate(x, y))
    {
        goal.first = x;
        goal.second = y;
        return true;
    }
    return false;
}

// PATHFINDING
DFSPathfinder::DFSPathfinder(Maze& m, int start_x, int start_y) : maze(m)
{
    maze = m;
    // Empty stack
    while(!path_stack.empty()) {path_stack.pop();}

    // Mark Start as Explored
    maze.set_explored(start_x,start_y);
    path_stack.push(std::pair<int, int>(start_x, start_y));
}

// Used on first blind run to explore maze starting with closest possible locations to check movements. 
// Each call of the function returns the next cell the robot should explore
std::pair<int, int> DFSPathfinder::get_next_move() 
{
    // If empty, return invalid
    if (path_stack.empty()) {return {-1, -1};}
    
    // Pop current cell
    std::pair<int, int> location = path_stack.top();
    int x = location.first, y =location.second;

    // If goal is not found and goal is goal?
    if (!goal_found && maze.is_goal(x, y)) {goal_found = true;}

    // List directions
    std::pair<int, int> directions[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    // Check every direction from location
    for (auto [dx, dy] : directions) 
    {
        int nx = x + dx, ny = y + dy;

        if (maze.is_valid_coordinate(nx, ny) && !maze.is_explored(nx,ny) && !maze.has_wall_in_direction(x, y, nx, ny)) 
        {
            path_stack.push({nx, ny});
            maze.set_explored(nx, ny);
        }
    }

    // Pop current cell and return next location to check
    path_stack.pop();
    return {x, y};
}

// Set maze ref
StandardRoutePlanner::StandardRoutePlanner(Maze &m) : maze(m) {maze = m;}

// Return a queue of points for the robot to traverse on its way to the destination point
std::queue<std::pair<int,int>> StandardRoutePlanner::find_route(int start_x, int start_y, int finish_x, int finish_y)
{
    std::queue<std::pair<int, int>> path;
    std::queue<std::pair<int, int>> frontier;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> came_from;
    
    frontier.push({start_x, start_y});
    came_from[{start_x, start_y}] = {-1, -1};

    while (!frontier.empty()) 
    {
        auto [x, y] = frontier.front();
        frontier.pop();

        if (x == finish_x && y == finish_y) 
        {
            // Reconstruct the path
            std::pair<int, int> current = {x, y};
            while (current != std::make_pair(-1, -1)) 
            {
                path.push(current);
                current = came_from[current];
            }
            return path;
        }

        std::pair<int, int> directions[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

        for (auto [dx, dy] : directions) 
        {
            int nx = x + dx, ny = y + dy;
            if (maze.is_valid_coordinate(nx, ny) && came_from.find({nx, ny}) == came_from.end() && !maze.has_wall_in_direction(x, y, nx, ny)) 
            {
                frontier.push({nx, ny});
                came_from[{nx, ny}] = {x, y};
            }
        }
    }

    return path;
}

