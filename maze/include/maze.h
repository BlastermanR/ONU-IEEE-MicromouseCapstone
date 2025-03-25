#ifndef MAZE_H
#define MAZE_H
// Defines the maze
// STD Libraries
#include <cstdint> // For uint8_t
#include <vector>
#include <iostream>

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
#define WALL_NORTH 0b1000 // Binary representation
#define WALL_EAST  0b0100
#define WALL_SOUTH 0b0010
#define WALL_WEST  0b0001

// Cell structure
struct Cell {
    uint8_t walls = 0; // Stores wall information (4 bits)

    // Member functions
    bool hasWall(uint8_t wall) const;
    void addWall(uint8_t wall);
    void removeWall(uint8_t wall);
    void printWalls() const;
};

// Define Maze
class Maze {
    private:
    // Define maze dimensions

    // Define maze layout with cells
    static Cell mazeLayout[MAZE_WIDTH][MAZE_LENGTH];

    // Define array for explored maze
    enum Status{
        UNKNOWN = 0,
        PARTIAL = 1,
        FULL = 2,
    };
    static Status mazeStatus[MAZE_WIDTH][MAZE_LENGTH];

    Maze();
    void resetMaze();
    const Cell& getCell(int x, int y) const;
    bool isExplored(int x, int y) const;
    void setCellStatus(int x, int y, Status status);
    void setCellWalls(int x, int y, uint8_t walls);
    void updateCellWalls(int x, int y, uint8_t walls);
    void markAsExplored(int x, int y);
    bool isValidCoordinate(int x, int y) const;
    std::vector<std::pair<int, int>> getAdjacentCells(int x, int y) const;
};

#endif // MAZE_H