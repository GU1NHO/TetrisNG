/**
 * @file game.cpp
 * @brief Implementation of the Game class logic.
 */

#include "game.hpp"
#include "raylib.h" 
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdio> // Required for sprintf

Game::Game(bool useArrowsInput) {
    grid = Grid();
    blocks = GetAllBlocks();
    
    // Initialize RNG with a random device seed
    rng.seed(std::random_device{}()); 
    
    Reset();
    useArrows = useArrowsInput;
}

// --- LOGIC: Block Management ---

Block Game::GetRandomBlock() {
    if (blocks.empty()) blocks = GetAllBlocks();
    
    // Select a random block using the instance-specific RNG
    std::uniform_int_distribution<int> dist(0, blocks.size() - 1);
    int randomIndex = dist(rng);
    
    Block block = blocks[randomIndex];
    blocks.erase(blocks.begin() + randomIndex);
    return block;
}

std::vector<Block> Game::GetAllBlocks() {
    return {IBlock(), JBlock(), LBlock(), OBlock(), SBlock(), TBlock(), ZBlock()};
}

// --- DRAWING ---

void Game::Draw(int offsetX, int offsetY, Font font) { 
    // Calculate dynamic sizes for responsive rendering
    int dynamicCellSize = GetScreenHeight() / 25; 
    float p = dynamicCellSize / 30.0f; 
    int gridStartX = offsetX + (int)(20 * p);
    int gridStartY = offsetY + (int)(20 * p);

    // 1. Draw Grid (Background)
    grid.Drawn(gridStartX, gridStartY, dynamicCellSize); 

    // 2. Draw Ghost Piece (Guide)
    DrawGhostPiece(offsetX, offsetY); 

    // 3. Draw Current Active Block
    if (!gameOver) {
        currentBlock.Draw(gridStartX, gridStartY, dynamicCellSize); 
    }

    // 4. Draw User Interface (Score, Next Piece, etc.)
    DrawUI(offsetX, offsetY, dynamicCellSize, font, p);
}

void Game::DrawUI(int offsetX, int offsetY, int cellSize, Font font, float p) {
    float uiX = offsetX + (12 * cellSize);
    
    // Draw Score Panel
    DrawTextEx(font, "Score", {uiX, offsetY + (20 * p)}, 30 * p, 2, WHITE);
    DrawRectangleRounded({uiX - (5*p), offsetY + (55 * p), 140 * p, 45 * p}, 0.3, 6, lightBlue);
    
    char scoreText[15];
    sprintf(scoreText, "%d", score);
    DrawTextEx(font, scoreText, {uiX + (10*p), offsetY + (65 * p)}, 25 * p, 2, WHITE);

    // Draw Level Panel
    DrawTextEx(font, "Level", {uiX, offsetY + (110 * p)}, 30 * p, 2, WHITE);
    DrawRectangleRounded({uiX - (5*p), offsetY + (145 * p), 140 * p, 45 * p}, 0.3, 6, lightBlue);
    
    char levelText[5];
    sprintf(levelText, "%d", level);
    DrawTextEx(font, levelText, {uiX + (10*p), offsetY + (155 * p)}, 25 * p, 2, YELLOW);
   
    // Draw Next Piece Panel
    DrawRectangleRounded({uiX - (5*p), offsetY + (245 * p), 140 * p, 100 * p}, 0.3, 6, lightBlue);
    DrawTextEx(font, "Next", {uiX, offsetY + (210 * p)}, 30 * p, 2, WHITE);
    nextBlock.Draw(uiX - (85*p), offsetY + (265 * p), cellSize);

    // Draw Game Over Message
    if (gameOver) {
        DrawTextEx(font, "GAME OVER", {uiX, offsetY + (450 * p)}, 30 * p, 2, YELLOW);
    }
}

void Game::DrawGhostPiece(int offsetX, int offsetY) {
    if (gameOver) return;

    int dynamicCellSize = GetScreenHeight() / 25; 
    float p = dynamicCellSize / 30.0f;
    int finalX = offsetX + (int)(20 * p);
    int finalY = offsetY + (int)(20 * p);

    Block ghostBlock = currentBlock;

    // Simulate hard drop to find landing position
    while (true) {
        ghostBlock.Move(1, 0);
        if (IsBlockOutside(ghostBlock) || !BlockFits(ghostBlock)) {
            ghostBlock.Move(-1, 0); // Step back to valid position
            break;
        }
    }

    // Draw with transparency (Alpha 50)
    ghostBlock.Draw(finalX, finalY, dynamicCellSize, {255, 255, 255, 50}); 
}

// --- LOGIC: Collision & Movement ---

bool Game::IsBlockOutside(Block block) {
    std::vector<Position> tiles = block.GetCellPositions();
    for(Position item: tiles){
        if(grid.IsCellOutside(item.row, item.column)){
            return true;
        }
    }
    return false;
}

bool Game::BlockFits(Block block) {
    std::vector<Position> tiles = block.GetCellPositions();
    for (Position item : tiles) {
        // Check if the cell is occupied in the grid
        if (grid.IsCellEmpty(item.row, item.column) == false) {
            return false;
        }
    }
    return true;
}

void Game::HandleInput(InputState input) {
    if (gameOver) { 
        if (input.reset) { Reset(); } 
        return; 
    }

    if (input.left)   MoveBlockLeft();
    if (input.right)  MoveBlockRight();
    if (input.rotate) RotateBlock();
    
    if (input.down) { 
        if (MoveBlockDown()) {
            UpdateScore(0, 1); // 1 point for soft drop
        }
    }
}

void Game::MoveBlockLeft() {
    if(!gameOver){
        currentBlock.Move(0, -1); 
        if (IsBlockOutside(currentBlock) || !BlockFits(currentBlock)){
            currentBlock.Move(0, 1); // Revert move
        }
    } 
}

void Game::MoveBlockRight(){
    if(!gameOver){
        currentBlock.Move(0, 1); 
        if (IsBlockOutside(currentBlock) || !BlockFits(currentBlock)){
            currentBlock.Move(0, -1); // Revert move
        }
    }
}

bool Game::MoveBlockDown(){ 
    if(!gameOver){
        currentBlock.Move(1, 0); 
        if (IsBlockOutside(currentBlock) || !BlockFits(currentBlock)){
            currentBlock.Move(-1, 0); // Revert move
            LockBlock();
            return false;
        }
        return true;
    }
    return false;
}

void Game::RotateBlock() {
    if(!gameOver){
        currentBlock.Rotate();
        // Check collision after rotation, undo if invalid (Wall kick could be implemented here)
        if(IsBlockOutside(currentBlock) || !BlockFits(currentBlock)){
            currentBlock.UndoRotation();
        }
    }
}

// --- LOGIC: Game State & Scoring ---

void Game::LockBlock() {
    std::vector<Position> tiles = currentBlock.GetCellPositions();
    for (Position item: tiles){
        if(item.row >= 0) {
            grid.grid[item.row][item.column] = currentBlock.id;
        }
        else {
            // Block locked above the visible grid area
            gameOver = true;
        }
    }

    if (!gameOver) {
        currentBlock = nextBlock;
        
        // Immediate loss check upon spawning new block
        if(!BlockFits(currentBlock)) {
            gameOver = true;
        }
        
        nextBlock = GetRandomBlock();
        int rowsCleared = grid.ClearFullRows();
        UpdateScore(rowsCleared, 0);
    }
}

double Game::GetSpeed() {
    // Increase speed as level increases (capped at 0.05s)
    return (double)fmax(0.05, 0.8 - ((level - 1) * 0.07)); 
}

void Game::Reset(int seed) {
    grid.Initalize();
    blocks = GetAllBlocks();
    
    // Seed RNG: Use provided seed (Multiplayer) or Time (Singleplayer)
    if (seed != -1) {
        rng.seed(seed);
    } else {
        rng.seed((unsigned int)time(NULL));
    }

    currentBlock = GetRandomBlock();
    nextBlock = GetRandomBlock();
    score = 0;
    level = 1;
    totalLinesCleared = 0;
    gameOver = false;
}

void Game::UpdateScore(int linesCleared, int moveDownPoints) {
    // Classic Tetris scoring system
    switch (linesCleared) {
        case 1: score += 100 * level; break;
        case 2: score += 300 * level; break;
        case 3: score += 500 * level; break;
        case 4: score += 800 * level; break;
        default: break;
    }
    score += moveDownPoints; 
    totalLinesCleared += linesCleared;
    level = 1 + (totalLinesCleared / 10);
}