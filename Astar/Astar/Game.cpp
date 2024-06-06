#include "Game.h"

Game::Game(int screenWidth, int screenHeight, int gridWidth, int gridHeight, int cellSize)
    : grid(gridWidth, gridHeight), gridRenderer(renderer, cellSize) {
    // Initialize SDL and create window/renderer
}

void Game::run() {
    // Main game loop
}
