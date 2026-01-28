/**
 * @file ui_manager.hpp
 * @brief Definition of the UIManager class.
 * Provides static helper methods to render standard UI elements like buttons
 * and the main game interface overlay (Pause, Restart, Menu).
 */

#pragma once
#include "raylib.h"
#include "NetworkManager.hpp"
#include "game_types.hpp"

class UIManager {
public:
    // Draws a generic confirmation button and returns true if clicked.
    static bool DrawConfirmButton(Font font, const char* text, Rectangle rect, float p);

    // Draws the standard in-game HUD buttons (Menu, Restart, Pause).
    static void DrawInterfaceButtons(Font font, float p, bool showRestart, GameState state, NetworkManager& net, bool& showMenuConfirm, bool& showRestartConfirm, bool& isPaused);
};