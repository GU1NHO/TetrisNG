/**
 * @file utils.cpp
 * @brief Implementation of the Utils class.
 */

#include "../include/utils.hpp"

// Initialize static member
double Utils::lastUpdateTime = 0;

bool Utils::EventTriggered(double interval, bool stopTimer) {
    double currentTime = GetTime();
    
    // Anti-Lag / Anti-Teleport Logic:
    // If the game is paused (stopTimer is true), we constantly update
    // lastUpdateTime to the current time. 
    // This ensures that when the game resumes, the time delta is effectively 0,
    // preventing the block from instantly dropping multiple rows due to accumulated time.
    if (stopTimer) {
        lastUpdateTime = currentTime; 
        return false;
    }
    
    // Check if the target interval has elapsed
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    
    return false;
}