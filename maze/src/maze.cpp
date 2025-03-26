# include <maze.h>

// Check if a specific wall exists
bool Cell::hasWall(uint8_t wall) const 
{
    return walls & wall;
}

// Add a wall
void Cell::addWall(uint8_t wall) 
{
    walls |= wall;
}

// Remove a wall
void Cell::removeWall(uint8_t wall) 
{
    walls &= ~wall;
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
    // TODO: Complete
    /*
    for (int i = 0; i < MAZE_WIDTH; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            // Reset Maze
        }
    }    
    */
}

// NAVIGATION FUNCTIONS
// Get cell information
/*
const Cell& Maze::getCell(int x, int y) const 
{
    Cell null;
    if (isValidCoordinate(x, y)) {return mazeLayout[x][y];}
    else {return null;}
}
*/

// Check exploration status
bool Maze::isExplored(int x, int y) const 
{
    if (isValidCoordinate(x, y)) {return mazeStatus[x][y];}
    else {return 0;}
}

// MANIPULATION FUNCTIONS
// Update the cell exploration status
void Maze::setCellStatus(int x, int y, Status status) 
{
    if (isValidCoordinate(x, y)) {mazeStatus[x][y] = status;}
}

// Update the cell's walls, does not propogate
void Maze::setCellWalls(int x, int y, uint8_t walls) 
{
    if (isValidCoordinate(x,y)) {mazeLayout[x][y].walls = walls;}
}

// Update walls, propogates
void Maze::updateCellWalls(int x, int y, uint8_t walls) 
{
    if (isValidCoordinate(x,y)) {
       // TODO: COMPLETE
    }
}

// Update exploration status, propogates
void Maze::markAsExplored(int x, int y) 
{
    if (isValidCoordinate(x, y)) 
    {
        // TODO: Complete
    }
}

// UTILITY FUNCTIONS
// Check if coordinates are valid
bool Maze::isValidCoordinate(int x, int y) const 
{  
    // TODO: Finish
    return true;
}

// Return a list of adjacent coordinates
std::vector<std::pair<int, int>> Maze::getAdjacentCells(int x, int y) const 
{
    // TODO: Finish
    std::vector<std::pair<int, int>> cells;
    if(isValidCoordinate(x, y + 1)) {cells.push_back(std::pair<int, int>(x, y + 1));}
    else {cells.push_back(std::pair<int, int>(-1, -1));}
    if(isValidCoordinate(x + 1, y)) {cells.push_back(std::pair<int, int>(x + 1, y));}
    else {cells.push_back(std::pair<int, int>(-1, -1));}
    if(isValidCoordinate(x, y - 1)) {cells.push_back(std::pair<int, int>(x, y - 1));}
    else {cells.push_back(std::pair<int, int>(-1, -1));}
    if(isValidCoordinate(x - 1, y)) {cells.push_back(std::pair<int, int>(x - 1, y));}
    else {cells.push_back(std::pair<int, int>(-1, -1));}
    return cells;
}