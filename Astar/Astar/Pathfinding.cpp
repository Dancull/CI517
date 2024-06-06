#include "Pathfinding.h"
#include <cmath>
#include <queue>
#include <algorithm>

std::vector<Node*> Pathfinding::openList_container;

std::vector<Node*> Pathfinding::findPath(const Grid& grid, const Node& start, const Node& end) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> openList;
    std::vector<Node*> closedList;

    openList.push(new Node(start.x, start.y));
    openList_container.push_back(new Node(start.x, start.y)); // Initialize openList_container

    while (!openList.empty()) {
        Node* current = openList.top();
        openList.pop();

        if (*current == end) {
            // Reconstruct path and return
            std::vector<Node*> path;
            while (current != nullptr) {
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedList.push_back(current);

        // Explore neighboring nodes
        for (auto neighbor : getNeighbors(*current, grid)) {
            if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end()) {
                delete neighbor;
                continue;
            }

            float tentative_g = current->g + 1;

            auto it = std::find_if(openList_container.begin(), openList_container.end(), [&](Node* n) { return *n == *neighbor; });
            if (it == openList_container.end() || tentative_g < neighbor->g) {
                neighbor->g = tentative_g;
                neighbor->h = heuristic(*neighbor, end);
                neighbor->parent = current;

                if (it == openList_container.end()) {
                    openList.push(neighbor);
                    openList_container.push_back(neighbor);
                }
            }
        }
    }

    return std::vector<Node*>();
}

float Pathfinding::heuristic(const Node& a, const Node& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Node*> Pathfinding::getNeighbors(const Node& node, const Grid& grid) {
    std::vector<Node*> neighbors;
    // Implement the logic to get neighboring nodes
    return neighbors;
}
