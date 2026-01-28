/**
 * @file menu.cpp
 * @brief Implementation of the Menu class.
 * Handles the main menu state, including option navigation, rendering,
 * and mouse/keyboard input processing.
 */

#include "../include/menu.hpp"
#include "../include/colors.hpp"

Menu::Menu() {
    selectedOption = 0;
    
    // Menu Options definition
    // Order corresponds to indices used in main.cpp switch logic:
    // 0: Normal Mode
    // 1: Dual Window Mode
    // 2: Host Game
    // 3: Join Game
    // 4: Rules
    // 5: Exit
    options = {"Normal Mode", "Dual Window", "Host Game", "Join Game", "Rules", "Exit"};
}

void Menu::Draw(Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    
    // Calculate scaling factor based on a reference height of 620px
    float p = screenH / 620.0f; 

    // --- Draw Title ---
    float titleFontSize = 64.0f * p;
    Vector2 titleSize = MeasureTextEx(font, "TETRIS", titleFontSize, 4);
    DrawTextEx(font, "TETRIS", {(screenW - titleSize.x) / 2.0f, 80.0f * p}, titleFontSize, 4, WHITE);

    // --- Draw Menu Options ---
    for (int i = 0; i < (int)options.size(); i++) {
        float fontSize = 38.0f * p;
        Color color = (i == selectedOption) ? YELLOW : WHITE;
        
        Vector2 textSize = MeasureTextEx(font, options[i].c_str(), fontSize, 2);
        float posX = (screenW - textSize.x) / 2.0f;
        
        // Vertical positioning: Start at 220px, with 65px spacing between items
        float posY = (220.0f + i * 65.0f) * p; 

        // Mouse Hover Logic
        // Check if the mouse cursor is inside the bounding box of the text option
        Rectangle itemRect = { posX, posY, textSize.x, textSize.y };
        bool isMouseOver = CheckCollisionPointRec(GetMousePosition(), itemRect);
        
        if (isMouseOver) {
            selectedOption = i;
        }

        // Render Text
        DrawTextEx(font, options[i].c_str(), {posX, posY}, fontSize, 2, color);
        
        // Render Selection Indicator (Arrow)
        if (i == selectedOption) {
            DrawTextEx(font, ">", {posX - (30.0f * p), posY}, fontSize, 2, YELLOW);
        }
    }
}

int Menu::HandleInput() {
    int keyPressed = GetKeyPressed();

    // Navigation Down
    if (keyPressed == KEY_DOWN || keyPressed == KEY_S) {
        selectedOption++;
        if (selectedOption >= (int)options.size()) selectedOption = 0; // Wrap around to top
    }
    // Navigation Up
    else if (keyPressed == KEY_UP || keyPressed == KEY_W) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = (int)options.size() - 1; // Wrap around to bottom
    }
    
    // Mouse Support
    bool mouseClicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    // Confirmation (Enter or Click)
    if (IsKeyPressed(KEY_ENTER) || mouseClicked) {
        return selectedOption;
    }

    return -1; // No selection confirmed yet
}