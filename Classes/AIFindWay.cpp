#include "AIFindWay.h"
#include <iostream>


void AIFindWay::path(Point start, Point finish) {
	
	Point* x = new Point[_n * _m + 5];
	x[1] = start;
	int a = start.x;
	int b = start.y;
	_a[(int)start.x][(int)start.y] = 1;
	tryBack(x, 1, finish);

	
}

void AIFindWay::tryBack(Point* x, int k, Point finish) {
	int hh[] = { 0, -1,  0,  1 };
	int hc[] = { 1,  0, -1,  0 };
	if (x[k] == finish) moveToX(x, k);
	else
	{
		for( int t =0 ;t<=3;t++)
		{
			x[k + 1].x = x[k].x + hh[t];
			x[k + 1].y = x[k].y + hc[t];
			if (_a[(int) x[k + 1].x][(int) x[k + 1].y] == 0)
			{
				_a[(int) x[k + 1].x][(int) x[k + 1].y] = 1; //move
				tryBack(x, k + 1, finish);
				_a[(int) x[k + 1].x][(int) x[k + 1].y] = 0; //back
			}
		}
	}
}

void AIFindWay::moveToX(Point* x, int k) {
	for (int i = 1;i <= k;i++) {
		std::cout << "(" << x[i].x << "," << x[i].y << ")->";
	}

}
void AIFindWay::loadGrid(int a[100][100]) {

	for (int i = 1;i <= _n;i++) {
		for (int j = 1;j <= _m;j++) {
			_a[i][j] = a[i][j];

		}
	}
	for (int i = 1;i < _n;i++) {
		_a[i][0] = _a[i][_m + 1] = 1;
	}

	for (int j = 1;j < _m;j++) {
		_a[0][j] = _a[_n + 1][j] = 1;
	}
}
