#include <bits/stdc++.h>
#include <iostream>
#include <raylib.h>
using namespace std;

// Make a grid
// 0 Empty
// 1 INCENTIVIZED BLOCK
// 2 GOAL
// 4 PATH

// Define the priority queue using a custom comparator

struct Coord {
  int x, y;
  bool operator==(const Coord &coord) const {
    return coord.x == x && coord.y == y;
  }
  bool operator!=(const Coord &coord) const { return !(*this == coord); }
};

namespace std {
template <> struct hash<Coord> {
  std::size_t operator()(const Coord &coord) const noexcept {
    std::size_t h1 = std::hash<int>{}(coord.x);
    std::size_t h2 = std::hash<int>{}(coord.y);
    return h1 ^ (h2 << 1); // Simple hash combining
  }
};
} // namespace std

struct Node {
  Coord loc;
  int cost;
  int fCost;
  Node(Coord loc, int cost, int fCost) : loc(loc), cost(cost), fCost(fCost) {}
  bool operator<(const Node &other) const { return fCost > other.fCost; }
};

int dx[] = {1, 0, -1, 0};
int dy[] = {0, 1, 0, -1};

int grid[40][40];
int CostCalc(Coord a, Coord b) { return abs(a.x - b.x) + abs(a.y - b.y); }

int CalcPenalty(Coord curr, Coord prev) { return -abs(curr.x - prev.x); }

// Depending on the value of the block give benefit, If the grid value is
// greater than 1 then give benefit of x - 1
int CalcBenefit(Coord curr) {
  if (grid[curr.x][curr.y] > 1) {
    return -(grid[curr.x][curr.y] - 1);
  }
  return 0;
}

vector<Coord> Astar(Coord start, Coord goal) {
  priority_queue<Node> openSet;
  unordered_map<Coord, int> gCost; // Tracks the best cost to a Coord
  unordered_map<Coord, Coord> cameFrom;
  Node startNode = Node(start, 0, CostCalc(start, goal));
  openSet.push(startNode);
  gCost[start] = 0;

  while (!openSet.empty()) {
    Node currentNode = openSet.top();
    Coord currentCoord = currentNode.loc;
    openSet.pop();

    if (currentCoord == goal) {
      // cout << "Found the goal with cost: " << currentNode->cost << endl;
      vector<Coord> path;
      while (currentCoord != start) {
        path.push_back(currentCoord);
        currentCoord = cameFrom[currentCoord];
      }
      path.push_back(currentCoord);
      return path;
    }

    for (int k = 0; k < 4; ++k) {
      Coord newCoord = {currentCoord.x + dx[k], currentCoord.y + dy[k]};

      if (newCoord.x < 0 || newCoord.y < 0 || newCoord.x >= 40 ||
          newCoord.y >= 40 || grid[newCoord.x][newCoord.y] == 1) {
        continue;
      }

      int newCost = (currentNode.cost + 10);
      // cout << newCost << endl;
      // newCoord in different direction is penalized
      int movementPenalty = CalcPenalty(newCoord, currentNode.loc);
      int movementBenifit = CalcBenefit(newCoord);
      newCost += movementPenalty;
      newCost += movementBenifit;
      if (gCost.find(newCoord) == gCost.end() || newCost < gCost[newCoord]) {
        gCost[newCoord] = newCost;
        int fCost = newCost + CostCalc(newCoord, goal);
        openSet.push(Node(newCoord, newCost, fCost));
        cameFrom[newCoord] = currentCoord;
      }
    }
  }

  // Deallocation of startNode and any other nodes not managed by openSet would
  // be necessary
  return vector<Coord>();
};

Color BlockColor(int i, int j) {
  if (grid[i][j] == 1)
    return RED;
  else if (grid[i][j] >= 2)
    return BLUE;
  else
    return GRAY;
}

int GAP_SIZE = 5;
void drawGrid() {
  for (int i = 0; i < 40; i++) {
    for (int j = 0; j < 40; j++) {
      Color color = BlockColor(i, j);
      DrawRectangle(i * 20, j * 20, 20 - GAP_SIZE, 20 - GAP_SIZE, color);
    }
  }
}

void HandlClicks() {
  Vector2 mousePos = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    grid[(int)(mousePos.x / 20)][(int)(mousePos.y / 20)] = 1;
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    grid[(int)(mousePos.x / 20)][(int)(mousePos.y / 20)] = 4;
  }
}

class Entity {
  Coord start{0, 0};
  Coord goal{39, 39};
  int frame = 0;
  vector<Coord> path;
  int pathIndex = 0;
  vector<pair<int, int>> generate_manhattan_circle(int cx, int cy, int r) {
    vector<pair<int, int>> points;
    for (int x = cx - r; x <= cx + r; x++) {
      // Calculate potential y values for the given x
      int dx = abs(x - cx);
      int dy = r - dx; // Determining the remainder to reach the radius after
                       // accounting for dx

      int y1 = cy + dy;
      int y2 = cy - dy;

      // Adding points to the list (ensure duplicates are not added, if y1 ==
      // y2, add only once)
      points.emplace_back(x, y1);
      if (y1 != y2) {
        points.emplace_back(x, y2);
      }
    }
    return points;
  }

public:
  Entity() {
    start = {GetRandomValue(0, 39), GetRandomValue(0, 39)};
    goal = {GetRandomValue(0, 39), GetRandomValue(0, 39)};
    path = Astar(start, goal);
    reverse(path.begin(), path.end());
    pathIndex = 0;
  }
  void Update() {
    if (pathIndex == path.size()) {
      // When the Entity reaches the end of the path
      // Randomly place Breadcrumbs (grid value ++= 5)
      for (Coord c : path) {
        int probability = GetRandomValue(1, 10);
        if (probability == 1) {
          // Place the pellet randomly in Manhattan circle
        }
      }
      start = goal;
      goal = {GetRandomValue(0, 39), GetRandomValue(0, 39)};
      path = Astar(start, goal);
      reverse(path.begin(), path.end());
      pathIndex = 0;
    }
    if (frame == 5) {
      frame = 0;
      start = path[pathIndex];
      pathIndex++;
    }
    frame++;
  }
  void Draw() {
    Update();
    for (int i = pathIndex; i < path.size(); i++) {
      Coord c = path[i];
      // cout << "(" << c.x << ", " << c.y << ") ";
      DrawRectangle(c.x * 20, c.y * 20, 20 - GAP_SIZE, 20 - GAP_SIZE, WHITE);
    }
    DrawRectangle(start.x * 20, start.y * 20, 20 - GAP_SIZE, 20 - GAP_SIZE,
                  GREEN);
  }
};

int main(int argc, char *argv[]) {
  InitWindow(800, 800, "Raylib");
  memset(grid, 0, sizeof(grid));
  SetTargetFPS(60);
  Entity entity;
  while (!WindowShouldClose()) {
    HandlClicks();
    BeginDrawing();
    ClearBackground(BLACK);
    drawGrid();
    entity.Draw();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}