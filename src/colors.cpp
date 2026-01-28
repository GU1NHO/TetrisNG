/**
 * @file colors.cpp
 * @brief Definition of color constants used in the game.
 */

#include "../include/colors.hpp"
#include <vector>

// Color Palette Definitions ---
// Uses RGBA format: {Red, Green, Blue, Alpha}

const Color darkGrey = {26, 31, 40, 255};
const Color green    = {47, 230, 23, 255};
const Color red      = {232, 18, 18, 255};
const Color orange   = {226, 116, 17, 255};
const Color yellow   = {237, 234, 4, 255};
const Color purple   = {166, 0, 247, 255};
const Color cyan     = {21, 204, 209, 255};
const Color blue     = {13, 64, 216, 255};
const Color lightBlue= {59, 85, 162, 255};
const Color darkBlue = {44, 44, 127, 255};

std::vector<Color> GetCellColors() {
     // Returns the vector indexed by Block ID
     // Index 0: Background (darkGrey), Index 1: LBlock (Green?), etc.
     // Note: Ensure these match the IDs assigned in blocks.cpp logic
     return {darkGrey, green, red, orange, yellow, purple, cyan, blue};
}