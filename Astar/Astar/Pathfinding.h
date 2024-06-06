#pragma once
#include "Grid.h"
#include <vector>

struct Node {
    int x, y;
    float g, h;
    Node* parent;
    Node(int x, int y) : x(x), y(y), g(0), h(0), parent(nullptr) {}
    float f() const { return g + h; }
    bool operator==(const Node& other) const { return x == other.x && y == other.y; }
};

struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->f() > b->f();
    }
};

class Pathfinding {
public:
    static std::vector<Node*> findPath(const Grid& grid, const Node& start, const Node& end);
private:
    static float heuristic(const Node& a, const Node& b);
    static std::vector<Node*> getNeighbors(const Node& node, const Grid& grid);
    static std::vector<Node*> openList_container; // Define openList_container here
};
