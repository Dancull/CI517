#include "Grid.h"

Grid::Grid(int width, int height) : width(width), height(height) {
    data.resize(height, std::vector<int>(width, 0));
}

void Grid::setObstacle(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        data[y][x] = 1;
    }
}

void Grid::clearObstacle(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        data[y][x] = 0;
    }
}

bool Grid::isObstacle(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return data[y][x] == 1;
    }
    return false;
}
