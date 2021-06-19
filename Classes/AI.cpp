#include "AI.h"
#include <iostream>

AI::AI(String a, int x, int y, int maskAi, int maskBullet) {

    _ai = Sprite::create("images/tank-1-sheet/tank-ss-panther-6-0-1-3.png");
    _ai->setPosition(x, y);
    _ai->setScale(0.4);
    _ai->setRotation(180);
    _ai->setZOrder(9);

    auto aiBody = PhysicsBody::createCircle(_ai->getContentSize().width / 2);
    aiBody->setContactTestBitmask(0x1);
    aiBody->setDynamic(false);
    aiBody->setCollisionBitmask(maskAi);
    aiBody->setContactTestBitmask(true);
    _ai->setPhysicsBody(aiBody);

}
