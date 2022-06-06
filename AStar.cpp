/*
 Sample code from https://www.redblobgames.com/pathfinding/a-star/
 Copyright 2014 Red Blob Games <redblobgames@gmail.com>
 
 Feel free to use this code in your own projects, including commercial projects
 License: Apache v2.0 <http://www.apache.org/licenses/LICENSE-2.0.html>
*/

// #include "AStar.h"



// struct SimpleGraph {
//   std::unordered_map<char, std::vector<char> > edges;

//   std::vector<char> neighbors(char id) {
//     return edges[id];
//   }
// };


// std::array<GridLocation, 8> SquareGrid::DIRS = {
//   /* East, West, North, South */
//   GridLocation{1, 0}, GridLocation{-1, 0},
//   GridLocation{0, -1}, GridLocation{0, 1},
//   GridLocation{-1, 1}, GridLocation{1, 1},
//   GridLocation{-1, -1}, GridLocation{1, -1}
// };
// // Helpers for GridLocation

// bool operator == (GridLocation a, GridLocation b) {
//   return a.x == b.x && a.y == b.y;
// }

// bool operator != (GridLocation a, GridLocation b) {
//   return !(a == b);
// }

// bool operator < (GridLocation a, GridLocation b) {
//   return tie(a.x, a.y) < tie(b.x, b.y);
// }

// basic_iostream<char>::basic_ostream& operator<<(basic_iostream<char>::basic_ostream& out, const GridLocation& loc) {
//   out << '(' << loc.x << ',' << loc.y << ')';
//   return out;
// }

// // This outputs a grid. Pass in a distances map if you want to print
// // the distances, or pass in a point_to map if you want to print
// // arrows that point to the parent location, or pass in a path vector
// // if you want to draw the path.
// template<class Graph>
// void draw_grid(const Graph& graph,
//                unordered_map<GridLocation, double>* distances=nullptr,
//                unordered_map<GridLocation, GridLocation>* point_to=nullptr,
//                vector<GridLocation>* path=nullptr,
//                GridLocation* start=nullptr,
//                GridLocation* goal=nullptr) {
//   const int field_width = 3;
//   cout << string(field_width * graph.width, '_') << '\n';
//   for (int y = 0; y != graph.height; ++y) {
//     for (int x = 0; x != graph.width; ++x) {
//       GridLocation id {x, y};
//       if (graph.walls.find(id) != graph.walls.end()) {
//         cout << string(field_width, '#');
//       } else if (start && id == *start) {
//         cout << " A ";
//       } else if (goal && id == *goal) {
//         cout << " Z ";
//       } else if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
//         cout << " @ ";
//       } else if (point_to != nullptr && point_to->count(id)) {
//         GridLocation next = (*point_to)[id];
//         if (next.x == x + 1) { cout << " > "; }
//         else if (next.x == x - 1) { cout << " < "; }
//         else if (next.y == y + 1) { cout << " v "; }
//         else if (next.y == y - 1) { cout << " ^ "; }
//         else { cout << " * "; }
//       } else if (distances != nullptr && distances->count(id)) {
//         cout << ' ' << left << setw(field_width - 1) << (*distances)[id];
//       } else {
//         cout << " . ";
//       }
//     }
//     cout << '\n';
//   }
//   cout << string(field_width * graph.width, '~') << '\n';
// }

// void add_rect(SquareGrid& grid, int x1, int y1, int x2, int y2) {
//   for (int x = x1; x < x2; ++x) {
//     for (int y = y1; y < y2; ++y) {
//       grid.walls.insert(GridLocation{x, y});
//     }
//   }
// }


// template<typename T, typename priority_t>
// struct PriorityQueue {
//   typedef pair<priority_t, T> PQElement;
//   priority_queue<PQElement, vector<PQElement>,
//                  greater<PQElement>> elements;

//   inline bool empty() const {
//      return elements.empty();
//   }

//   inline void put(T item, priority_t priority) {
//     elements.emplace(priority, item);
//   }

//   T get() {
//     T best_item = elements.top().second;
//     elements.pop();
//     return best_item;
//   }
// };

// template<typename Location>
//    vector<Location> reconstruct_path(
//    Location start, Location goal,
//    unordered_map<Location, Location> came_from
// ) {
//   vector<Location> path;
//   Location current = goal;
//   while (current != start) {  // note: this will fail if no path found
//     path.push_back(current);
//     current = came_from[current];
//   }
//   path.push_back(start); // optional
//   reverse(path.begin(), path.end());
//   return path;
// }

// inline double heuristic(GridLocation a, GridLocation b) {
//   return abs(a.x - b.x) + abs(a.y - b.y);
// }

// template<typename Location, typename Graph>
// void a_star_search
//   (Graph graph,
//    Location start,
//    Location goal,
//    unordered_map<Location, Location>& came_from,
//    unordered_map<Location, double>& cost_so_far)
// {
//   PriorityQueue<Location, double> frontier;
//   frontier.put(start, 0);

//   came_from[start] = start;
//   cost_so_far[start] = 0;
  
//   while (!frontier.empty()) {
//     Location current = frontier.get();

//     if (current == goal) {
//       break;
//     }

//     for (Location next : graph.neighbors(current)) {
//       double new_cost = cost_so_far[current] + graph.cost(current, next);
//       if (cost_so_far.find(next) == cost_so_far.end()
//           || new_cost < cost_so_far[next]) {
//         cost_so_far[next] = new_cost;
//         double priority = new_cost + heuristic(next, goal);
//         frontier.put(next, priority);
//         came_from[next] = current;
//       }
//     }
//   }
// }
