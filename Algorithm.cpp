


float angleCalculator(vector<float> start,vector<float> finish)
{
	float cateto1 = finish[0] - start[0];
	float cateto2 = finish[2] - start[2];
	float angle = atanf(cateto1 / cateto2) * 180 / PI;

	return angle;
}


pair<int, int> actualPositionToHeatMapPosition(vector<float> actualPosition)
{
	vector<int> auxActualPosition(2);

	auxActualPosition[0] = (actualPosition[0] + 4.5) * 10;
	auxActualPosition[1] = -(actualPosition[1] - 3) * 10;

	if(auxActualPosition[0] == 90)
	{
		auxActualPosition[0] -= 1;
	}
	else if(auxActualPosition[1] == 60)
	{
		auxActualPosition[1] -= 1;
	}

	pair<int, int> index;
	index.first = auxActualPosition[0];
	index.second = auxActualPosition[1];

	return index;
}