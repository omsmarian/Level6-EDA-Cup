#ifndef Algorithm_h
#define Algorithm_h

#include <vector>
#include <raylib.h>
#include <math.h>
#include <utility>

using namespace std;

pair<int, int> actualPositionToHeatMapPosition(vector<float> );

class Algorithm 
{
    public:
        pair<int, int> actualPositionToHeatMapPosition(vector<float> );
    private: 

};

#endif