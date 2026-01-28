/**
 * @file game.hpp
 * @brief Definition of the Game class.
 * Manages the core Tetris logic, including the grid, blocks, collision detection,
 * scoring and rendering.
 */

#pragma once
#include "grid.hpp"
#include "../src/blocks.cpp"
#include <random> 

// Structure to encapsulate input state for local and network processing
struct InputState {
    bool left;
    bool right;
    bool down;
    bool rotate;
    bool reset;
    int currentScore; // Used for multiplayer score synchronization
};

class Game {
public:
    // Constructor
    Game(bool useArrowsInput);

    // --- Core Game Loop Methods ---
    
    // Renders the game state (grid, blocks, UI)
    void Draw(int offsetX, int offsetY, Font font);
    
    // Processes input commands (movement, rotation)
    void HandleInput(InputState input);
    
    // Moves the current block down by one cell. Returns true if successful.
    bool MoveBlockDown();
    
    // Resets the game state. Accepts an optional seed for deterministic RNG (network play).
    void Reset(int seed = -1);

    // Calculates the current fall speed based on the level.
    double GetSpeed();

    // --- Public State Variables ---
    bool gameOver;
    int score;
    int level;             
    int totalLinesCleared; 

private:
    // --- Internal Logic Methods ---
    
    Block GetRandomBlock();
    std::vector<Block> GetAllBlocks();
    
    void MoveBlockLeft();
    void MoveBlockRight();
    void RotateBlock();
    
    // Locks the current block into the grid and triggers line clearing
    void LockBlock();
    
    // Collision Checks
    bool IsBlockOutside(Block block); 
    bool BlockFits(Block block); 
    
    void UpdateScore(int linesCleared, int moveDownPoints);

    // --- Drawing Helpers ---
    void DrawGhostPiece(int offsetX, int offsetY);
    void DrawUI(int offsetX, int offsetY, int cellSize, Font font, float p);

    // --- Member Variables ---
    Grid grid;
    std::vector<Block> blocks;
    Block currentBlock;
    Block nextBlock;
    bool useArrows;
    
    // Instance-specific random number generator
    std::mt19937 rng; 
};