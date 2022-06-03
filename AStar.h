#ifndef ASTAR_h 
#define ASTAR_h

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <cstdlib>

using namespace std;


struct GridLocation {
  int x, y;
  int weight;
};

struct SquareGrid {
  static array<GridLocation, 8> DIRS;

  int width, height;
  unordered_set<GridLocation> walls;

  SquareGrid(int width_, int height_)
     : width(width_), height(height_) {}

  bool in_bounds(GridLocation id) const {
    return 0 <= id.x && id.x < width
        && 0 <= id.y && id.y < height;
  }

  bool passable(GridLocation id) const {
    return walls.find(id) == walls.end();
  }

  vector<GridLocation> neighbors(GridLocation id) const {
    vector<GridLocation> results;

    for (GridLocation dir : DIRS) {
      GridLocation next{id.x + dir.x, id.y + dir.y};
      if (in_bounds(next) && passable(next)) {
        results.push_back(next);
      }
    }

    if ((id.x + id.y) % 2 == 0) {
      // see "Ugly paths" section for an explanation:
      reverse(results.begin(), results.end());
    }

    return results;
  }
};

struct GridWithWeights: SquareGrid {
  unordered_set<GridLocation> forests;
  GridWithWeights(int w, int h): SquareGrid(w, h) {}
  double cost(GridLocation from_node, GridLocation to_node) {
    return forests.find(to_node) != forests.end()? forests.find(to_node)->weight : 1;
  }
};

namespace std {
/* implement hash function so  can put GridLocation into an unordered_set */
template <> struct hash<GridLocation> {
  size_t operator()(const GridLocation& id) const noexcept {
    // NOTE: better to use something like boost hash_combine
    return hash<int>()(id.x ^ (id.y << 16));
  }
};
}

template<typename Location>
   vector<Location> reconstruct_path(
   Location start, Location goal,
   unordered_map<Location, Location> came_from
);

#endif