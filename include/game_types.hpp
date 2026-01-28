/**
 * @file game_types.hpp
 * @brief Definition of global enumerations used for game state management.
 */

#pragma once

// represents the high-level state of the application
enum GameState { 
    MENU,           // Main menu screen
    PLAYING,        // Singleplayer mode
    DUAL_PLAYING,   // Local multiplayer mode
    ONLINE_PLAYING, // Network multiplayer mode
    RULES           // Rules/Tutorial screen
};

// Represents the sub-states within the Menu
enum MenuState { 
    MAIN,           // Standard menu selection
    IP_INPUT        // Screen for typing IP address (Join Game)
};