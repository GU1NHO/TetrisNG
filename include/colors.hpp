/**
 * @file colors.hpp
 * @brief Declaration of color constants and palette management.
 */

#pragma once
#include <raylib.h>
#include <vector>

// Global Color Definitions (Extern allows sharing across files)
extern const Color darkGrey;
extern const Color green;
extern const Color red;
extern const Color orange;
extern const Color yellow;
extern const Color purple;
extern const Color cyan;
extern const Color blue;
extern const Color lightBlue;
extern const Color darkBlue;

// Returns a vector containing the color palette for the blocks
std::vector<Color> GetCellColors();