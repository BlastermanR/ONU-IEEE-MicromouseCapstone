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
    for (int i = 0; i < MAZE_WIDTH; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            setCellWalls(i, j, 0b0000);
            setCellStatus(i, j, UNKNOWN);
        }
    }
}

// NAVIGATION FUNCTIONS
// Get cell information
const Cell* Maze::getCell(int x, int y) const 
{
    if (isValidCoordinate(x, y)) {
        return &mazeLayout[x][y];
    } else {
        return nullptr;
    }
}

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
    if (isValidCoordinate(x, y) && mazeStatus[x][y] < status) {mazeStatus[x][y] = status;}
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
        mazeLayout[x][y].addWall(walls);
        if (walls |= 0b1000 && isValidCoordinate(x, y + 1)) {mazeLayout[x][y + 1].addWall(0b1000);}
        if (walls |= 0b0100 && isValidCoordinate(x + 1, y)) {mazeLayout[x + 1][y].addWall(0b0100);}
        if (walls |= 0b0010 && isValidCoordinate(x, y - 1)) {mazeLayout[x][y - 1].addWall(0b0010);}
        if (walls |= 0b0001 && isValidCoordinate(x - 1, y)) {mazeLayout[x - 1][y].addWall(0b0001);}
    }
}

// Update exploration status, propogates
void Maze::markAsExplored(int x, int y) 
{
    Maze::setCellStatus(x, y, KNOWN);

    std::vector<std::pair<int, int>> adj_cells = getAdjacentCells(x, y);
    while (!adj_cells.empty())
    {
        std::pair<int, int> cell = adj_cells.front();
        setCellStatus(cell.first, cell.second, PARTIAL);
        adj_cells.pop_back();
    }
}

// UTILITY FUNCTIONS
// Check if coordinates are valid
bool Maze::isValidCoordinate(int x, int y) const 
{  
    if (x >= 0 && x < MAZE_WIDTH && y >= 0 && y < MAZE_LENGTH) {return true;}
    else {return false;}
}

// Return a list of adjacent coordinates
std::vector<std::pair<int, int>> Maze::getAdjacentCells(int x, int y) const 
{
    // TODO: Finish
    std::vector<std::pair<int, int>> cells;
    if(isValidCoordinate(x, y + 1)) {cells.push_back(std::pair<int, int>(x, y + 1));}
    if(isValidCoordinate(x + 1, y)) {cells.push_back(std::pair<int, int>(x + 1, y));}
    if(isValidCoordinate(x, y - 1)) {cells.push_back(std::pair<int, int>(x, y - 1));}
    if(isValidCoordinate(x - 1, y)) {cells.push_back(std::pair<int, int>(x - 1, y));}
    return cells;
}