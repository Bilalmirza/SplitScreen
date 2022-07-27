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

#include "GameScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

namespace {
    bool ALLOW_DIVIDER = true;
}
float calcAngle(cocos2d::Point p1, cocos2d::Point p2)
{
    
    cocos2d::Point diff = p2 - p1;
    float rads = atan2f( diff.y, diff.x);
    float degs = CC_RADIANS_TO_DEGREES(rads);
    
    degs = 360 - degs;
    
    
    return (degs );
}
Scene* GameScene::createScene()
{
    return GameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in GameSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
 
    Size winSize = Director::getInstance()->getWinSize();
    Sprite *bg = Sprite::create("res/1624.jpg");
    this->addChild(bg,-1);
    bg->setPosition(winSize/2);
    bg->setScale(3);
    bg->setCameraMask((int)CameraFlag::USER5);

    _player1 = Sprite::create("CloseNormal.png");
    this->addChild(_player1);
    _player1->setPosition(winSize.width * 0.25f , winSize.height/2);
    _player1->setCameraMask((int)CameraFlag::USER5);
    _player1->setColor(Color3B::RED);
    _player1->setScale(4.5);
    
    _player2 = Sprite::create("CloseNormal.png");
    this->addChild(_player2);
    _player2->setPosition(winSize.width  * 0.75f , winSize.height/2);
    _player2->setCameraMask((int)CameraFlag::USER5);
    _player2->setScale(4.5);
    
    float fieldOfView = 60;
    Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize() * 2 ;
    
    _camera1 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 120000);
    _camera1->setCameraFlag(CameraFlag::USER5);
    this->addChild(_camera1);
    _camera2 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 120000);
    _camera2->setCameraFlag(CameraFlag::USER5);
    this->addChild(_camera2);
    _camera1->setPositionZ(10000);
    _camera2->setPositionZ(10000);
    
    auto fboSize = Director::getInstance()->getOpenGLView()->getFrameSize();

    {
    auto fbo = new experimental::FrameBuffer;
    fbo->init(1,fboSize.width,fboSize.height);
    fbo->setClearColor(Color4F(0,0,0,0));
    auto rt = new experimental::RenderTarget;
    rt->init(fboSize.width, fboSize.height, Texture2D::PixelFormat::RGBA8888);
    fbo->attachRenderTarget(rt);
    fbo->getRenderTarget()->getTexture()->setAliasTexParameters();
    rt->release();
    auto rtDS = new experimental::RenderTargetDepthStencil;
    rtDS->init(fboSize.width,fboSize.height);
    fbo->attachDepthStencilTarget(rtDS);
    rtDS->release();
    
    
    _camera1->setFrameBufferObject(fbo);
        
        _playerScreen1 = Sprite::create();
        _playerScreen1->initWithTexture(fbo->getRenderTarget()->getTexture());
        this->addChild(_playerScreen1);
        _playerScreen1->setScale(frameSize.width/_playerScreen1->getContentSize().width,frameSize.height/_playerScreen1->getContentSize().height);
        _playerScreen1->setTextureRect(Rect(0,0,fboSize.width/2,fboSize.height));
        _playerScreen1->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        _playerScreen1->setPosition(winSize.width/2,winSize.height/2);
        _playerScreen1->setFlippedY(true);

    }
    {
    auto fbo = new experimental::FrameBuffer;
    fbo->init(1,fboSize.width,fboSize.height);
    fbo->setClearColor(Color4F(0,0,0,0));
    auto rt = new experimental::RenderTarget;
    rt->init(fboSize.width, fboSize.height, Texture2D::PixelFormat::RGBA8888);
    fbo->attachRenderTarget(rt);
    fbo->getRenderTarget()->getTexture()->setAliasTexParameters();
    rt->release();
    auto rtDS = new experimental::RenderTargetDepthStencil;
    rtDS->init(fboSize.width,fboSize.height);
    fbo->attachDepthStencilTarget(rtDS);
    rtDS->release();
    
    
    _camera2->setFrameBufferObject(fbo);
        
        
        _playerScreen2 = Sprite::create();
        _playerScreen2->initWithTexture(fbo->getRenderTarget()->getTexture());
        this->addChild(_playerScreen2);
        _playerScreen2->setScale(frameSize.width/_playerScreen2->getContentSize().width,frameSize.height/_playerScreen2->getContentSize().height);
        _playerScreen2->setTextureRect(Rect(fboSize.width/2,0,fboSize.width/2,fboSize.height));
        _playerScreen2->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        _playerScreen2->setPosition(winSize.width/2,winSize.height/2);
        _playerScreen2->setFlippedY(true);
//        _playerScreen2->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_GRAYSCALE));

    }

    
    _divider = Sprite::create("HelloWorld.png");
    this->addChild(_divider,100);
    _divider->setScale(0, 1000);
    _divider->setPosition(winSize/2);
    _divider->setColor(Color3B::BLACK);
    
    controls();
    this->scheduleUpdate();
    
    
    
    return true;
}



void GameScene::update(float dt)
{

    p1a *= 0.99;
    p1a += _movePlayer1;
    
    p2a *= 0.99;
    p2a += _movePlayer2;
    _player1->setPosition(_player1->getPosition() + p1a);
    _player2->setPosition(_player2->getPosition() + p2a);
    _player1->setRotation(90 +  CC_RADIANS_TO_DEGREES( atan2(p1a.y,p1a.x)) * -1);
    _player2->setRotation(90 +  CC_RADIANS_TO_DEGREES( atan2(p2a.y,p2a.x)) * -1);
  
    
    const float distanceToSplit = 2250;
    Point pointToLookAt =  _player2->getPosition().getMidpoint(_player1->getPosition());
    float angle = calcAngle(_player1->getPosition(),pointToLookAt);
   
    _camera1->setRotation(angle);
    _camera2->setRotation(angle);

    _playerScreen1->setRotation(angle);
    _playerScreen2->setRotation(angle);
    
    _divider->setRotation(angle);
    float divideWidth = clampf((_player1->getPosition().distance(pointToLookAt) - distanceToSplit)/3000,0,0.25f);
    _divider->setScaleX(divideWidth);
    if(_player1->getPosition().distance(pointToLookAt) < distanceToSplit)
    {
     //combine screens
        _camera1->setPosition(pointToLookAt);
        _camera2->setPosition(pointToLookAt);
    }
    else{
        Point CameraPositionA = Point( pointToLookAt.x + (_player1->getPosition().distance(pointToLookAt) - distanceToSplit) * cosf(CC_DEGREES_TO_RADIANS(_camera2->getRotation() )),pointToLookAt.y +(_player1->getPosition().distance(pointToLookAt) - distanceToSplit) * -sinf(CC_DEGREES_TO_RADIANS(_camera2->getRotation() )));
        Point CameraPositionB = Point(pointToLookAt.x + (_player2->getPosition().distance(pointToLookAt) - distanceToSplit) * -cosf(CC_DEGREES_TO_RADIANS(_camera1->getRotation() )),pointToLookAt.y +(_player2->getPosition().distance(pointToLookAt) - distanceToSplit) * sinf(CC_DEGREES_TO_RADIANS(_camera1->getRotation() )));
//        _camera2->setPosition(_camera2->getPosition().lerp(CameraPositionA,dt * 20000/_player1->getPosition().distance(pointToLookAt)));
        
        _camera2->setPosition(CameraPositionA);
        _camera1->setPosition(CameraPositionB);
    }

}


void GameScene::controls()
{
    
    float speed = 0.65f;

    auto keylistener = EventListenerKeyboard::create();
    keylistener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event * event){
        if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            _movePlayer2.x -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            _movePlayer2.x += speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            _movePlayer2.y += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            _movePlayer2.y -= speed;
            
        }
        
        
        if(keyCode == EventKeyboard::KeyCode::KEY_A)
        {
            _movePlayer1.x -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_D)
        {
            _movePlayer1.x += speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_W)
        {
            _movePlayer1.y += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            _movePlayer1.y -= speed;
            
        }
        
    };
    keylistener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event * event){
        if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            _movePlayer2.x += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            _movePlayer2.x -= speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            _movePlayer2.y -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            _movePlayer2.y += speed;
            
        }
        
        
        if(keyCode == EventKeyboard::KeyCode::KEY_A)
        {
            _movePlayer1.x += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_D)
        {
            _movePlayer1.x -= speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_W)
        {
            _movePlayer1.y -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            _movePlayer1.y += speed;
            
        }
        
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keylistener, this);
    
    
}
