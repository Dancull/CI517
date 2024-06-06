#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>

// Grid size constants
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 30;

// Node structure representing a cell in the grid
struct Node {
    int x, y; // Coordinates of the cell
    float g, h; // Cost and heuristic values for A* algorithm
    Node* parent; // Pointer to parent node for path reconstruction

    Node(int x, int y) : x(x), y(y), g(0), h(0), parent(nullptr) {}

    // Calculate the total cost f = g + h
    float f() const {
        return g + h;
    }

    // Overload equality operator for comparing nodes
    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};

// Functor for comparing nodes by their total cost f
struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->f() > b->f();
    }
};

// Heuristic function for estimating distance between two nodes
float heuristic(Node* a, Node* b) {
    // Manhattan distance
    return std::abs(a->x - b->x) + std::abs(a->y - b->y);
}

// Function to get neighboring nodes of a given node
std::vector<Node*> get_neighbors(Node* node, std::vector<std::vector<int>>& grid) {
    std::vector<Node*> neighbors;
    int dirs[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} }; // Possible directions: right, down, left, up
    for (auto& dir : dirs) {
        int nx = node->x + dir[0];
        int ny = node->y + dir[1];
        // Check if neighboring node is within grid boundaries and is not an obstacle
        if (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT && grid[ny][nx] == 0) {
            neighbors.push_back(new Node(nx, ny)); // Add valid neighboring node to the list
        }
    }
    return neighbors;
}

// A* algorithm implementation
void a_star(Node* start, Node* end, std::vector<std::vector<int>>& grid, std::vector<Node*>& path) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNodes> open_list;
    std::vector<Node*> open_list_container;
    std::vector<Node*> closed_list; // List of visited nodes

    open_list.push(start);
    open_list_container.push_back(start);

    while (!open_list.empty()) {
        Node* current = open_list.top();
        open_list.pop();
        open_list_container.erase(std::remove(open_list_container.begin(), open_list_container.end(), current), open_list_container.end());

        // debug code for the processing node
        std::cout << "Processing node (" << current->x << ", " << current->y << ") with f: " << current->f() << std::endl;

        // If current node is the destination, reconstruct path and return
        if (*current == *end) {
            Node* temp = current;
            while (temp != nullptr) {
                path.push_back(temp);
                temp = temp->parent;
            }
            std::reverse(path.begin(), path.end());
            return;
        }

        closed_list.push_back(current);

        // Explore neighboring nodes
        for (auto neighbor : get_neighbors(current, grid)) {
            // Skip if neighbor is already in closed list
            if (std::find(closed_list.begin(), closed_list.end(), neighbor) != closed_list.end()) {
                delete neighbor;
                continue;
            }

            // Calculate tentative cost to reach neighbor
            float tentative_g = current->g + 1;

            // Check if neighbor is already in open list or if new path to neighbor is better
            auto it = std::find_if(open_list_container.begin(), open_list_container.end(), [&](Node* n) { return *n == *neighbor; });
            if (it == open_list_container.end() || tentative_g < neighbor->g) {
                neighbor->g = tentative_g;
                neighbor->h = heuristic(neighbor, end);
                neighbor->parent = current;

                // debug neighbor evaluate check
                std::cout << "Considering neighbor (" << neighbor->x << ", " << neighbor->y << ") with g: " << neighbor->g << " h: " << neighbor->h << " f: " << neighbor->f() << std::endl;

                // If neighbor is not in open list, add it
                if (it == open_list_container.end()) {
                    open_list.push(neighbor);
                    open_list_container.push_back(neighbor);
                }
            }
        }
    }
}

// Function to render the grid
void render_grid(SDL_Renderer* renderer, std::vector<std::vector<int>>& grid) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            SDL_Rect cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE }; // Rectangle representing cell
            if (grid[y][x] == 1) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for obstacles
            }
            else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for free cells
            }
            SDL_RenderFillRect(renderer, &cell); // Fill cell with color
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Grid lines color
            SDL_RenderDrawRect(renderer, &cell); // Draw cell outline
        }
    }
}

// Function to render the path with thicker lines
void render_path(SDL_Renderer* renderer, std::vector<Node*>& path, int thickness) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the path

    for (size_t i = 1; i < path.size(); ++i) {
        int x1 = path[i - 1]->x * CELL_SIZE + CELL_SIZE / 2;
        int y1 = path[i - 1]->y * CELL_SIZE + CELL_SIZE / 2;
        int x2 = path[i]->x * CELL_SIZE + CELL_SIZE / 2;
        int y2 = path[i]->y * CELL_SIZE + CELL_SIZE / 2;

        for (int w = -thickness / 2; w <= thickness / 2; ++w) {
            SDL_RenderDrawLine(renderer, x1 - w, y1 - w, x2 - w, y2 - w);
            SDL_RenderDrawLine(renderer, x1 + w, y1 + w, x2 + w, y2 + w);
            SDL_RenderDrawLine(renderer, x1 - w, y1 + w, x2 - w, y2 + w);
            SDL_RenderDrawLine(renderer, x1 + w, y1 - w, x2 + w, y2 - w);
        }
    }
}

// Function to draw a single digit
void draw_digit(SDL_Renderer* renderer, int digit, int x, int y) {
    const int SEGMENT_LENGTH = 5; // Length of each segment
    const int SEGMENT_WIDTH = 1;  // Width of each segment
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for the digits

    // Coordinates for segments of a 7-segment display
    int segments[10][7][4] = {
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 0
        {{SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}}, // 1
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 2
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 3
        {{0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}}, // 4
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 5
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 6
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}}, // 7
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}}, // 8
        {{0, 0, SEGMENT_LENGTH, SEGMENT_WIDTH}, {0, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {SEGMENT_LENGTH, 0, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH}, {SEGMENT_LENGTH, SEGMENT_LENGTH, SEGMENT_WIDTH, SEGMENT_LENGTH}, {0, SEGMENT_LENGTH * 2, SEGMENT_LENGTH, SEGMENT_WIDTH}} // 9
    };

    for (auto& segment : segments[digit]) {
        SDL_Rect rect = { x + segment[0], y + segment[1], segment[2], segment[3] };
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Function to render axis labels
void render_axis(SDL_Renderer* renderer) {
    for (int i = 0; i < GRID_WIDTH; ++i) {
        draw_digit(renderer, i % 10, i * CELL_SIZE + CELL_SIZE / 3, 0);
        draw_digit(renderer, i % 10, 0, i * CELL_SIZE + CELL_SIZE / 3);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("A* Pathfinding", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    std::vector<std::vector<int>> grid(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, 0));
    for (int i = 5; i < 15; ++i) {
        grid[10][i] = 1;
        //grid[i][10] = 1;
        //grid[i][i] = 1;
    }

    Node* start = nullptr;
    Node* destination = nullptr;
    std::vector<Node*> path;

    // Load sound effects
    Mix_Chunk* placeSound = Mix_LoadWAV("sound1.wav");
    Mix_Chunk* resetSound = Mix_LoadWAV("easteregg.wav");

    if (!placeSound || !resetSound) {
        std::cerr << "Failed to load sound effect! SDL_Error: " << Mix_GetError() << std::endl;
    }

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;

                if (e.button.button == SDL_BUTTON_RIGHT) {
                    if (!start) {
                        start = new Node(gridX, gridY);
                    }
                    else if (!destination) {
                        destination = new Node(gridX, gridY);
                        a_star(start, destination, grid, path);
                    }
                    std::cout << "Right-clicked coordinates: (" << gridX << ", " << gridY << ")" << std::endl; // Debug message

                    // Play sound effect for placing node
                    if (placeSound) {
                        Mix_PlayChannel(-1, placeSound, 0);
                    }
                }
                else if (e.button.button == SDL_BUTTON_LEFT) {
                    // Toggle obstacle state
                    if (grid[gridY][gridX] == 0) {
                        grid[gridY][gridX] = 1; // Set obstacle
                    }
                    else {
                        grid[gridY][gridX] = 0; // Clear obstacle
                    }
                    std::cout << "Left-clicked coordinates: (" << gridX << ", " << gridY << ")" << std::endl; // Debug message
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_r) {
                    delete start;
                    delete destination;
                    start = nullptr;
                    destination = nullptr;
                    path.clear();

                    // Play sound effect for reset
                    if (resetSound) {
                        Mix_PlayChannel(-1, resetSound, 0);
                    }
                }
                else if (e.key.keysym.sym == SDLK_q) {
                    // Play sound effect for pressing 'q'
                    // Add your sound effect for 'q' here
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        render_grid(renderer, grid);
        if (start) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the start node
            SDL_Rect startRect = { start->x * CELL_SIZE, start->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderFillRect(renderer, &startRect);
        }
        if (destination) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for the destination node
            SDL_Rect destRect = { destination->x * CELL_SIZE, destination->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderFillRect(renderer, &destRect);
        }
        render_path(renderer, path, 3);
        render_axis(renderer);

        SDL_RenderPresent(renderer);
    }

    Mix_FreeChunk(placeSound);
    Mix_FreeChunk(resetSound);
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete start;
    delete destination;

    for (auto node : path) {
        delete node;
    }

    return 0;
}
