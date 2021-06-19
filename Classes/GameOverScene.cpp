#include "GameOverScene.h"

USING_NS_CC;

int result;

Scene* GameOverScene::createScene(int status)
{

    result = status;
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameOverScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    if (result == 1) {//win
        auto backgroundSprite = Sprite::create("images/common-sheet/win.jpg");
        backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        backgroundSprite->setScale(1.2);

        this->addChild(backgroundSprite);

    }
    if (result == 0) {//lose
        auto backgroundSprite = Sprite::create("images/common-sheet/lose.jpg");
        backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(backgroundSprite);
    }
   


    return true;
}

