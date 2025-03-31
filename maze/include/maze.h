#ifndef MAZE_H
#define MAZE_H
// Defines the maze
// STD Libraries
#include <cstdint>
#include <vector>
#include <queue>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <stack>

// Tolerance
#define MAZE_TOLERANCE 0.05

// Critical Measurements
#define WALL_THICKNESS_MM 12
#define PATH_SIZE_MM 168
#define CELL_SIZE_MM 180

// Cell Structure
#define MAZE_LENGTH 16 // Cells
#define MAZE_WIDTH 16

// 4 bits represent walls (N, E, S, W)
#define WALL_NORTH 0b00001000 // Binary representation
#define WALL_EAST  0b00000100
#define WALL_SOUTH 0b00000010
#define WALL_WEST  0b00000001

// Define Status Flags
#define DONE_FLAG  0b00100000
#define VISIT_FLAG 0b00010000

// Cell structure
struct Cell {
    // Lower nibble stores wall data: (N, E, S, W)
    // Upper nibble stores status data: (X, X, Done, Visited)
    // Done --> Used to solve
    // Visited --> Marks if robot has been to cell
    uint8_t cell_data = 0; // Stores wall information (4 bits)

    // Member functions
    bool hasWall(uint8_t wall) const;
    void addWall(uint8_t wall);
    void removeWall(uint8_t wall);
    void printWalls() const;
};

// Define Maze
class Maze {
    private:

    // Define maze layout with cells
    static Cell mazeLayout[MAZE_WIDTH][MAZE_LENGTH];
    std::pair<int,int> goal = std::pair<int, int> (-1, -1);

    public:
    Maze();
    void resetMaze();
    const Cell* getCell(int x, int y) const;
    bool is_explored(int x, int y) const;
    void set_explored(int x, int y);
    void set_done(int x, int y);
    void set_cell_walls(int x, int y, uint8_t walls);
    void update_cell_walls(int x, int y, uint8_t walls);
    bool is_valid_coordinate(int x, int y) const;
    bool is_goal(int x, int y);
    bool set_goal(int x, int y);
    std::queue<std::pair<int, int>> get_adjacent_cells(int x, int y) const;
    
    // Pathfining
    bool has_wall_in_direction(int x, int y, int dx, int dy) const;
};

// Depth-First Search Class
class DFSPathfinder 
{
    private:
        Maze& maze;
        std::stack<std::pair<int, int>> path_stack;
        bool goal_found = false;
    
    public:
        DFSPathfinder(Maze &m, int start_x, int start_y);
        std::pair<int, int> get_next_move();
};

class StandardRoutePlanner
{
    private:
    Maze& maze;

    struct pair_hash 
    {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const 
        {
            return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
        }
    };

    public:
    StandardRoutePlanner(Maze &m);
    std::queue<std::pair<int,int>> find_route(int start_x, int start_y, int finish_x, int finish_y);

    // TODO: Add complex finfinding for final run

};

#endif // MAZE_H