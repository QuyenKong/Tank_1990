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

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

#include "Joystick.h"

using namespace cocos2d;
class HelloWorld : public cocos2d::Scene
{
public:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;

    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void setViewPointCenter(CCPoint position);
    
    cocos2d::Point tileCoordForPosition(cocos2d::Point position);

    Sprite* _tile[1000]; // This wizard class is used to load tiles that need to add collision components.
    int _lastTile = 0; // T

    CREATE_FUNC(HelloWorld);
private:
    CCTMXLayer* _block;

    Joystick* joyStick;

    CCTMXTiledMap* _tileMap;

    CCTMXLayer* _background;

    CCTMXLayer* _brick;

    CCTMXLayer* _steel;

    CCTMXLayer* _grass;

    CCTMXLayer* _water;

    CCTMXLayer* _frozen;

    CCTMXLayer* _flag;

    CCSprite* _player;

    CCSprite* _ai;


    bool onContactBegin(const PhysicsContact& contact);

    void onContactSeparate(const PhysicsContact& contact);

    void onTouchEnded(cocos2d::Touch* touches, cocos2d::Event* event);

    void onTouchMoved(cocos2d::Touch* touches, cocos2d::Event* event);

    bool onTouchBegan(cocos2d::Touch* touches, cocos2d::Event* event);
        
    void spriteMovePlayerFinished(Node* sender);

    void spriteAIMoveFinished(Node* sender);

    void setPlayerPosition(CCPoint position);

    void setAIPosition(CCPoint position);

    void aiCheck();

    void update(float dt);

    void updateAI(float dt);


    void updatePlayer(float dt);


    // determine whether the run animation  
    bool IsRunning;
};

#endif // __HELLOWORLD_SCENE_H__
