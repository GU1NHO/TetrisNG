/**
 * @file blocks.cpp
 * @brief Implementation of specific Tetromino shapes (I, J, L, O, S, T, Z).
 * Each class defines the initial cell positions for all rotation states.
 */

#include "../include/block.hpp"
#include "../include/position.hpp"

// --- L Block (Orange) ---
class LBlock : public Block {
public:
    LBlock() {
       id = 1;
       cells[0] = {Position(0, 2), Position(1, 0), Position(1, 1), Position(1, 2)};
       cells[1] = {Position(0, 1), Position(1, 1), Position(2, 1), Position(2, 2)};
       cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 0)};
       cells[3] = {Position(0, 0), Position(0, 1), Position(1, 1), Position(2, 1)};
       Move(0, 3); // Initial spawn position
    }    
};

// --- J Block (Blue) ---
class JBlock : public Block {
public:
    JBlock() {
       id = 2;
       cells[0] = {Position(0, 0), Position(1, 0), Position(1, 1), Position(1, 2)};
       cells[1] = {Position(0, 1), Position(0, 2), Position(1, 1), Position(2, 1)};
       cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 2)};
       cells[3] = {Position(0, 1), Position(1, 1), Position(2, 0), Position(2, 1)};
       Move(0, 3);
    }    
};

// --- I Block (Cyan) ---
class IBlock : public Block {
public:
    IBlock() {
       id = 3;
       cells[0] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(1, 3)};
       cells[1] = {Position(0, 2), Position(1, 2), Position(2, 2), Position(3, 2)};
       cells[2] = {Position(2, 0), Position(2, 1), Position(2, 2), Position(2, 3)};
       cells[3] = {Position(0, 1), Position(1, 1), Position(2, 1), Position(3, 1)};
       Move(-1, 3); // Starts one row higher due to shape
    }    
};

// --- O Block (Yellow) ---
class OBlock : public Block {
public:
    OBlock() {
       id = 4;
       // O Block does not rotate visually, so all states are identical
       cells[0] = {Position(0, 0), Position(0, 1), Position(1, 0), Position(1, 1)};
       cells[1] = {Position(0, 0), Position(0, 1), Position(1, 0), Position(1, 1)};
       cells[2] = {Position(0, 0), Position(0, 1), Position(1, 0), Position(1, 1)};
       cells[3] = {Position(0, 0), Position(0, 1), Position(1, 0), Position(1, 1)};
       Move(0, 4);
    }   
}; 
    
// --- S Block (Green) ---
class SBlock : public Block {
public:
    SBlock() {
       id = 5;
       cells[0] = {Position(0, 1), Position(0, 2), Position(1, 0), Position(1, 1)};
       cells[1] = {Position(0, 1), Position(1, 1), Position(1, 2), Position(2, 2)};
       cells[2] = {Position(1, 1), Position(1, 2), Position(2, 0), Position(2, 1)};
       cells[3] = {Position(0, 0), Position(1, 0), Position(1, 1), Position(2, 1)};
       Move(0, 3);
    }    
};

// --- T Block (Purple) ---
class TBlock : public Block {
public:
    TBlock() {
       id = 6;
       cells[0] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(1, 2)};
       cells[1] = {Position(0, 1), Position(1, 1), Position(1, 2), Position(2, 1)};
       cells[2] = {Position(1, 0), Position(1, 1), Position(1, 2), Position(2, 1)};
       cells[3] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(2, 1)};
       Move(0, 3);
    }    
};

// --- Z Block (Red) ---
class ZBlock : public Block {
public:
    ZBlock() {
       id = 7;
       cells[0] = {Position(0, 0), Position(0, 1), Position(1, 1), Position(1, 2)};
       cells[1] = {Position(0, 2), Position(1, 1), Position(1, 2), Position(2, 1)};
       cells[2] = {Position(1, 0), Position(1, 1), Position(2, 1), Position(2, 2)};
       cells[3] = {Position(0, 1), Position(1, 0), Position(1, 1), Position(2, 0)};
       Move(0, 3);
    }    
};