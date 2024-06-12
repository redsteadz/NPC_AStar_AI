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
  unordered_set<Coord> closedSet;
  Node startNode = Node(start, 0, CostCalc(start, goal));
  openSet.push(startNode);
  gCost[start] = 0;

  while (!openSet.empty()) {
    const Node currentNode = openSet.top();
    Coord currentCoord = currentNode.loc;
    openSet.pop();

    if (currentCoord == goal) {
      // cout << "Found the goal with cost: " << currentNode->cost << endl;
      vector<Coord> path;
      cout << gCost[goal] << endl;
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
          newCoord.y >= 40 || grid[newCoord.x][newCoord.y] == 1 || closedSet.count(newCoord) != 0){
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
    closedSet.insert(currentCoord);
  }
  return vector<Coord>();
};

Color BlockColor(int i, int j) {
  unsigned char val = min(grid[i][j] * 50, 255);
  if (grid[i][j] == 1)
    return RED;
  else if (grid[i][j] >= 2)
    return {0, val, 234, 255};
  else
    return GRAY;
}
int frame = 0;
int GAP_SIZE = 5;
void drawGrid() {
  for (int i = 0; i < 40; i++) {
    for (int j = 0; j < 40; j++) {
      Color color = BlockColor(i, j);
      DrawRectangle(i * 20, j * 20, 20 - GAP_SIZE, 20 - GAP_SIZE, color);
      if (grid[i][j] > 1 && frame == 160) {
        grid[i][j] = max(0, grid[i][j] - 2);
        if (grid[i][j] == 1)
          grid[i][j] = 0;
        // cout << i << " " << j << " = " << grid[i][j] << endl;
      }
    }
  }
  if (frame == 160) frame = 0;
  frame++;
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
      // Check that the points are within the bounds of the grid
      if (x < 0 || x >= 40 || y1 < 0 || y1 >= 40 || y2 < 0 || y2 >= 40) {
        continue;
      }
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
          vector<pair<int, int>> points =
              generate_manhattan_circle(c.x, c.y, 2);
          for (auto p : points) {
            if (p.first < 0 || p.first >= 40 || p.second < 0 ||
                p.second >= 40) {
              continue;
            }
            // Theres a 10% chance that the entity will place the breadcrumb
            // in the point
            if (GetRandomValue(1, 10) <= 1) {
              grid[p.first][p.second] += 5;
            }
          }
        }
      }
      start = goal;
      goal = {GetRandomValue(0, 39), GetRandomValue(0, 39)};
      path = Astar(start, goal);
      reverse(path.begin(), path.end());
      pathIndex = 0;
    }
    if (frame == 10) {
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
      // DrawRectangle(c.x * 20, c.y * 20, 20 - GAP_SIZE, 20 - GAP_SIZE, WHITE);
    }
    DrawRectangle(start.x * 20, start.y * 20, 20 - GAP_SIZE, 20 - GAP_SIZE,
                  GREEN);
  }
};

int main(int argc, char *argv[]) {
  InitWindow(800, 800, "Raylib");
  memset(grid, 0, sizeof(grid));
  SetTargetFPS(60);
  Entity e[20];
  while (!WindowShouldClose()) {
    HandlClicks();
    BeginDrawing();
    ClearBackground(BLACK);
    // drawGrid();
    for (int i = 0; i < 20; i++) e[i].Draw();
    // e[0].Draw();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
