/**
 * @file utils.hpp
 * @brief Definition of the Utils class.
 * Provides general utility functions, primarily for timing and game loop events.
 */

#pragma once
#include "raylib.h"

class Utils {
public:
    /**
     * Checks if a specific time interval has passed since the last update.
     * Used mainly for game tick logic (gravity).
     * True if the interval has passed, False otherwise
     */
    static bool EventTriggered(double interval, bool stopTimer);

private:
    static double lastUpdateTime;
};