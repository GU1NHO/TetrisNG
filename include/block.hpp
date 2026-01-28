/**
 * @file block.hpp
 * @brief Definition of the base Block class.
 * Represents a Tetris piece (Tetromino), handling its shape, rotation, 
 * movement, and rendering.
 */

#pragma once
#include <vector>
#include <map>
#include "position.hpp"
#include "colors.hpp"

class Block {
public:
    Block();

    // unique identifier for the block type (determines color)
    int id; 

    // Stores the block's shape for each of the 4 rotation states (0, 90, 180, 270 degrees)
    // Key: Rotation state (0-3), Value: List of cell positions relative to the block's pivot
    std::map<int, std::vector<Position>> cells; 

    // Renders the block on the screen.
    void Draw(int offsetX, int offsetY, int dynamicCellSize, Color tint = {0, 0, 0, 0});   
    
    // Updates the block's position in the grid.
    void Move(int rows, int columns);
    
    // Returns the absolute grid positions of the block's cells based on current rotation and offset.
    std::vector<Position> GetCellPositions();
    
    // Rotates the block 90 degrees clockwise.
    void Rotate();
    
    // Reverts the last rotation (used when a rotation causes a collision).
    void UndoRotation();

    // Currently unused, potential leftover for storing previous state
    std::vector<Position> movedTiles;

private:
    int cellSize; 
    int rotationState; 
    std::vector<Color> colors;
    int rowOffset;
    int columnOffset;
};