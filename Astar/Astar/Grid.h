#pragma once
#include <vector>

class Grid {
public:
    Grid(int width, int height);
    void setObstacle(int x, int y);
    void clearObstacle(int x, int y);
    bool isObstacle(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
private:
    int width;
    int height;
    std::vector<std::vector<int>> data;
};
