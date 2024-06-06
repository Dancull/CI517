#pragma once
#include "Grid.h"
#include "Pathfinding.h"
#include <SDL.h>

class Renderer {
public:
    Renderer(SDL_Renderer* renderer, int cellSize);
    void render(const Grid& grid, const std::vector<Node*>& path) const;
    void renderAxis(int width, int height) const;
private:
    SDL_Renderer* renderer;
    int cellSize;
};
