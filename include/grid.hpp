/**
 * @file grid.hpp
 * @brief Definition of the Grid class.
 * Represents the Tetris playfield, handling cell states, row clearing,
 * and rendering of the game board.
 */

#pragma once
#include <vector>
#include "raylib.h"

class Grid {
public:
    Grid();

    // Resets the grid to an empty state (all zeros).
    void Initalize();

    // Prints the grid state to the console (debug purposes).
    void Print();

    //Renders the grid on the screen.
    void Drawn(int offsetX, int offsetY, int dynamicCellSize);

    // Checks if specific coordinates are outside the grid boundaries.
    bool IsCellOutside(int row, int column);

    // Checks if a specific cell is empty (0) and within valid bounds.
    // Note: Cells above the grid (negative rows) are considered valid empty space for spawning.
    bool IsCellEmpty(int row, int column);

    // Scan for full rows, clears them, moves blocks down, and returns the count of cleared rows.
    int ClearFullRows();

    // The actual 2D array representing the game board (20 rows x 10 columns).
    // 0 = Empty, 1-7 = Color IDs of blocks.
    int grid[20][10];

private:
    // Helper to check if a single row is completely filled.
    bool IsRowFull(int row);

    // Helper to clear a single row (set all cells to 0).
    void ClearRow(int row);

    // Helper to move a row down by a specific number of steps.
    void MoveRowDown(int row, int numRows);

    int numRows;
    int numColums;
    int cellSize;
    std::vector<Color> colors;
};