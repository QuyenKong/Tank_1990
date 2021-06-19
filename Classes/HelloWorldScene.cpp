/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Joystick.h"
#include <GameOverScene.h>
#include <AI.h>

USING_NS_CC;


#define BULLET_MASK 10
#define BULLET_AI_MASK 20
#define BRICK_MASK 11
#define TANK_MASK 12
#define OBSTACLE_MASK 13
#define GRASS_MASK 14
#define STEEL_MASK 15
#define WATER_MASK 16
#define FROZEN_MASK 17
#define FLAG_MASK 18
#define AI_MASK 19

int g_aiDr;
bool g_playerIsShot = false;
bool g_aiIsShot = false;
bool g_aiIsMoving = true;
bool g_playerIsMoving = true;
int tankSpeed = 2;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();

    scene->getPhysicsWorld()
        ->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); // Used to see the physical engine components of the elf after running
    auto layer = HelloWorld::create();

    scene->addChild(layer);

    // return the scene
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
   
     _visibleSize = Director::getInstance()->getVisibleSize();
     _origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
   

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

     _tileMap = TMXTiledMap::create("Map/FinallMap.tmx");
    // _tileMap->initWithTMXFile("Map/FinallMap2.tmx");
     _background = _tileMap->layerNamed("background");

     _brick = _tileMap->layerNamed("brick");

     _steel = _tileMap->layerNamed("steel");

     _water = _tileMap->layerNamed("water");

     _frozen = _tileMap->layerNamed("frozen");

     _grass = _tileMap->layerNamed("grass");

     _flag = _tileMap->layerNamed("flag");

     //find point PlayerSpawn and  setting Player 
     TMXObjectGroup* objects = _tileMap->getObjectGroup("PlayerSpawn");
     CCASSERT(NULL != objects, "'Object-Player' object group not found");
     auto spawnPoint = objects->getObject("SpawnPoint");
     CCASSERT(!spawnPoint.empty(), "PlayerShowUpPoint object not found");

     int x = spawnPoint["x"].asInt();
     int y = spawnPoint["y"].asInt();

     _player = Sprite::create("images/tank-1-sheet/tank-ss-panther-6-0-1-0.png");
     _player->setPosition(x + _tileMap->getTileSize().width / 2, y + _tileMap->getTileSize().height / 2);
     _player->setScale(0.4);
     _player->setZOrder(9);

     auto playerBody = PhysicsBody::createCircle(_player->getContentSize().width / 2);
     playerBody->setContactTestBitmask(0x1);
     playerBody->setDynamic(false);
     playerBody->setCollisionBitmask(TANK_MASK);
     playerBody->setContactTestBitmask(true);
     _player->setPhysicsBody(playerBody);

     _tileMap->addChild(_player);
     setViewPointCenter(_player->getPosition());


     //find point AISpawn and setting AI
     TMXObjectGroup* objects1 = _tileMap->getObjectGroup("AISpawn");
     CCASSERT(NULL != objects1, "'Object-Player' object group not found");
     auto spawnPoint1 = objects1->getObject("SpawnAIPoint");
     CCASSERT(!spawnPoint1.empty(), "PlayerShowUpPoint object not found");


     int x1 = spawnPoint1["x"].asInt();
     int y1 = spawnPoint1["y"].asInt();
     

     _ai = Sprite::create("images/tank-1-sheet/tank-ss-panther-6-0-1-3.png");
     _ai->setPosition(x1 + _tileMap->getTileSize().width / 2, y1 + _tileMap->getTileSize().height / 2);
     _ai->setScale(0.4);
     _ai->setRotation(180);
     _ai->setZOrder(9);

     auto aiBody = PhysicsBody::createCircle(_ai->getContentSize().width / 2);
     aiBody->setContactTestBitmask(0x1);
     aiBody->setDynamic(false);
     aiBody->setCollisionBitmask(AI_MASK);
     aiBody->setContactTestBitmask(true);
     _ai->setPhysicsBody(aiBody);

     _tileMap->addChild(_ai);
     setViewPointCenter(_ai->getPosition());

     this->addChild(_tileMap);


    


     //add physic for brick
     this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), CallFunc::create([=]() {
         
   //      _brick = _tileMap->layerNamed("brick"); // Get the layers where you need to add PhysicsBody's tiles

         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _brick->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                    
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(BRICK_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }
         
         })));

     //add physic for grass

     this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), CallFunc::create([=]() {

       //  _grass = _tileMap->layerNamed("grass"); // Get the layers where you need to add PhysicsBody's tiles

         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _grass->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(GRASS_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }

         })));

     //add physic for steel

     this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), CallFunc::create([=]() {

       //  _steel = _tileMap->layerNamed("steel"); // Get the layers where you need to add PhysicsBody's tiles
         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _steel->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                    // a[i][j] = 1;//set a[i][j] = 1 để tìm đường đi tránh những điểm là 1 trong mảng
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(STEEL_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }

         })));
     //add physic for water

     this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), CallFunc::create([=]() {

         //_water = _tileMap->layerNamed("water"); // Get the layers where you need to add PhysicsBody's tiles

         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _water->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                   //  a[i][j] = 1;//set a[i][j] = 1 để tìm đường đi tránh những điểm là 1 trong mảng
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(WATER_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }

         })));
     //add physic for frozen

     this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), CallFunc::create([=]() {

        // _frozen = _tileMap->layerNamed("frozen"); // Get the layers where you need to add PhysicsBody's tiles

         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _frozen->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(FROZEN_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }

         })));
     //add physic for flag

     this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), CallFunc::create([=]() {

         // _flag = _tileMap->layerNamed("flag"); // Get the layers where you need to add PhysicsBody's tiles

         for (int i = 0; i < 64; i++)
         {
             for (int j = 0; j < 48; j++) // tile map size 40X40, starting from 0, this loop traverses all tiles
             {

                 _tile[_lastTile] = _flag->tileAt(ccp(i, j));
                 if (_tile[_lastTile]) // is not all positions, if there is no tile, it is empty
                 {
                     auto boxPhysicsBody = PhysicsBody::createBox(_tile[_lastTile]->getContentSize()); // Setting PhysicsBody components
                     boxPhysicsBody->setDynamic(false);
                     boxPhysicsBody->setCollisionBitmask(FLAG_MASK);
                     boxPhysicsBody->setContactTestBitmask(true);
                     boxPhysicsBody->setContactTestBitmask(0x1);
                     _tile[_lastTile]->setPhysicsBody(boxPhysicsBody); // Add a PhysicsBody component to the tile
                     _lastTile++;
                 }
             }
         }

         })));

     
     //add physic for 4 edge
     auto edgeBody = PhysicsBody::createEdgeBox(_visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
     edgeBody->setCollisionBitmask(OBSTACLE_MASK);
     edgeBody->setContactTestBitmask(true);



     auto edgeNode = Node::create();
     edgeNode->setPosition(Point(_visibleSize.width / 2 + _origin.x, _visibleSize.height / 2 + _origin.y));

     edgeNode->setPhysicsBody(edgeBody);

     this->addChild(edgeNode);

  

     
    //create explorAnimation
    auto explorAnimation = Animation::create();
    explorAnimation->setDelayPerUnit(0.03f);
    explorAnimation->setLoops(1);

    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-1.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-2.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-3.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-4.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-5.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-6.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-7.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-8.png")->getSpriteFrame());
    explorAnimation->addSpriteFrame(Sprite::create("images/explosion-sheet/explosion-9.png")->getSpriteFrame());
    

    const std::string k_explorAnimationName = "explorAnimation";

    AnimationCache::getInstance()->addAnimation(explorAnimation, k_explorAnimationName);


    //create bullet animation
    auto bulletAnimation = Animation::create();
    bulletAnimation->setDelayPerUnit(0.15f);
    bulletAnimation->setLoops(1);

    bulletAnimation->addSpriteFrame(Sprite::create("images/bullet-sheet/bullet-boss-2-1.png")->getSpriteFrame());
    bulletAnimation->addSpriteFrame(Sprite::create("images/bullet-sheet/bullet-boss-2-2.png")->getSpriteFrame());
    bulletAnimation->addSpriteFrame(Sprite::create("images/bullet-sheet/bullet-boss-2-3.png")->getSpriteFrame());

    const std::string k_bulletAnimationnName = "bulletAnimation";

    AnimationCache::getInstance()->addAnimation(bulletAnimation, k_bulletAnimationnName);

  

    //Tạo đối tượng truyền tải thông tin của các sự kiện
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    //Tạo 1 đối tượng lắng nghe sự kiện Chạm màn hình theo cách One by One, xử lý 1 chạm tại 1 thời điểm
    auto listener1 = EventListenerTouchOneByOne::create();
    //Thiết lập "nuốt" sự kiện Touch khi xảy ra, ngăn ko cho các đối tượng Bắt sự kiện khác sử dụng event này
    listener1->setSwallowTouches(true);

    //Bắt sự kiện Touch, khi xảy ra sự kiện Touch nào thì sẽ gọi đến hàm tương ứng của lớp HelloWorld
    listener1->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener1->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener1->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);

    //Gửi cho dispatcher xử lý
    dispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
  

    joyStick = Joystick::create();
    this->addChild(joyStick, 100);
    
    auto _listener = EventListenerCustom::create(JoystickEvent::EVENT_JOYSTICK, [=](EventCustom* event) {
        JoystickEvent* jsevent = static_cast<JoystickEvent*>(event->getUserData());

        auto angle = jsevent->mAnagle;
        if (angle < 0) angle = 360 + angle;

        log("joystick event, %p,  angle=%f", jsevent, angle);


        if ((angle >= 0 && angle <= 30) || (angle >= 330 && angle <= 360))
        {
            _player->setRotation(90);
            joyStick->_dr = 1;
            cocos2d::log("move_left");
        }
        else if (angle >= 30 && angle <= 65)
        {
            cocos2d::log("move_up_left");
        }
        else if (angle >= 65 && angle <= 115)
        {
            _player->setRotation(0);
            joyStick->_dr = 2;
            cocos2d::log("move_up");
        }
        else if (angle >= 115 && angle <= 150)
        {
            cocos2d::log("move_up_right");
        }
        else if ((angle >= 150 && angle <= 210))
        {

            _player->setRotation(-90);
            cocos2d::log("move_right %d");
            joyStick->_dr = 3;
        }
        else if ((angle >= 210 && angle <= 245))
        {
            cocos2d::log("move_down_right");
        }
        else if ((angle >= 245 && angle <= 295))
        {
            joyStick->_dr = 4;
            _player->setRotation(180);
            cocos2d::log("move_down %f",_player->getRotation());
        }
        else if ((angle >= 295 && angle <= 330))
        {
            cocos2d::log("move_down_left");
        }
        });
  

    _eventDispatcher->addEventListenerWithFixedPriority(_listener, 1);


    //collision event
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    contactListener->onContactSeparate= CC_CALLBACK_1(HelloWorld::onContactSeparate, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


  // this->scheduleUpdate();
  
   _player->schedule(CC_CALLBACK_1(HelloWorld::updatePlayer, this), "player");
   _ai->schedule(CC_CALLBACK_1(HelloWorld::updateAI,this), "ai");
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event)

{

    return true; 

}



void HelloWorld::onTouchMoved(Touch* touch, Event* event)
{


}

void HelloWorld::onTouchEnded(Touch* touches, Event* event) {
     

    if (g_playerIsShot) {

        return;
    }

    g_playerIsShot = true;

    // Lấy tọa độ của điểm chạm
    
    Point location = touches->getLocationInView();
    
    location = Director::getInstance()->convertToGL(location);

    Size winSize = Director::getInstance()->getWinSize();

    //Tạo viên đạn là 1 Sprite, đặt vị trí đầu tiên gần nhân vật chính
    auto bullet = Sprite::create("images/bullet-sheet/bullet-boss-2-1.png");
    bullet->setScale(0.6f);
    Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("bulletAnimation"));
    //UP
    if(_player->getRotation() ==0){
    bullet->setPosition(Vec2( _player->getPosition().x, _player->getPosition().y + 20));
    this->addChild(bullet, 1);

        bullet->runAction(Sequence::create(
            animate,
            MoveBy::create(2, Point(0, _visibleSize.height)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMovePlayerFinished, this)), NULL));
            
        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setDynamic(false);
        projectileBody->setContactTestBitmask(0x1);
        bullet->setPhysicsBody(projectileBody);
    
    }
    //Down
    if (_player->getRotation() == 180) {

        bullet->setPosition(Vec2(_player->getPosition().x , _player->getPosition().y-20));
        this->addChild(bullet, 1);

        bullet->setRotation(180);
        bullet->runAction(Sequence::create(
            animate,
            MoveBy::create(2, Point(0, -_visibleSize.height)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMovePlayerFinished, this)), NULL));

        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setDynamic(false);
        projectileBody->setContactTestBitmask(0x1);
        bullet->setPhysicsBody(projectileBody);
    }

    //Right
    if (_player->getRotation() == 90) {

        bullet->setPosition(Vec2(_player->getPosition().x+20, _player->getPosition().y ));
        this->addChild(bullet, 1);

        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setContactTestBitmask(0x1);
        projectileBody->setDynamic(false);
        projectileBody->setRotationEnable(true);
        bullet->setPhysicsBody(projectileBody);

        bullet->setRotation(90);
        bullet->runAction(Sequence::create(
            animate,
            MoveBy::create(2, Point(_visibleSize.width,0)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMovePlayerFinished, this)), NULL));
        
    }
    //Left
    if (_player->getRotation() == -90) {

        bullet->setPosition(Vec2(_player->getPosition().x - 20, _player->getPosition().y));
        this->addChild(bullet, 1);


        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setContactTestBitmask(0x1);
        projectileBody->setDynamic(false);
        bullet->setPhysicsBody(projectileBody);

        bullet->setRotation(-90);
        bullet->runAction(Sequence::create(
            animate,
            MoveBy::create(2, Point(-_visibleSize.width, 0)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteMovePlayerFinished, this)), NULL));
    }
    
   

    //setPosition player with SpawnObject
   // this->setViewPointCenter(_player->getPosition());

}
void HelloWorld::spriteMovePlayerFinished(Node* sender)
{
    g_playerIsShot = false;
    // Ép kiểu Contrỏ Sprite của 1 Node*
    auto sprite = (Sprite*)sender;
    
    this->removeChild(sprite, true);
}

void HelloWorld::spriteAIMoveFinished(Node* sender)
{
    g_aiIsShot = false;
    // Ép kiểu Contrỏ Sprite của 1 Node*
    auto sprite = (Sprite*)sender;
    
    this->removeChild(sprite, true);
}

void HelloWorld::update(float dt)
{
    // moving player 
    switch (joyStick->_dr)
    {
    case 1:
        setPlayerPosition(ccp(_player->getPosition().x + tankSpeed, _player->getPosition().y)); // right away  
        break;
    case  2:
        setPlayerPosition(ccp(_player->getPosition().x, _player->getPosition().y + tankSpeed));   // go up  
        break;
    case 3:
        setPlayerPosition(ccp(_player->getPosition().x - tankSpeed, _player->getPosition().y));   //turn left  
        break;
    case 4:
        setPlayerPosition(ccp(_player->getPosition().x, _player->getPosition().y - tankSpeed));   // go down  
        break;
    default:
        joyStick->_dr = -1;
        break;

    }
    //moving ai 
    if (g_aiDr == 1) {
        _ai->setRotation(-90);

    }
    if (g_aiIsMoving == true) {
        if ((int)_ai->getRotation() == 180) { 
            setAIPosition(ccp(_ai->getPosition().x, _ai->getPosition().y - tankSpeed)); // go down 
        }
        if ((int)_ai->getRotation() == -90) {
            setAIPosition(ccp(_ai->getPosition().x - tankSpeed, _ai->getPosition().y));
        }

    }
    else {
        this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create([=] {
            aiCheck();}), NULL));
       // g_aiIsShot = false;
  
    }

}

void HelloWorld::setViewPointCenter(CCPoint position) {

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    int x = MAX(position.x, winSize.width / 2);
    int y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (_tileMap->getMapSize().width * this->_tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
    CCPoint actualPosition = ccp(x, y);

    CCPoint centerOfView = ccp(winSize.width / 2, winSize.height / 2);
    CCPoint viewPoint = ccpSub(centerOfView, actualPosition);
    this->setPosition(viewPoint);
}
void HelloWorld::setPlayerPosition(Point position)
{

    if (!g_playerIsMoving) {
        return;
    }
    _player->setPosition(position);
}
void HelloWorld::setAIPosition(Point position)
{

    if (!g_aiIsMoving) {
        return;
    }
    _ai->setPosition(position);
}

Point HelloWorld::tileCoordForPosition(Point position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return Point(x, y);
}

bool HelloWorld::onContactBegin(const PhysicsContact& contact) {
    PhysicsBody* a = contact.getShapeA()->getBody();
    //Lấy giá trị cờ để xét xem đối tượng nào ( đạn, quái, hay nhân vật)


    //Lấy đối tượng va chạm thứ hai, ép kiểu con trỏ Sprite*
    PhysicsBody* b = contact.getShapeB()->getBody();
    //Lấy giá trị cờ để xét xem đối tượng nào ( đạn, quái, hay nhân vật)

    //BULLET contact with BRICK
    if (a->getCollisionBitmask() == BULLET_MASK & b->getCollisionBitmask() == BRICK_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor,2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));
       
        this->removeChild(a->getNode(), true);

        b->getNode()->removeFromParent();

        g_playerIsShot = false;
    }
    //BRICK contact with BULLET
    if (a->getCollisionBitmask() == BRICK_MASK & b->getCollisionBitmask() == BULLET_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));

        this->removeChild(b->getNode(), true);
        a->getNode()->removeFromParent();

        g_playerIsShot = false;
    }
  
    //BULLET contact with STELL
    if (a->getCollisionBitmask() == STEEL_MASK & b->getCollisionBitmask() == BULLET_MASK ||
        a->getCollisionBitmask() == BULLET_MASK & b->getCollisionBitmask() == STEEL_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));
        this->removeChild(a->getNode(), true);
        g_playerIsShot = false;
    }
    //BULLET contact with FLAG

    if (a->getCollisionBitmask() == FLAG_MASK & b->getCollisionBitmask() == BULLET_MASK ||
        a->getCollisionBitmask() == BULLET_MASK & b->getCollisionBitmask() == FLAG_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));
        this->removeChild(a->getNode(), true);

        g_playerIsShot = false;

        //load game over scene
    }
   
    //TANK contact with BRICK
    if ((a->getCollisionBitmask() == BRICK_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == BRICK_MASK))
    {
       // Point p = contact.getContactData()->points[0];
        g_playerIsMoving = false;
        if(_player->getRotation()==0){ _player->setPosition((_player->getPosition() + Vec2(0.0f,-tankSpeed))); }
        if(_player->getRotation()==-90){ _player->setPosition((_player->getPosition() + Vec2(tankSpeed,0.0f))); }
        if(_player->getRotation()==90){ _player->setPosition((_player->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if(_player->getRotation()==180){ _player->setPosition((_player->getPosition() + Vec2(0.f, tankSpeed))); }

    }
    
    //TANK contact with 4 EDGE
    if ((a->getCollisionBitmask() == OBSTACLE_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == OBSTACLE_MASK))
    {
        // Point p = contact.getContactData()->points[0];
        g_playerIsMoving = false;
        if (_player->getRotation() == 0) { _player->setPosition((_player->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_player->getRotation() == -90) { _player->setPosition((_player->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_player->getRotation() == 90) { _player->setPosition((_player->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_player->getRotation() == 180) { _player->setPosition((_player->getPosition() + Vec2(0.f, tankSpeed))); }

    }
 
    //TANK contact with STEEL
    if ((a->getCollisionBitmask() == STEEL_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == STEEL_MASK))
    {
        g_playerIsMoving = false;
        if (_player->getRotation() == 0) { _player->setPosition((_player->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_player->getRotation() == -90) { _player->setPosition((_player->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_player->getRotation() == 90) { _player->setPosition((_player->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_player->getRotation() == 180) { _player->setPosition((_player->getPosition() + Vec2(0.f, tankSpeed))); }

    }
   
    
    //TANK contact with GRASS
    if ((a->getCollisionBitmask() == GRASS_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == GRASS_MASK))
    {
        _grass->setOpacity(0.4);
        _grass->setZOrder(10);
        _player->setZOrder(9);
    }
   
    //TANK contact with WATER
    if ((a->getCollisionBitmask() == WATER_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == WATER_MASK))
    {
        g_playerIsMoving = false;
        if (_player->getRotation() == 0) { _player->setPosition((_player->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_player->getRotation() == -90) { _player->setPosition((_player->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_player->getRotation() == 90) { _player->setPosition((_player->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_player->getRotation() == 180) { _player->setPosition((_player->getPosition() + Vec2(0.f, tankSpeed))); }


    }
    
    //TANK contact with FROZEN
    if ((a->getCollisionBitmask() == FROZEN_MASK & b->getCollisionBitmask() == TANK_MASK) ||
        (a->getCollisionBitmask() == TANK_MASK & b->getCollisionBitmask() == FROZEN_MASK))
    {
        tankSpeed =1;
    }
    /*-----------------------------------------------------------------------------------------------------------------*/

      //BRICK contact with BULLET_AI
    if (a->getCollisionBitmask() == BRICK_MASK & b->getCollisionBitmask() == BULLET_AI_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));

        this->removeChild(b->getNode(), true);
        a->getNode()->removeFromParent();


        this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create([=] {
            g_aiIsShot = false; }), NULL));
    }

    //BULLET_AI  contact with BRICK
    if (a->getCollisionBitmask() == BULLET_AI_MASK & b->getCollisionBitmask() == BRICK_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));

        this->removeChild(a->getNode(), true);

        b->getNode()->removeFromParent();

        this->runAction(Sequence::create(DelayTime::create(2), CallFunc::create([=] {
            g_aiIsShot = false; }), NULL));
    }
     //BULLET_AI contact with FLAG
    if (a->getCollisionBitmask() == FLAG_MASK & b->getCollisionBitmask() == BULLET_AI_MASK ||
        a->getCollisionBitmask() == BULLET_AI_MASK & b->getCollisionBitmask() == FLAG_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));
        this->removeChild(a->getNode(), true);

        //  g_isFinish = false;
        auto scene = GameOverScene::createScene(0);
        Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
        //load game over scene
    }
    //BULLET contact with AI
    if (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == BULLET_MASK ||
        a->getCollisionBitmask() == BULLET_MASK & b->getCollisionBitmask() == AI_MASK)
    {
        Point p = contact.getContactData()->points[0];

        auto bulletExplor = Sprite::create("images/explosion-sheet/explosion-1.png");
        Animate* animateExplor = Animate::create(AnimationCache::getInstance()->getAnimation("explorAnimation"));
        bulletExplor->setScale(0.2);
        bulletExplor->setPosition(p);
        this->addChild(bulletExplor, 2);
        bulletExplor->runAction(Sequence::createWithTwoActions(animateExplor, RemoveSelf::create()));

        this->removeChild(b->getNode(), true);
        a->getNode()->removeFromParent();

        auto scene = GameOverScene::createScene(1);
        Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
        g_playerIsShot = false;
    }

    //AI contact with BRICK
    if ((a->getCollisionBitmask() == BRICK_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == BRICK_MASK))
    {
        g_aiIsMoving = false;
        if (_ai->getRotation() == 0) { _ai->setPosition((_ai->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_ai->getRotation() == -90) { _ai->setPosition((_ai->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 90) { _ai->setPosition((_ai->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 180) { _ai->setPosition((_ai->getPosition() + Vec2(0.f, tankSpeed))); }

    }

    //AI contact with FLAG
    if ((a->getCollisionBitmask() == FLAG_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == FLAG_MASK))
    {
        g_aiIsMoving = false;
        if (_ai->getRotation() == 0) { _ai->setPosition((_ai->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_ai->getRotation() == -90) { _ai->setPosition((_ai->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 90) { _ai->setPosition((_ai->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 180) { _ai->setPosition((_ai->getPosition() + Vec2(0.f, tankSpeed))); }

    }
       //AI contact with 4 EDGE
    if ((a->getCollisionBitmask() == OBSTACLE_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == OBSTACLE_MASK))
    {
        // Point p = contact.getContactData()->points[0];
        g_aiDr = 1;
        //  g_aiIsShot = true;
        g_aiIsMoving = false;
        if (_ai->getRotation() == 0) { _ai->setPosition((_ai->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_ai->getRotation() == -90) { _ai->setPosition((_ai->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 90) { _ai->setPosition((_ai->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 180) { _ai->setPosition((_ai->getPosition() + Vec2(0.f, tankSpeed))); }

    }
     //AI contact with STEEL
    if ((a->getCollisionBitmask() == STEEL_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == STEEL_MASK))
    {
        g_aiIsMoving = false;
        if (_ai->getRotation() == 0) { _ai->setPosition((_ai->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_ai->getRotation() == -90) { _ai->setPosition((_ai->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 90) { _ai->setPosition((_ai->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 180) { _ai->setPosition((_ai->getPosition() + Vec2(0.f, tankSpeed))); }

    }
     //AI contact with GRASS
    if ((a->getCollisionBitmask() == GRASS_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == GRASS_MASK))
    {
        _grass->setOpacity(0.4);
        _grass->setZOrder(10);
        _ai->setZOrder(9);
    }

   //AI contact with WATER
    if ((a->getCollisionBitmask() == WATER_MASK & b->getCollisionBitmask() == AI_MASK) ||
        (a->getCollisionBitmask() == AI_MASK & b->getCollisionBitmask() == WATER_MASK))
    {
        g_aiIsMoving = false;
        if (_ai->getRotation() == 0) { _ai->setPosition((_ai->getPosition() + Vec2(0.0f, -tankSpeed))); }
        if (_ai->getRotation() == -90) { _ai->setPosition((_ai->getPosition() + Vec2(tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 90) { _ai->setPosition((_ai->getPosition() + Vec2(-tankSpeed, 0.0f))); }
        if (_ai->getRotation() == 180) { _ai->setPosition((_ai->getPosition() + Vec2(0.f, tankSpeed))); }


    }
    return true;
}
void HelloWorld::onContactSeparate(const PhysicsContact& contact) {

    PhysicsBody* a = contact.getShapeA()->getBody();
    PhysicsBody* b = contact.getShapeB()->getBody();
    if (a->getCollisionBitmask() == TANK_MASK )
    {
        g_playerIsMoving = true;
    }
    if (a->getCollisionBitmask() == AI_MASK)
    {
        g_aiIsMoving = true;
    }
    if (b->getCollisionBitmask() == TANK_MASK)
    {
        g_playerIsMoving = true;
    }
    if (b->getCollisionBitmask() == AI_MASK)
    {
        g_aiIsMoving = true;
    }
    tankSpeed = 2;
}
void HelloWorld::aiCheck() {

   
    if ((int)_ai->getRotation() == 180) {

        if (g_aiIsShot) {

            return;
        }
        auto bullet = Sprite::create("images/bullet-sheet/bullet-boss-2-1.png");
        bullet->setScale(0.6f);
        Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("bulletAnimation"));
        //Tạo viên đạn là 1 Sprite, đặt vị trí đầu tiên gần nhân vật chính
        bullet->setPosition(Vec2(_ai->getPosition().x, _ai->getPosition().y - 20));
        this->addChild(bullet, 1);

        bullet->setRotation(180);
        bullet->runAction(Sequence::create(

            animate,
            MoveBy::create(2, Point(0, -_visibleSize.height)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteAIMoveFinished, this)),
            NULL));

        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_AI_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setDynamic(false);
        projectileBody->setContactTestBitmask(0x1);
        bullet->setPhysicsBody(projectileBody);
        g_aiIsMoving = true;
        g_aiIsShot = true;
        //this->setViewPointCenter(_ai->getPosition());
        //  g_aiIsShot = false;
     //   g_aiIsMoving = true;
    }
    if ((int)_ai->getRotation() == -90) {
        
        if (g_aiIsShot) {

            return;
        }
       
        auto bullet = Sprite::create("images/bullet-sheet/bullet-boss-2-1.png");
        bullet->setScale(0.6f);
        Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation("bulletAnimation"));

        bullet->setPosition(Vec2(_ai->getPosition().x - 20, _ai->getPosition().y));
        this->addChild(bullet, 1);


        auto projectileBody = PhysicsBody::createBox(bullet->getContentSize());
        projectileBody->setCollisionBitmask(BULLET_AI_MASK);
        projectileBody->setContactTestBitmask(true);
        projectileBody->setContactTestBitmask(0x1);
        projectileBody->setDynamic(false);
        bullet->setPhysicsBody(projectileBody);

        bullet->setRotation(-90);
        bullet->runAction(Sequence::create(
            animate,
            MoveBy::create(2, Point(-_visibleSize.width, 0)),
            CallFuncN::create(CC_CALLBACK_1(HelloWorld::spriteAIMoveFinished, this)), NULL));
        g_aiIsShot = true;
        g_aiIsMoving = true;
      
      //  g_aiIsMoving = true;
    //    g_aiIsShot = true;
 
   }
}

void HelloWorld::updatePlayer(float dt) {
    switch (joyStick->_dr)
    {
    case 1:
        setPlayerPosition(ccp(_player->getPosition().x + tankSpeed, _player->getPosition().y)); // right away  
        break;
    case  2:
        setPlayerPosition(ccp(_player->getPosition().x, _player->getPosition().y + tankSpeed));   // go up  
        break;
    case 3:
        setPlayerPosition(ccp(_player->getPosition().x - tankSpeed, _player->getPosition().y));   //turn left  
        break;
    case 4:
        setPlayerPosition(ccp(_player->getPosition().x, _player->getPosition().y - tankSpeed));   // go down  
        break;
    default:
        joyStick->_dr = -1;
        break;
    }
}

    void HelloWorld::updateAI(float dt) {
        //moving ai 
        if (g_aiDr == 1) {
            _ai->setRotation(-90);

        }
        if (g_aiIsMoving == true) {
            if ((int)_ai->getRotation() == 180) {
                setAIPosition(ccp(_ai->getPosition().x, _ai->getPosition().y - tankSpeed)); // go down 
            }
            if ((int)_ai->getRotation() == -90) {
                setAIPosition(ccp(_ai->getPosition().x - tankSpeed, _ai->getPosition().y));
            }
            
        }
        else {
            _ai->runAction(Sequence::create(DelayTime::create(2), CallFunc::create([=] {
                aiCheck();}), NULL));
            // g_aiIsShot = false;

        }
   }