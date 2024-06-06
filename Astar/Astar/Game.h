#pragma once
#include "Grid.h"
#include "Pathfinding.h"
#include "Renderer.h"
#include <SDL.h>

class Game {
public:
    Game(int screenWidth, int screenHeight, int gridWidth, int gridHeight, int cellSize);
    void run();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Grid grid;
    Renderer gridRenderer;
    // Other game components...
};
