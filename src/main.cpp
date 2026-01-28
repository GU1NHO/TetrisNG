/**
 * @file main.cpp
 * @brief Entry point of the Tetris NG application.
 * Handles the main game loop, state management, window initialization,
 * and high-level input processing.
 */

#include "raylib.h"
#include "../include/game.hpp"
#include "../include/colors.hpp"
#include "../include/menu.hpp"
#include "../include/NetworkManager.hpp"
#include "../include/game_types.hpp"
#include "../include/ui_manager.hpp"
#include "../include/input_handler.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <ctime>
#include <algorithm> 

// --- Global Variables ---
bool isPaused = false;
bool showMenuConfirm = false;
bool showRestartConfirm = false; 
float countdownTimer = 0; 
char lastConnectedIP[16] = "127.0.0.1";
MenuState currentMenuState = MAIN;
int letterCount = 9;

// --- Seed Configuration ---
bool useSameSeeds = true;      
bool showDualSeedMenu = false; 

// --- DAS Configuration ---
// 'dasInterval' defines the repetition speed when a key is held down.
float dasInterval = 0.06f; 

int main() {
    // --- Application Startup (Clean Initialization) ---
    // Start window hidden to avoid flickering during resize/positioning
    SetConfigFlags(FLAG_WINDOW_HIDDEN); 
    InitWindow(100, 100, "Tetris NG");
    
    // Load and set window icon
    Image icon = LoadImage("..\\Assets\\icon.png"); 
    if (icon.width > 0) { 
        SetWindowIcon(icon); 
        UnloadImage(icon); 
    }
    SetExitKey(KEY_NULL); // Disable default ESC exit behavior
   
    // Calculate responsive window size based on monitor height
    int monitor = GetCurrentMonitor();
    float p = (GetMonitorHeight(monitor) * 0.8f) / 620.0f;
    int winH = 620 * p;
    int winW_Single = 500 * p;
    int winW_Dual = 1000 * p;

    // Apply size and center window while hidden
    SetWindowSize(winW_Single, winH);
    SetWindowPosition((GetMonitorWidth(monitor) - winW_Single) / 2, (GetMonitorHeight(monitor) - winH) / 2);
    
    SetTargetFPS(60);
    ClearWindowState(FLAG_WINDOW_HIDDEN); // Show window ready
    // --------------------------------------------------

    // Load Resources
    Texture2D background = LoadTexture("..\\Assets\\background.png");
    Texture2D rulesImg = LoadTexture("..\\Assets\\rules.png");
    Font font = LoadFontEx("..\\Font\\monogram.ttf", 64, 0, 0);

    // Initialize Game Objects
    Game gameSolo(true);
    Game gameP1(false); // Player 1 in Dual/Online mode (WASD)
    Game gameP2(true);  // Player 2 in Dual/Online mode (Arrows)
    NetworkManager net; 
    Menu menu; 
    GameState currentState = MENU;

    // --- Main Game Loop ---
    while (!WindowShouldClose()) {
        // Update global countdown timer
        if (countdownTimer > 0 && !isPaused && !showMenuConfirm && !showRestartConfirm && !showDualSeedMenu) {
            countdownTimer -= GetFrameTime();
        }

        // Determine input and timer blocking states
        bool inputBlocked = (isPaused || showMenuConfirm || showRestartConfirm || countdownTimer > 0 || showDualSeedMenu);
        bool timerStopped = (countdownTimer > 0 || isPaused || showDualSeedMenu);

        BeginDrawing();
        ClearBackground(BLACK); 

        // Draw Background (except in Rules screen)
        if (currentState != RULES) {
            float screenW = (float)GetScreenWidth(); 
            float screenH = (float)GetScreenHeight();
            float scale = std::max(screenW / background.width, screenH / background.height);
            float destW = background.width * scale; 
            float destH = background.height * scale;
            
            DrawTexturePro(background, {0,0,(float)background.width,(float)background.height}, 
                           {(screenW-destW)/2, (screenH-destH)/2, destW, destH}, {0,0}, 0.0f, WHITE);
            DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(darkBlue, 0.85f));
        }

        // --- STATE: MENU ---
        if (currentState == MENU) {
            isPaused = false; 
            showMenuConfirm = false; 
            showRestartConfirm = false; 
            countdownTimer = 0;

            // Ensure window is Single Player size in menu
            if (GetScreenWidth() != winW_Single) {
                SetWindowSize(winW_Single, winH);
                int m = GetCurrentMonitor();
                SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
            }

            // Sub-state: Dual Player Seed Selection
            if (showDualSeedMenu) {
                menu.Draw(font); // Draw menu background behind overlay
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                DrawTextEx(font, "USE SAME PIECES FOR BOTH?", {winW_Single/2.0f - 150*p, winH/2.0f - 40*p}, 25*p, 2, WHITE);
                
                Rectangle yR = { winW_Single/2.0f - 110*p, winH/2.0f + 20*p, 100*p, 40*p };
                Rectangle nR = { winW_Single/2.0f + 10*p, winH/2.0f + 20*p, 100*p, 40*p };

                if (UIManager::DrawConfirmButton(font, "YES (Y)", yR, p) || IsKeyPressed(KEY_Y)) { 
                    useSameSeeds = true; 
                    showDualSeedMenu = false; 
                    currentState = DUAL_PLAYING; 
                    unsigned int s = (unsigned int)time(NULL); 
                    gameP1.Reset(s); 
                    gameP2.Reset(s); 
                    
                    SetWindowSize(winW_Dual, winH); 
                    countdownTimer = 3.5f; 
                    int m = GetCurrentMonitor(); 
                    SetWindowPosition((GetMonitorWidth(m) - winW_Dual) / 2, (GetMonitorHeight(m) - winH) / 2);
                }
                if (UIManager::DrawConfirmButton(font, "NO (N)", nR, p) || IsKeyPressed(KEY_N)) { 
                    useSameSeeds = false; 
                    showDualSeedMenu = false; 
                    currentState = DUAL_PLAYING; 
                    gameP1.Reset((unsigned int)time(NULL)); 
                    gameP2.Reset((unsigned int)time(NULL) + 100); 
                    
                    SetWindowSize(winW_Dual, winH); 
                    countdownTimer = 3.5f; 
                    int m = GetCurrentMonitor(); 
                    SetWindowPosition((GetMonitorWidth(m) - winW_Dual) / 2, (GetMonitorHeight(m) - winH) / 2);
                }
            }
            // Sub-state: Main Menu
            else if (currentMenuState == MAIN) {
                menu.Draw(font);
                int choice = menu.HandleInput();
                
                if (choice == 0) { // Play Solo
                    currentState = PLAYING; 
                    gameSolo.Reset(); 
                    countdownTimer = 3.5f; 
                    SetWindowSize(winW_Single, winH);
                    int m = GetCurrentMonitor(); 
                    SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                } 
                else if (choice == 1) { // Dual Local
                    showDualSeedMenu = true; 
                } 
                else if (choice == 2) { // Host Game
                    if (net.StartServer(1234)) { 
                        currentState = ONLINE_PLAYING; 
                        net.opponentQuit = false; 
                        net.remoteStartedNewGame = false;
                        SetWindowSize(winW_Dual, winH);
                        int m = GetCurrentMonitor(); 
                        SetWindowPosition((GetMonitorWidth(m) - winW_Dual) / 2, (GetMonitorHeight(m) - winH) / 2);
                    }
                } 
                else if (choice == 3) { // Join Game
                    currentMenuState = IP_INPUT; 
                    letterCount = strlen(lastConnectedIP); 
                } 
                else if (choice == 4) { // Rules
                    currentState = RULES; 
                    SetWindowSize(rulesImg.width, rulesImg.height);
                    int m = GetCurrentMonitor(); 
                    SetWindowPosition((GetMonitorWidth(m) - rulesImg.width) / 2, (GetMonitorHeight(m) - rulesImg.height) / 2);
                } 
                else if (choice == 5 || IsKeyPressed(KEY_ESCAPE)) { // Quit
                    break; 
                } 
            }
            // Sub-state: IP Input
            else if (currentMenuState == IP_INPUT) {
                int key = GetCharPressed();
                while (key > 0) { 
                    if (((key >= '0' && key <= '9') || key == '.') && (letterCount < 15)) { 
                        lastConnectedIP[letterCount] = (char)key; 
                        lastConnectedIP[letterCount+1] = '\0'; 
                        letterCount++; 
                    } 
                    key = GetCharPressed(); 
                }
                if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) { 
                    letterCount--; 
                    lastConnectedIP[letterCount] = '\0'; 
                }

                DrawTextEx(font, "ENTER HOST IP:", {winW_Single/2.0f - 120*p, winH/2.0f - 60*p}, 25*p, 2, WHITE);
                DrawRectangleLinesEx({winW_Single/2.0f - 130*p, winH/2.0f - 20*p, 260*p, 50*p}, 2, WHITE);
                DrawTextEx(font, lastConnectedIP, {winW_Single/2.0f - 120*p, winH/2.0f - 10*p}, 30*p, 2, YELLOW);

                Rectangle connectRect = { winW_Single/2.0f - 145*p, winH/2.0f + 50*p, 140*p, 40*p };
                Rectangle backRect    = { winW_Single/2.0f + 5*p,   winH/2.0f + 50*p, 140*p, 40*p };

                if (UIManager::DrawConfirmButton(font, "CONNECT", connectRect, p) || IsKeyPressed(KEY_ENTER)) {
                    if (net.StartClient(lastConnectedIP, 1234)) {
                        currentState = ONLINE_PLAYING; 
                        currentMenuState = MAIN;
                        gameP1.Reset(); 
                        gameP2.Reset(); 
                        net.opponentQuit = false; 
                        net.remoteStartedNewGame = false;
                        SetWindowSize(winW_Dual, winH);
                        int m = GetCurrentMonitor(); 
                        SetWindowPosition((GetMonitorWidth(m) - winW_Dual) / 2, (GetMonitorHeight(m) - winH) / 2);
                    }
                }
                if (UIManager::DrawConfirmButton(font, "MENU (M)", backRect, p) || IsKeyPressed(KEY_M)) {
                    currentMenuState = MAIN;
                }
            }
        } 
        
        // --- STATE: RULES ---
        else if (currentState == RULES) {
            DrawTexture(rulesImg, 0, 0, WHITE);
            Rectangle backBtn = { 20*p, 20*p, 100*p, 40*p };
            
            if (UIManager::DrawConfirmButton(font, "BACK", backBtn, p) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                currentState = MENU; 
                SetWindowSize(winW_Single, winH);
                int m = GetCurrentMonitor(); 
                SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
            }
        }
        // --- STATE: IN-GAME (Solo, Dual, Online) ---
        else {
            if (IsKeyPressed(KEY_ESCAPE)) showMenuConfirm = !showMenuConfirm; 
            
            bool gameOver = false;
            if (currentState == PLAYING) gameOver = gameSolo.gameOver;
            else if (currentState == DUAL_PLAYING || currentState == ONLINE_PLAYING) gameOver = (gameP1.gameOver && gameP2.gameOver);
            
            if (!gameOver) {
                if (IsKeyPressed(KEY_M)) showMenuConfirm = true;
                if (IsKeyPressed(KEY_R) && currentState != ONLINE_PLAYING) showRestartConfirm = true;
            }

            bool connected = (currentState != ONLINE_PLAYING || net.isConnected);
            
            // Draw standard UI buttons ONLY if game is active
            if (!gameOver && !net.opponentQuit && (currentState != ONLINE_PLAYING || net.isConnected)) {
                UIManager::DrawInterfaceButtons(font, p, connected, currentState, net, showMenuConfirm, showRestartConfirm, isPaused);
            }
            
            // --- ONLINE LOGIC ---
            if (currentState == ONLINE_PLAYING) {
                net.Update(gameP1, gameP2, isPaused, countdownTimer, useSameSeeds);
                
                if (!net.isConnected && net.role == CLIENT && !net.opponentQuit) {
                    static double lastRetry = 0;
                    if (GetTime() - lastRetry > 1.0) { 
                        net.StartClient(lastConnectedIP, 1234); 
                        lastRetry = GetTime(); 
                    }
                }

                bool anyReqActive = net.restartRequestReceived || net.restartRequestPending || 
                                    net.pauseRequestReceived || net.pauseRequestPending || 
                                    net.resumeRequestReceived || net.resumeRequestPending || 
                                    net.remoteStartedNewGame;

                if (net.opponentQuit || !net.isConnected) {
                    isPaused = false;
                    
                    if (net.role == SERVER && !net.opponentQuit && !net.isConnected) {
                        DrawTextEx(font, "WAITING FOR PLAYER...", {100.0f * p, winH/2.0f - 60*p}, 30*p, 2, YELLOW);
                        
                        Rectangle seedBtn = { 100*p, winH/2.0f + 80*p, 200*p, 40*p };
                        const char* seedTxt = useSameSeeds ? "SEEDS: EQUAL" : "SEEDS: RANDOM";
                        if (UIManager::DrawConfirmButton(font, seedTxt, seedBtn, p)) useSameSeeds = !useSameSeeds;
                        
                        DrawTextEx(font, net.GetLocalIPInfo().c_str(), {100*p, winH/2.0f + 20*p}, 20*p, 2, GREEN);
                        
                        Rectangle menuRect = { winW_Dual - 150*p, winH - 50*p, 140*p, 40*p };
                        if (UIManager::DrawConfirmButton(font, "MENU (M)", menuRect, p) || IsKeyPressed(KEY_M)) {
                            net.Stop(); 
                            currentState = MENU; 
                            SetWindowSize(winW_Single, winH);
                            int m = GetCurrentMonitor(); 
                            SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                        }
                    } 
                    else {
                        const char* title = "CONNECTION LOST.";
                        if (net.role == CLIENT && !net.opponentQuit) title = "CONNECTION LOST. RECONNECTING...";
                        const char* subtitle = "WAITING FOR OPPONENT RETURN...";
                        
                        Vector2 tSz = MeasureTextEx(font, title, 30*p, 2);
                        DrawTextEx(font, title, {winW_Dual/2.0f - tSz.x/2, winH/2.0f - 40*p}, 30*p, 2, YELLOW);
                        Vector2 sSz = MeasureTextEx(font, subtitle, 20*p, 2);
                        DrawTextEx(font, subtitle, {winW_Dual/2.0f - sSz.x/2, winH/2.0f + 10*p}, 20*p, 2, ORANGE);

                        Rectangle menuRect = { winW_Dual - 160*p, winH - 60*p, 140*p, 40*p };
                        if (UIManager::DrawConfirmButton(font, "MENU (M)", menuRect, p) || IsKeyPressed(KEY_M)) {
                            net.SendQuit(); currentState = MENU; net.Stop(); 
                            SetWindowSize(winW_Single, winH);
                            int m = GetCurrentMonitor(); SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                        }
                    }
                } 
                else if (net.remoteStartedNewGame) {
                    DrawRectangle(0, 0, winW_Dual, winH, Fade(BLACK, 0.86f));
                    DrawTextEx(font, "OPPONENT STARTED A NEW GAME!", {winW_Dual/2.0f - 180*p, winH/2.0f - 30*p}, 25*p, 2, YELLOW);
                    DrawTextEx(font, "PRESS [R] TO RESET AND JOIN", {winW_Dual/2.0f - 180*p, winH/2.0f + 20*p}, 22*p, 2, WHITE);
                    
                    if (IsKeyPressed(KEY_R)) { net.remoteStartedNewGame = false; gameP1.Reset(); gameP2.Reset(); }
                }
                else {
                    if (!anyReqActive && !showMenuConfirm && countdownTimer <= 0 && !gameP1.gameOver) {
                        if (IsKeyPressed(KEY_P)) { if (isPaused) net.SendRequest(PACKET_RESUME_REQ); else net.SendRequest(PACKET_PAUSE_REQ); }
                        if (IsKeyPressed(KEY_R)) net.SendRequest(PACKET_RESTART_REQ);
                        
                        InputState localIn = { 
                            InputHandler::HandleKeyWithDAS(KEY_LEFT, KEY_A, 0, 0, dasInterval, inputBlocked), 
                            InputHandler::HandleKeyWithDAS(KEY_RIGHT, KEY_D, 0, 1, dasInterval, inputBlocked), 
                            InputHandler::HandleKeyWithDAS(KEY_DOWN, KEY_S, 0, 2, dasInterval, inputBlocked), 
                            !inputBlocked && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)), 
                            false, gameP1.score 
                        };
                        net.SendInput(localIn); gameP1.HandleInput(localIn);
                    }
                    
                    if (net.role == SERVER && !isPaused && !anyReqActive && countdownTimer <= 0 && !gameP1.gameOver) {
                        if(Utils::EventTriggered(gameP1.GetSpeed(), timerStopped)) { 
                            gameP1.MoveBlockDown(); gameP2.MoveBlockDown(); net.SendTick(); 
                        }
                    }
                    
                    gameP1.Draw(0, 0, font); gameP2.Draw(winW_Single, 0, font);

                    if (gameP1.gameOver && gameP2.gameOver) {
                        DrawRectangle(0, 0, winW_Dual, winH, Fade(BLACK, 0.85f));
                        const char* res = (gameP1.score >= gameP2.score) ? "YOU WIN!" : "YOU LOSE!";
                        Color resColor = (gameP1.score >= gameP2.score) ? GREEN : RED;
                        
                        Vector2 txtSz = MeasureTextEx(font, res, 40*p, 2);
                        DrawTextEx(font, res, {winW_Dual/2.0f - txtSz.x/2, winH/2.0f - 60*p}, 40*p, 2, resColor);

                        if (!anyReqActive) {
                            Rectangle restartRect = { winW_Dual/2.0f - 150*p, winH/2.0f + 20*p, 140*p, 40*p };
                            Rectangle menuRect    = { winW_Dual/2.0f + 10*p,  winH/2.0f + 20*p, 140*p, 40*p };
                            
                            if (UIManager::DrawConfirmButton(font, "RESTART (R)", restartRect, p) || IsKeyPressed(KEY_R)) {
                                net.SendRequest(PACKET_RESTART_REQ);
                            }
                            if (UIManager::DrawConfirmButton(font, "MENU (M)", menuRect, p) || IsKeyPressed(KEY_M)) {
                                net.SendQuit(); currentState = MENU; net.Stop(); 
                                SetWindowSize(winW_Single, winH);
                                int m = GetCurrentMonitor(); SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                            }
                        }
                    }

                    if (anyReqActive) {
                        DrawRectangle(0, 0, winW_Dual, winH, Fade(BLACK, 0.86f)); 
                        if (net.restartRequestReceived || net.pauseRequestReceived || net.resumeRequestReceived) {
                            const char* txt = net.restartRequestReceived ? "OPPONENT WANTS RESTART?" : 
                                             (net.pauseRequestReceived ? "OPPONENT WANTS TO PAUSE?" : "OPPONENT WANTS TO RESUME?");
                            DrawTextEx(font, txt, {winW_Dual/2.0f - 150*p, winH/2.0f - 40*p}, 25*p, 2, WHITE);
                            
                            Rectangle yesRect = { winW_Dual/2.0f - 110*p, winH/2.0f + 10*p, 100*p, 40*p };
                            Rectangle noRect  = { winW_Dual/2.0f + 10*p,  winH/2.0f + 10*p, 100*p, 40*p };
                            
                            if (UIManager::DrawConfirmButton(font, "YES (Y)", yesRect, p) || IsKeyPressed(KEY_Y)) {
                                if (net.restartRequestReceived) {
                                    net.SendResponse(PACKET_RESTART_RES, true); 
                                    if (net.role == SERVER) {
                                        unsigned int newSeed = (unsigned int)time(NULL);
                                        unsigned int s2 = useSameSeeds ? newSeed : newSeed + 9999;
                                        net.SendSeed(newSeed, s2);
                                        gameP1.Reset(newSeed); gameP2.Reset(s2);
                                    }
                                } else {
                                    bool res = net.resumeRequestReceived;
                                    net.SendResponse(res ? PACKET_RESUME_RES : PACKET_PAUSE_RES, true);
                                    isPaused = !res;
                                }
                                countdownTimer = 3.5f; net.restartRequestReceived = net.pauseRequestReceived = net.resumeRequestReceived = false; 
                            }
                            if (UIManager::DrawConfirmButton(font, "NO (N)", noRect, p) || IsKeyPressed(KEY_N)) {
                                net.SendResponse(net.restartRequestReceived ? PACKET_RESTART_RES : (net.resumeRequestReceived ? PACKET_RESUME_RES : PACKET_PAUSE_RES), false);
                                net.restartRequestReceived = net.pauseRequestReceived = net.resumeRequestReceived = false;
                            }
                        } else { 
                            DrawTextEx(font, "WAITING FOR OPPONENT VALIDATION...", {winW_Dual/2.0f - 180*p, winH/2.0f}, 25*p, 2, YELLOW); 
                        }
                    }
                }
            } 
            // --- OFFLINE MODES ---
            else {
                if (currentState == PLAYING) { // Solo
                    if (!showMenuConfirm && !showRestartConfirm && IsKeyPressed(KEY_P)) isPaused = !isPaused;
                    
                    InputState soloIn = { 
                        InputHandler::HandleKeyWithDAS(KEY_LEFT, KEY_A, 0, 0, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_RIGHT, KEY_D, 0, 1, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_DOWN, KEY_S, 0, 2, dasInterval, inputBlocked), 
                        !inputBlocked && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)), 
                        false, gameSolo.score 
                    };
                    
                    gameSolo.HandleInput(soloIn); 
                    if(Utils::EventTriggered(gameSolo.GetSpeed(), timerStopped)) gameSolo.MoveBlockDown();
                    gameSolo.Draw(0, 0, font);

                    if (gameSolo.gameOver) {
                        DrawRectangle(0, 0, winW_Single, winH, Fade(BLACK, 0.85f));
                        DrawTextEx(font, "GAME OVER", {winW_Single/2.0f - 100*p, winH/2.0f - 60*p}, 40*p, 2, RED);

                        Rectangle restartRect = { winW_Single/2.0f - 150*p, winH/2.0f + 20*p, 140*p, 40*p };
                        Rectangle menuRect    = { winW_Single/2.0f + 10*p,  winH/2.0f + 20*p, 140*p, 40*p };

                        if (UIManager::DrawConfirmButton(font, "RESTART (R)", restartRect, p) || IsKeyPressed(KEY_R)) {
                            gameSolo.Reset((unsigned int)time(NULL)); 
                            countdownTimer = 3.5f; 
                        }
                        if (UIManager::DrawConfirmButton(font, "MENU (M)", menuRect, p) || IsKeyPressed(KEY_M)) {
                            currentState = MENU; 
                            SetWindowSize(winW_Single, winH);
                            int m = GetCurrentMonitor(); 
                            SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                        }
                    }
                } 
                else if (currentState == DUAL_PLAYING) { // Dual Local
                    if (!showMenuConfirm && !showRestartConfirm && IsKeyPressed(KEY_P)) isPaused = !isPaused;
                    
                    InputState p1In = { 
                        InputHandler::HandleKeyWithDAS(KEY_A, KEY_NULL, 0, 0, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_D, KEY_NULL, 0, 1, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_S, KEY_NULL, 0, 2, dasInterval, inputBlocked), 
                        !inputBlocked && IsKeyPressed(KEY_W), 
                        false, gameP1.score 
                    };
                    InputState p2In = { 
                        InputHandler::HandleKeyWithDAS(KEY_LEFT, KEY_NULL, 1, 0, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_RIGHT, KEY_NULL, 1, 1, dasInterval, inputBlocked), 
                        InputHandler::HandleKeyWithDAS(KEY_DOWN, KEY_NULL, 1, 2, dasInterval, inputBlocked), 
                        !inputBlocked && IsKeyPressed(KEY_UP), 
                        false, gameP2.score 
                    };
                    
                    gameP1.HandleInput(p1In); 
                    gameP2.HandleInput(p2In);
                    
                    if(Utils::EventTriggered(gameP1.GetSpeed(), timerStopped)) { 
                        gameP1.MoveBlockDown(); 
                        gameP2.MoveBlockDown(); 
                    }
                    
                    gameP1.Draw(0, 0, font); 
                    gameP2.Draw(winW_Single, 0, font);

                    // Game Over Screen (Local Dual)
                    if (gameP1.gameOver && gameP2.gameOver) {
                        DrawRectangle(0, 0, winW_Dual, winH, Fade(BLACK, 0.85f));
                        const char* res; 
                        Color col;
                        if (gameP1.score > gameP2.score) { res = "PLAYER 1 WINS!"; col = GREEN; }
                        else if (gameP2.score > gameP1.score) { res = "PLAYER 2 WINS!"; col = GREEN; }
                        else { res = "DRAW!"; col = YELLOW; }

                        Vector2 txtSz = MeasureTextEx(font, res, 40*p, 2);
                        DrawTextEx(font, res, {winW_Dual/2.0f - txtSz.x/2, winH/2.0f - 60*p}, 40*p, 2, col);

                        Rectangle restartRect = { winW_Dual/2.0f - 150*p, winH/2.0f + 20*p, 140*p, 40*p };
                        Rectangle menuRect    = { winW_Dual/2.0f + 10*p,  winH/2.0f + 20*p, 140*p, 40*p };

                        if (UIManager::DrawConfirmButton(font, "RESTART (R)", restartRect, p) || IsKeyPressed(KEY_R)) {
                            unsigned int s = (unsigned int)time(NULL); 
                            gameP1.Reset(s); 
                            gameP2.Reset(useSameSeeds ? s : s + 9999); 
                            countdownTimer = 3.5f; 
                        }
                        if (UIManager::DrawConfirmButton(font, "MENU (M)", menuRect, p) || IsKeyPressed(KEY_M)) {
                            currentState = MENU; 
                            SetWindowSize(winW_Single, winH);
                            int m = GetCurrentMonitor(); 
                            SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                        }
                    }
                }
            }

            // --- POPUPS: Menu Confirm & Restart Confirm ---
            if (showMenuConfirm || showRestartConfirm) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                const char* msg = showMenuConfirm ? "RETURN TO MENU?" : "RESTART GAME?";
                DrawTextEx(font, msg, {(float)GetScreenWidth()/2 - 120*p, (float)GetScreenHeight()/2 - 40*p}, 30*p, 2, WHITE);
                
                Rectangle yesRect = { (float)GetScreenWidth()/2 - 110*p, (float)GetScreenHeight()/2 + 10*p, 100*p, 40*p };
                Rectangle noRect  = { (float)GetScreenWidth()/2 + 10*p,  (float)GetScreenHeight()/2 + 10*p, 100*p, 40*p };
                
                if (UIManager::DrawConfirmButton(font, "YES (Y)", yesRect, p) || IsKeyPressed(KEY_Y)) {
                    if (showMenuConfirm) {
                        if (currentState == ONLINE_PLAYING) net.SendQuit();
                        currentState = MENU; 
                        net.Stop(); 
                        SetWindowSize(winW_Single, winH);
                        int m = GetCurrentMonitor(); 
                        SetWindowPosition((GetMonitorWidth(m) - winW_Single) / 2, (GetMonitorHeight(m) - winH) / 2);
                    } else {
                        // Restart Logic (Unified)
                        unsigned int s = (unsigned int)time(NULL);
                        if (currentState == PLAYING) {
                            gameSolo.Reset(s);
                        } else {
                            gameP1.Reset(s); 
                            gameP2.Reset(useSameSeeds ? s : s + 9999);
                        }
                        countdownTimer = 3.5f; 
                        Utils::EventTriggered(0, true); 
                    }
                    showMenuConfirm = showRestartConfirm = false;
                }
                if (UIManager::DrawConfirmButton(font, "NO (N)", noRect, p) || IsKeyPressed(KEY_N)) {
                    showMenuConfirm = showRestartConfirm = false;
                }
            }

            // --- PAUSE OVERLAY ---
            bool isWaitingReq = net.restartRequestReceived || net.pauseRequestReceived || 
                                net.resumeRequestReceived || net.restartRequestPending || 
                                net.pauseRequestPending || net.resumeRequestPending;

            if (isPaused && !showMenuConfirm && !showRestartConfirm && !isWaitingReq && countdownTimer <= 0) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.86f));
                DrawTextEx(font, "PAUSED", {(float)GetScreenWidth()/2 - 50*p, (float)GetScreenHeight()/2}, 40*p, 2, RED);
                
                Rectangle resumeRect = { (float)GetScreenWidth()/2 - 70*p, (float)GetScreenHeight()/2 + 60*p, 140*p, 40*p };
                if (UIManager::DrawConfirmButton(font, "RESUME (P)", resumeRect, p) || IsKeyPressed(KEY_P)) {
                    if (currentState == ONLINE_PLAYING) net.SendRequest(PACKET_RESUME_REQ); 
                    else { 
                        isPaused = false; 
                        countdownTimer = 3.5f; 
                    }
                }
            }

            // --- COUNTDOWN OVERLAY ---
            if (countdownTimer > 0 && !isPaused && !showMenuConfirm && !showRestartConfirm && !showDualSeedMenu) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));
                int count = (int)countdownTimer;
                if (count > 0) {
                    const char* countText = TextFormat("%d", count);
                    Vector2 textSize = MeasureTextEx(font, countText, 100 * p, 2);
                    DrawTextEx(font, countText, {(float)GetScreenWidth()/2 - textSize.x/2, (float)GetScreenHeight()/2 - textSize.y/2}, 100 * p, 2, YELLOW);
                }
            }
        }
        EndDrawing();
    }
    
    // Cleanup
    UnloadTexture(background);
    UnloadTexture(rulesImg); 
    CloseWindow();
    return 0;
}