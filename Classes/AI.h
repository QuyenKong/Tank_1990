
#ifndef AI_H_
#define AI_H_

#include "cocos2d.h"
using namespace cocos2d;

class AI {

private:
    int _n = 64; 
    int _m = 48;
   
    int _d;
public:
    int _a[100][100];
    CCSprite* _player;
    void moveToX(Point* x, int k);
    void tryBack(Point* x, int k, Point f);
    void path(Point start, Point finish);
    void loadGrid(int a[100][100]);

};


#endif  /* AI_H_ */
