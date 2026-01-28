/**
 * @file ui_manager.cpp
 * @brief Implementation of the UIManager class.
 */

#include "ui_manager.hpp"
#include "colors.hpp" // Required for darkGrey and darkBlue constants

bool UIManager::DrawConfirmButton(Font font, const char* text, Rectangle rect, float p) {
    Vector2 mousePos = GetMousePosition();
    bool hover = CheckCollisionPointRec(mousePos, rect);
    
    // Draw Button Background (Highlight if hovered)
    DrawRectangleRec(rect, hover ? darkGrey : darkBlue);
    DrawRectangleLinesEx(rect, 2, WHITE);
    
    // Center Text inside the button
    Vector2 textSize = MeasureTextEx(font, text, 20 * p, 2);
    DrawTextEx(font, text, 
               { rect.x + (rect.width - textSize.x) / 2, rect.y + (rect.height - textSize.y) / 2 }, 
               20 * p, 2, WHITE);
    
    return (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

void UIManager::DrawInterfaceButtons(Font font, float p, bool showRestart, GameState state, NetworkManager& net, bool& showMenuConfirm, bool& showRestartConfirm, bool& isPaused) {
    // Check if any network request is currently active or pending.
    // If so, we disable buttons to prevent conflicting state changes.
    bool reqActive = net.restartRequestReceived || net.restartRequestPending ||
                     net.pauseRequestReceived || net.pauseRequestPending ||
                     net.resumeRequestReceived || net.resumeRequestPending ||
                     net.remoteStartedNewGame;

    // Define button dimensions and screen positioning
    float btnW = 140 * p; 
    float btnH = 40 * p;
    float screenW = (float)GetScreenWidth(); 
    float screenH = (float)GetScreenHeight();

    // Calculate dynamic positions (Bottom Right corner alignment)
    Rectangle menuRect      = { screenW - btnW - 10,           screenH - btnH - 10, btnW, btnH };
    Rectangle playAgainRect = { screenW - (btnW * 2) - 20,     screenH - btnH - 10, btnW, btnH };
    Rectangle pauseRect     = { screenW - (btnW * 3) - 30,     screenH - btnH - 10, btnW, btnH };

    Vector2 mousePos = GetMousePosition();

    // --- MENU BUTTON ---
    bool hoverMenu = CheckCollisionPointRec(mousePos, menuRect);
    if (hoverMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        showMenuConfirm = true;
    }
    
    DrawRectangleRec(menuRect, hoverMenu ? darkGrey : darkBlue);
    DrawRectangleLinesEx(menuRect, 2, WHITE);
    DrawTextEx(font, "MENU (M)", { menuRect.x + 10, menuRect.y + 10 }, 20 * p, 2, WHITE);

    // --- RESTART & PAUSE BUTTONS ---
    // Only rendered if the game is active, opponent hasn't quit, and no network sync is in progress
    if (showRestart && !net.opponentQuit && !reqActive) {
        
        // Restart Button
        bool hoverRestart = CheckCollisionPointRec(mousePos, playAgainRect);
        if (hoverRestart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (state == ONLINE_PLAYING) {
                net.SendRequest(PACKET_RESTART_REQ);
            } else {
                showRestartConfirm = true; 
            }
        }
        DrawRectangleRec(playAgainRect, hoverRestart ? darkGrey : darkBlue);
        DrawRectangleLinesEx(playAgainRect, 2, WHITE);
        DrawTextEx(font, "RESTART (R)", { playAgainRect.x + 10, playAgainRect.y + 10 }, 20 * p, 2, WHITE);
        
        // Pause Button
        bool hoverPause = CheckCollisionPointRec(mousePos, pauseRect);
        if (hoverPause && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (state == ONLINE_PLAYING) {
                if (isPaused) net.SendRequest(PACKET_RESUME_REQ);
                else net.SendRequest(PACKET_PAUSE_REQ);
            } else {
                isPaused = !isPaused;
            }
        }
        DrawRectangleRec(pauseRect, hoverPause ? darkGrey : darkBlue);
        DrawRectangleLinesEx(pauseRect, 2, WHITE);
        
        const char* pauseText = isPaused ? "RESUME (P)" : "PAUSE (P)";
        DrawTextEx(font, pauseText, { pauseRect.x + 10, pauseRect.y + 10 }, 20 * p, 2, WHITE);
    }
}