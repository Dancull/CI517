#include "Renderer.h"

Renderer::Renderer(SDL_Renderer* renderer, int cellSize) : renderer(renderer), cellSize(cellSize) {}

void Renderer::render(const Grid& grid, const std::vector<Node*>& path) const {
    // Implement rendering logic
}

void Renderer::renderAxis(int width, int height) const {
    // Implement axis rendering logic
}
