/**
 * @file grid.cpp
 * @brief Implementation of the Grid class.
 */

#include "../include/grid.hpp"
#include <iostream>
#include "../include/colors.hpp"

Grid::Grid() {
    numColums = 10;
    numRows = 20;
    Initalize();
    colors = GetCellColors();
    cellSize = 30; // Default fallback size, overridden in Draw
}

void Grid::Initalize() {
    // Fill the grid with zeros (empty cells)
    for (int row = 0; row < numRows; row++) {
        for (int column = 0; column < numColums; column++) {
            grid[row][column] = 0;
        }
    }
}

void Grid::Print() {
    // Debug output to console
    for (int row = 0; row < numRows; row++) {
        for (int column = 0; column < numColums; column++) {
            std::cout << grid[row][column] << " ";
        }
        std::cout << std::endl;
    }
}

void Grid::Drawn(int offsetX, int offsetY, int dynamicCellSize) {
    for (int row = 0; row < numRows; row++) {
        for (int column = 0; column < numColums; column++) {
            int cellValue = grid[row][column];
            
            // Draw each cell using the dynamic size calculated in Game::Draw
            // We subtract 1 from the size to create a small grid line effect
            DrawRectangle(column * dynamicCellSize + offsetX, 
                          row * dynamicCellSize + offsetY, 
                          dynamicCellSize - 1, 
                          dynamicCellSize - 1, 
                          colors[cellValue]);
        }
    }
}

bool Grid::IsCellOutside(int row, int column) {
    if (row >= 0 && row < numRows && column >= 0 && column < numColums) {
        return false; // Inside valid bounds
    }
    return true; // Outside bounds
}

bool Grid::IsCellEmpty(int row, int column) {
    // 1. Check lateral boundaries
    if (column < 0 || column >= numColums) return false;
    
    // 2. Check floor boundary
    if (row >= numRows) return false;

    // 3. Check ceiling (Spawning area)
    // Negative rows are valid empty space for new blocks entering the grid.
    if (row < 0) return true; 

    // 4. Check collision with existing blocks
    return (grid[row][column] == 0);
}

int Grid::ClearFullRows() {
    int completed = 0;
    
    // Iterate from bottom to top
    for (int row = numRows - 1; row >= 0; row--) {
        if (IsRowFull(row)) {
            ClearRow(row);
            completed++;
        }
        else if (completed > 0) {
            // If rows were cleared below, move the current row down
            MoveRowDown(row, completed);
        }
    }
    return completed;
}

bool Grid::IsRowFull(int row) {
    for (int column = 0; column < numColums; column++) {
        if (grid[row][column] == 0) return false;
    }
    return true;
}

void Grid::ClearRow(int row) {
    for (int column = 0; column < numColums; column++) {
        grid[row][column] = 0;
    }
}

void Grid::MoveRowDown(int row, int numRows) {
    for (int column = 0; column < numColums; column++) {
        grid[row + numRows][column] = grid[row][column]; 
        grid[row][column] = 0;
    }
}