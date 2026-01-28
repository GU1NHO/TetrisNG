/**
 * @file block.cpp
 * @brief Implementation of the Block class logic.
 */

#include "../include/block.hpp"

Block::Block() { 
    rotationState = 0;
    colors = GetCellColors();
    rowOffset = 0;
    columnOffset = 0;
}

void Block::Draw(int offsetX, int offsetY, int dynamicCellSize, Color tint) {
    std::vector<Position> tiles = GetCellPositions();
    
    for (Position item : tiles) {
        //Retrieve the block's original color (e.g., Red for Z-Block)
        Color finalColor = colors[id];

        // Apply alpha transparency if a tint is provided (e.g., for Ghost Piece)
        if (tint.a > 0) {
            finalColor.a = tint.a;
        }

        DrawRectangle(item.column * dynamicCellSize + offsetX, 
                      item.row * dynamicCellSize + offsetY, 
                      dynamicCellSize - 1, 
                      dynamicCellSize - 1, 
                      finalColor);
    }
}

void Block::Move(int rows, int columns) {
    rowOffset += rows;
    columnOffset += columns;
}

std::vector<Position> Block::GetCellPositions() {
    // Get local positions for the current rotation state
    std::vector<Position> tiles = cells[rotationState];
    std::vector<Position> movedTiles;
    
    // Apply the block's global offset to calculate absolute grid positions
    for (Position item : tiles) {
        Position newPos = Position(item.row + rowOffset, item.column + columnOffset);
        movedTiles.push_back(newPos);
    }
    return movedTiles;
}

void Block::Rotate() {
    rotationState++;
    // Cycle back to 0 if we exceed the number of defined rotation states (usually 4)
    if (rotationState == (int)cells.size()) {
        rotationState = 0;
    }
}

void Block::UndoRotation() {
    rotationState--;
    // Cycle back to the last state if we go below 0
    if (rotationState == -1) {
        rotationState = (int)cells.size() - 1;
    }
}