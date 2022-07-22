# SplitScreen
Voronoi Split Screen 
A split-screen is generally used when you have multiple primary focus points. However, a basic split-screen doesn’t provide everything we need. For 2 players battling each other in planes, it wouldn’t provide all the important information we need. What we want to do is split the screen only when it is necessary. So basically when players are together they are going to share a screen and when they move away from each other the screen should split dynamically in the direction of the other player.


Firstly make sure you have a screen size that you want when everything fits. Keep in mind that you would need the distance between the two focus points, in this case, the 2 planes.

In cocos2d-x:
```
update()
{
float  distance = Player1->getPosition().distance(Player2->getPosition());
}
```

![alt text for screen readers](https://miro.medium.com/max/1400/0*ho3PLv19pLySMKzu)

You would also want to take the midpoint between the 2. This is the point you’d want your camera to look at but that will be covered later on.
```
update()
{
Point midPoint = Player1->getPosition().getMidPoint(Player2->getPosition());
}
```

![alt text for screen readers](https://miro.medium.com/max/1400/0*iziEMUkOz6LSjbnU)

You want the View to be split at the midpoint and perpendicular to the line between the two focus points.
![alt text for screen readers](https://miro.medium.com/max/1400/0*ycYI2ixZ4yNf7s_v)

If the distance between the points is greater than the view you’d want the split-screen to kick in.
![alt text for screen readers](https://miro.medium.com/max/1400/0*M-8av-fRrgl0vqbD)

Let's get into the technical stuff. In cocos2d-x a simple way to achieve this effect would be to use frame buffers and render the camera into a sprite.

First things first. Initialize 2 cameras and assign the same cameraFlag to both
```
camera1 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 12000);
camera1->setCameraFlag(CameraFlag::USER5);
camera2 = Camera::createPerspective(fieldOfView, (GLfloat)frameSize.width/frameSize.height, 1, 12000);
camera2->setCameraFlag(CameraFlag::USER5);
```
Now we need a FBO and a render target. This allows the camera to render as a texture that we can use in a sprite.
```
auto fboSize = Size (Director::getInstance()->getOpenGLView()->getFrameSize().width,Director::getInstance()->getOpenGLView()->getFrameSize().height);
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
```
Now set the camera to target the FBO
```
camera1->setFrameBufferObject(fbo);
```
Create the Sprite you want to use to render and assign the FBO texture to it. Set texture rect width to be half of the screen. This way the Sprite won’t draw the full screen.

Place the sprite on the center of the screen. And set the anchor point middle right;
```
playerScreen1 = Sprite::create();
playerScreen1->initWithTexture(fbo->getRenderTarget()->getTexture());
this->addChild(playerScreen1);
playerScreen1->setScale(frameSize.width/playerScreen1->getContentSize().width,frameSize.height/playerScreen1->getContentSize().height);
playerScreen1->setTextureRect(Rect(0,0,fboSize.width/2,fboSize.height));
playerScreen1->setFlippedY(true);
playerScreen1->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
playerScreen1->setPosition(winSize.width/2,winSize.height/2);
```
Similarly, we create another FBO, render target, and sprite for screen 2. But on screen 2 we flip the sides. Set texture rect to draw the other side and set the anchor point middle left.
```
playerScreen2->setTextureRect(Rect(fboSize.width/2,0,fboSize.width/2,fboSize.height));
playerScreen2->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
```
Now that we have 2 screens, we want our cameras to look at the focus points, follow them, and then rotate them as required. Also we want them to merge when the focus points get too close (inside the screen size). We will use it as distanceToSplit. If the distance gets larger than distanceToSplit we will split the views otherwise we combine them.
```
const float distanceToSplit = 900;
Point pointToLookAt =  Player2->getPosition().getMidpoint(Player1->getPosition());
float angle = Shared::calcAngle(Player::getInstance()->getPosition(),pointToLookAt) ;
camera1>setPositionZ(3200); // whatever Position z you want
camera2>setPositionZ(3200);
camera->setRotation(angle);
playerScreen1->setRotation(angle);
playerScreen2->setRotation(angle);
if(Player1.distance(pointToLookAt) < distanceToSplit)
{
 //combine screens
camera1>setPosition(pointToLookAt);
camera2>setPosition(pointToLookAt);
}
else{
camera1>setPosition(pointToLookAt.x + (Player1->getPosition().distance(pointToLookAt) - distanceToSplit) * cosf(CC_DEGREES_TO_RADIANS(camera1>getRotation() )),pointToLookAt.y +(Player1->getPosition().distance(pointToLookAt) - distanceToSplit) * -sinf(CC_DEGREES_TO_RADIANS(camera1>getRotation() )));
camera2->setPosition(pointToLookAt.x + (Player2->getPosition().distance(pointToLookAt) - distanceToSplit) * -cosf(CC_DEGREES_TO_RADIANS(camera2->getRotation() )),pointToLookAt.y +(Player2->getPosition().distance(pointToLookAt) - distanceToSplit) * sinf(CC_DEGREES_TO_RADIANS(camera2->getRotation() )));
}
```
The Complete effect can be seen in the video below:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/4HxRO0wg2Ic/0.jpg)](https://www.youtube.com/watch?v=4HxRO0wg2Ic)


