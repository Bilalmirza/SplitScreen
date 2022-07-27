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

    Player1 = Sprite::create("CloseNormal.png");
    this->addChild(Player1);
    Player1->setPosition(winSize.width * 0.25f , winSize.height/2);
    Player1->setCameraMask((int)CameraFlag::USER5);
    Player1->setColor(Color3B::RED);
    Player1->setScale(4.5);
    
    Player2 = Sprite::create("CloseNormal.png");
    this->addChild(Player2);
    Player2->setPosition(winSize.width  * 0.75f , winSize.height/2);
    Player2->setCameraMask((int)CameraFlag::USER5);
    Player2->setScale(4.5);
    
    float fieldOfView = 60;
    Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize() * 2 ;
    
    camera1 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 120000);
    camera1->setCameraFlag(CameraFlag::USER5);
    this->addChild(camera1);
    camera2 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 120000);
    camera2->setCameraFlag(CameraFlag::USER5);
    this->addChild(camera2);
    camera1->setPositionZ(10000);
    camera2->setPositionZ(10000);
    
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
    
    
    camera1->setFrameBufferObject(fbo);
        
        playerScreen1 = Sprite::create();
        playerScreen1->initWithTexture(fbo->getRenderTarget()->getTexture());
        this->addChild(playerScreen1);
        playerScreen1->setScale(frameSize.width/playerScreen1->getContentSize().width,frameSize.height/playerScreen1->getContentSize().height);
        playerScreen1->setTextureRect(Rect(0,0,fboSize.width/2,fboSize.height));
        playerScreen1->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        playerScreen1->setPosition(winSize.width/2,winSize.height/2);
        playerScreen1->setFlippedY(true);

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
    
    
    camera2->setFrameBufferObject(fbo);
        
        
        playerScreen2 = Sprite::create();
        playerScreen2->initWithTexture(fbo->getRenderTarget()->getTexture());
        this->addChild(playerScreen2);
        playerScreen2->setScale(frameSize.width/playerScreen2->getContentSize().width,frameSize.height/playerScreen2->getContentSize().height);
        playerScreen2->setTextureRect(Rect(fboSize.width/2,0,fboSize.width/2,fboSize.height));
        playerScreen2->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        playerScreen2->setPosition(winSize.width/2,winSize.height/2);
        playerScreen2->setFlippedY(true);
//        playerScreen2->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_GRAYSCALE));

    }
 
//    playerScreen1->setColor(Color3B(35,35,35));
//    playerScreen1->setVisible(false);
    
  
//    playerScreen2->setVisible(false);
    
 
    controls();
    this->scheduleUpdate();
    
    
    
    return true;
}



void GameScene::update(float dt)
{

    p1a *= 0.99;
    p1a += movePlayer1;
    
    p2a *= 0.99;
    p2a += movePlayer2;
    Player1->setPosition(Player1->getPosition() + p1a);
    Player2->setPosition(Player2->getPosition() + p2a);
    Player1->setRotation(90 +  CC_RADIANS_TO_DEGREES( atan2(p1a.y,p1a.x)) * -1);
    Player2->setRotation(90 +  CC_RADIANS_TO_DEGREES( atan2(p2a.y,p2a.x)) * -1);
  
    
    const float distanceToSplit = 2250;
    Point pointToLookAt =  Player2->getPosition().getMidpoint(Player1->getPosition());
    float angle = calcAngle(Player1->getPosition(),pointToLookAt);
   
    camera1->setRotation(angle);
    camera2->setRotation(angle);

    playerScreen1->setRotation(angle);
    playerScreen2->setRotation(angle);
    if(Player1->getPosition().distance(pointToLookAt) < distanceToSplit)
    {
     //combine screens
        camera1->setPosition(pointToLookAt);
        camera2->setPosition(pointToLookAt);
    }
    else{
        Point CameraPositionA = Point( pointToLookAt.x + (Player1->getPosition().distance(pointToLookAt) - distanceToSplit) * cosf(CC_DEGREES_TO_RADIANS(camera2->getRotation() )),pointToLookAt.y +(Player1->getPosition().distance(pointToLookAt) - distanceToSplit) * -sinf(CC_DEGREES_TO_RADIANS(camera2->getRotation() )));
        Point CameraPositionB = Point(pointToLookAt.x + (Player2->getPosition().distance(pointToLookAt) - distanceToSplit) * -cosf(CC_DEGREES_TO_RADIANS(camera1->getRotation() )),pointToLookAt.y +(Player2->getPosition().distance(pointToLookAt) - distanceToSplit) * sinf(CC_DEGREES_TO_RADIANS(camera1->getRotation() )));
    camera2->setPosition(CameraPositionA);
    camera1->setPosition(CameraPositionB);
    }

}


void GameScene::controls()
{
    
    float speed = 0.65f;

    auto keylistener = EventListenerKeyboard::create();
    keylistener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event * event){
        if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            movePlayer2.x -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            movePlayer2.x += speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            movePlayer2.y += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            movePlayer2.y -= speed;
            
        }
        
        
        if(keyCode == EventKeyboard::KeyCode::KEY_A)
        {
            movePlayer1.x -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_D)
        {
            movePlayer1.x += speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_W)
        {
            movePlayer1.y += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            movePlayer1.y -= speed;
            
        }
        
    };
    keylistener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event * event){
        if(keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            movePlayer2.x += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            movePlayer2.x -= speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            movePlayer2.y -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            movePlayer2.y += speed;
            
        }
        
        
        if(keyCode == EventKeyboard::KeyCode::KEY_A)
        {
            movePlayer1.x += speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_D)
        {
            movePlayer1.x -= speed;
            
        }
        
        if(keyCode == EventKeyboard::KeyCode::KEY_W)
        {
            movePlayer1.y -= speed;
            
        }
        if(keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            movePlayer1.y += speed;
            
        }
        
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keylistener, this);
    
    
}
