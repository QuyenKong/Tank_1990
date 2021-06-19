
#ifndef AI_H_
#define AI_H_

#include "cocos2d.h"
using namespace cocos2d;

class AI {
private:
	CCSprite* _ai;
	bool g_aiIsMoving = true;
	bool g_playerIsMoving = true;
	int tankSpeed = 2;
	int g_aiDr;
	int collisionBitmask;
	int maskAi;
	int maskBullet;
public:
	//Constructors
	AI(String a, int x, int y, int maskAi, int maskBullet);

};


#endif  /* AI_H_ */
