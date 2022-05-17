#ifndef Algorithm_h
#define Algorithm_h

#include <utility>
#include <cfloat>
#include <cmath>
#include <stdio.h>
#include <stack>
#include <string.h>
#include <set>


using namespace std;

#define ROW 60
#define COL 90

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int> > pPair;

// A structure to hold the necessary parameters
struct cell {
	// Row and Column index of its parent
	// Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	int parent_i, parent_j;
	// f = g + h
	double f, g, h;
};

void aStarSearch(int grid[][COL], Pair src, Pair dest);



#endif