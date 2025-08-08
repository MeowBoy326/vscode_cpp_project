/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   03 08 00
#define COCOS2D_VERSION 0x00040000

//
// all cocos2d include files
//
#include "cocos_ccConfig.h"

// base
#include "cocos_CCAsyncTaskPool.h"
#include "cocos_CCAutoreleasePool.h"
#include "cocos_CCConfiguration.h"
#include "cocos_CCConsole.h"
#include "cocos_CCData.h"
#include "cocos_CCDirector.h"
#include "cocos_CCIMEDelegate.h"
#include "cocos_CCIMEDispatcher.h"
#include "cocos_CCMap.h"
#include "cocos_CCNS.h"
#include "cocos_CCProfiling.h"
#include "cocos_CCProperties.h"
#include "cocos_CCRef.h"
#include "cocos_CCRefPtr.h"
#include "cocos_CCScheduler.h"
#include "cocos_CCUserDefault.h"
#include "cocos_CCValue.h"
#include "cocos_CCVector.h"
#include "cocos_ZipUtils.h"
#include "cocos_base64.h"
#include "cocos_ccConfig.h"
#include "cocos_ccMacros.h"
#include "cocos_ccTypes.h"
#include "cocos_ccUTF8.h"
#include "cocos_ccUtils.h"

// EventDispatcher
#include "cocos_CCEventAcceleration.h"
#include "cocos_CCEventCustom.h"
#include "cocos_CCEventDispatcher.h"
#include "cocos_CCEventFocus.h"
#include "cocos_CCEventKeyboard.h"
#include "cocos_CCEventListenerAcceleration.h"
#include "cocos_CCEventListenerCustom.h"
#include "cocos_CCEventListenerFocus.h"
#include "cocos_CCEventListenerKeyboard.h"
#include "cocos_CCEventListenerMouse.h"
#include "cocos_CCEventListenerController.h"
#include "cocos_CCEventListenerTouch.h"
#include "cocos_CCEventMouse.h"
#include "cocos_CCEventController.h"
#include "cocos_CCController.h"
#include "cocos_CCEventTouch.h"
#include "cocos_CCEventType.h"

// math
#include "cocos_CCAffineTransform.h"
#include "cocos_CCGeometry.h"
#include "cocos_CCVertex.h"
#include "cocos_Mat4.h"
#include "cocos_MathUtil.h"
#include "cocos_Quaternion.h"
#include "cocos_Vec2.h"
#include "cocos_Vec3.h"
#include "cocos_Vec4.h"

// actions
#include "cocos_CCAction.h"
#include "cocos_CCActionCamera.h"
#include "cocos_CCActionCatmullRom.h"
#include "cocos_CCActionEase.h"
#include "cocos_CCActionGrid.h"
#include "cocos_CCActionGrid3D.h"
#include "cocos_CCActionInstant.h"
#include "cocos_CCActionInterval.h"
#include "cocos_CCActionManager.h"
#include "cocos_CCActionPageTurn3D.h"
#include "cocos_CCActionProgressTimer.h"
#include "cocos_CCActionTiledGrid.h"
#include "cocos_CCActionTween.h"
#include "cocos_CCTweenFunction.h"

// 2d nodes
#include "cocos_CCAtlasNode.h"
#include "cocos_CCClippingNode.h"
#include "cocos_CCClippingRectangleNode.h"
#include "cocos_CCDrawNode.h"
#include "cocos_CCFontFNT.h"
#include "cocos_CCLabel.h"
#include "cocos_CCLabelAtlas.h"
#include "cocos_CCLayer.h"
#include "cocos_CCMotionStreak.h"
#include "cocos_CCNode.h"
#include "cocos_CCNodeGrid.h"
#include "cocos_CCParticleBatchNode.h"
#include "cocos_CCParticleExamples.h"
#include "cocos_CCParticleSystem.h"
#include "cocos_CCParticleSystemQuad.h"
#include "cocos_CCProgressTimer.h"
#include "cocos_CCProtectedNode.h"
#include "cocos_CCRenderTexture.h"
#include "cocos_CCScene.h"
#include "cocos_CCTransition.h"
#include "cocos_CCTransitionPageTurn.h"
#include "cocos_CCTransitionProgress.h"

// 2d utils
#include "cocos_CCCamera.h"
#include "cocos_CCCameraBackgroundBrush.h"
#include "cocos_CCGrid.h"
#include "cocos_CCLight.h"

// include
#include "cocos_CCProtocols.h"

// renderer
#include "cocos_CCCallbackCommand.h"
#include "cocos_CCCustomCommand.h"
#include "cocos_CCGroupCommand.h"
#include "cocos_CCMaterial.h"
#include "cocos_CCPass.h"
#include "cocos_CCQuadCommand.h"
#include "cocos_CCRenderCommand.h"
#include "cocos_CCRenderCommandPool.h"
#include "cocos_CCRenderState.h"
#include "cocos_CCRenderer.h"
#include "cocos_CCTechnique.h"
#include "cocos_CCTexture2D.h"
#include "cocos_CCTextureCube.h"
#include "cocos_CCTextureCache.h"
#include "cocos_CCTrianglesCommand.h"
#include "cocos_ccShaders.h"

// physics
#include "cocos_CCPhysicsBody.h"
#include "cocos_CCPhysicsContact.h"
#include "cocos_CCPhysicsJoint.h"
#include "cocos_CCPhysicsShape.h"
#include "cocos_CCPhysicsWorld.h"

// platform
#include "cocos_CCCommon.h"
#include "cocos_CCDevice.h"
#include "cocos_CCFileUtils.h"
#include "cocos_CCImage.h"
#include "cocos_CCPlatformConfig.h"
#include "cocos_CCPlatformMacros.h"
#include "cocos_CCSAXParser.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #include "cocos_CCApplication-ios.h"
    #include "cocos_CCGLViewImpl-ios.h"
    #include "cocos_CCStdC-ios.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "cocos_CCApplication-android.h"
    #include "cocos_CCGLViewImpl-android.h"
    #include "cocos_CCGL-android.h"
    #include "cocos_CCStdC-android.h"
//Enhance modification begin
    #include "cocos_CCEnhanceAPI-android.h"
//Enhance modification end
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    #include "cocos_CCApplication-win32.h"
    #include "cocos_CCGLViewImpl-desktop.h"
    #include "cocos_CCGL-win32.h"
    #include "cocos_CCStdC-win32.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include "cocos_CCGLViewImpl-desktop.h"
    #include "cocos_CCApplication-mac.h"
    #include "cocos_CCStdC-mac.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    #include "cocos_CCApplication-linux.h"
    #include "cocos_CCGLViewImpl-desktop.h"
    #include "cocos_CCGL-linux.h"
    #include "cocos_CCStdC-linux.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

// script_support
#include "cocos_CCScriptSupport.h"

// sprite_nodes
#include "cocos_CCAnimation.h"
#include "cocos_CCAnimationCache.h"
#include "cocos_CCSprite.h"
#include "cocos_CCAutoPolygon.h"
#include "cocos_CCSpriteBatchNode.h"
#include "cocos_CCSpriteFrame.h"
#include "cocos_CCSpriteFrameCache.h"

// text_input_node
#include "cocos_CCTextFieldTTF.h"

// textures
#include "cocos_CCTextureAtlas.h"

// tilemap_parallax_nodes
#include "cocos_CCParallaxNode.h"
#include "cocos_CCTMXLayer.h"
#include "cocos_CCTMXObjectGroup.h"
#include "cocos_CCTMXTiledMap.h"
#include "cocos_CCTMXXMLParser.h"

// component
#include "cocos_CCComponent.h"
#include "cocos_CCComponentContainer.h"

//3d
#include "cocos_CCAABB.h"
#include "cocos_CCAnimate3D.h"
#include "cocos_CCAnimation3D.h"
#include "cocos_CCAttachNode.h"
#include "cocos_CCBillBoard.h"
#include "cocos_CCFrustum.h"
#include "cocos_CCMesh.h"
#include "cocos_CCMeshSkin.h"
#include "cocos_CCMotionStreak3D.h"
#include "cocos_CCMeshVertexIndexData.h"
#include "cocos_CCOBB.h"
#include "cocos_CCPlane.h"
#include "cocos_CCRay.h"
#include "cocos_CCSkeleton3D.h"
#include "cocos_CCSkybox.h"
#include "cocos_CCSprite3D.h"
#include "cocos_CCSprite3DMaterial.h"
#include "cocos_CCTerrain.h"
#include "cocos_CCVertexAttribBinding.h"

NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END

#endif // __COCOS2D_H__
