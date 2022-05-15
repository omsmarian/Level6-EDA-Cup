#include "Algorithm.h"




pair<int, int> Algorithm::actualPositionToHeatMapPosition(vector<float> actualPosition)
{
    vector<int> auxActualPosition(2);


    auxActualPosition[0] = actualPosition[0]*10 + 45;
    auxActualPosition[1] = -(actualPosition[1]*10 - 30);


    if(auxActualPosition[0] == 90)
    {
        auxActualPosition[0] -= 1;
    }
    if(auxActualPosition[1] == 60)
    {
        auxActualPosition[1] -= 1;
    }

    pair<int, int> index;
    index.first = auxActualPosition[0];
    index.second = auxActualPosition[1];

    return index;
}