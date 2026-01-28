/**
 * @file input_handler.hpp
 * @brief Definition of the InputHandler class.
 * Provides static utilities for handling keyboard input, specifically implementing
 * the Delayed Auto Shift (DAS) mechanic for smooth block movement.
 */

#pragma once
#include "raylib.h"

class InputHandler {
public:
    /**
     * Handles key presses with DAS (Delayed Auto Shift).
     * Allows a key to be pressed once for a single action, or held down to repeat the action.
     * True if the action should trigger this frame, False otherwise.
     */
    static bool HandleKeyWithDAS(int key1, int key2, int playerIdx, int timerIdx, float interval, bool inputBlocked);

private:
    // Initial delay before auto-repeat starts (in seconds)
    static float dasDelay;
    
    // Matrix to track DAS timers: [Player Index][Action Index]
    static float timers[2][3]; 
};