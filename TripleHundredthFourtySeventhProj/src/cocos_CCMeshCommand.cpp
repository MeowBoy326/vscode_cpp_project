/****************************************************************************
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

#include "cocos_CCMeshCommand.h"
#include "cocos_ccMacros.h"
#include "cocos_CCConfiguration.h"
#include "cocos_CCDirector.h"
#include "cocos_CCEventCustom.h"
#include "cocos_CCEventListenerCustom.h"
#include "cocos_CCEventDispatcher.h"
#include "cocos_CCEventType.h"
#include "cocos_CCLight.h"
#include "cocos_CCCamera.h"
#include "cocos_CCRenderer.h"
#include "cocos_CCTextureAtlas.h"
#include "cocos_CCTexture2D.h"
#include "cocos_CCTechnique.h"
#include "cocos_CCMaterial.h"
#include "cocos_CCPass.h"
#include "xxhash.h"

NS_CC_BEGIN


MeshCommand::MeshCommand()
#if CC_ENABLE_CACHE_TEXTURE_DATA
 : _rendererRecreatedListener(nullptr)
#endif
{
    _type = RenderCommand::Type::MESH_COMMAND;
    _is3D = true;
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // listen the event that renderer was recreated on Android/WP8
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(MeshCommand::listenRendererRecreated, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void MeshCommand::init(float globalZOrder)
{
    CustomCommand::init(globalZOrder);
}

void MeshCommand::init(float globalZOrder, const Mat4 &transform)
{
    CustomCommand::init(globalZOrder);        
    if (Camera::getVisitingCamera())
    {
        _depth = Camera::getVisitingCamera()->getDepthInView(transform);
    }
    _mv = transform;
}

MeshCommand::~MeshCommand()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

#if CC_ENABLE_CACHE_TEXTURE_DATA
void MeshCommand::listenRendererRecreated(EventCustom* event)
{
}
#endif

NS_CC_END
