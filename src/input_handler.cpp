#include "../include/input_handler.hpp"

// Inicialização das variáveis estáticas
float InputHandler::dasDelay = 0.20f;
float InputHandler::timers[2][3] = {0};

bool InputHandler::HandleKeyWithDAS(int key1, int key2, int playerIdx, int timerIdx, float interval, bool inputBlocked) {
    // Se o input estiver bloqueado (pausa, menus, contagem regressiva), não faz nada
    if (inputBlocked) return false;
    
    // Verifica se a tecla 1 OU a tecla 2 (se existir) foram pressionadas
    bool isPressed = IsKeyPressed(key1) || (key2 != KEY_NULL && IsKeyPressed(key2));
    bool isDown = IsKeyDown(key1) || (key2 != KEY_NULL && IsKeyDown(key2));

    if (isPressed) {
        timers[playerIdx][timerIdx] = GetTime() + dasDelay;
        return true;
    }
    if (isDown) {
        if (GetTime() >= timers[playerIdx][timerIdx]) {
            timers[playerIdx][timerIdx] = GetTime() + interval;
            return true;
        }
    }
    return false;
}