/**
 * @file menu.hpp
 * @brief Definition of the Menu class.
 */

#pragma once
#include "raylib.h"
#include <string>
#include <vector>

class Menu {
public:
    Menu();

    // Render the menu elements on the screen.
    void Draw(Font font);

    // Processes input for navigation (Keyboard and Mouse).
    // Returns the selected option index or -1 if nothing was confirmed.
    int HandleInput();

    // Returns the currently highlighted option index.
    int GetSelectedOption() { return selectedOption; }

private:
    int selectedOption;
    std::vector<std::string> options;
};