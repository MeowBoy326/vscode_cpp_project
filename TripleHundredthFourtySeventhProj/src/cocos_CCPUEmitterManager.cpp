/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#include "cocos_CCPUEmitterManager.h"
#include "cocos_CCPUBoxEmitter.h"
#include "cocos_CCPUCircleEmitter.h"
#include "cocos_CCPULineEmitter.h"
#include "cocos_CCPUMeshSurfaceEmitter.h"
#include "cocos_CCPUPointEmitter.h"
#include "cocos_CCPUPositionEmitter.h"
#include "cocos_CCPUSlaveEmitter.h"
#include "cocos_CCPUSphereSurfaceEmitter.h"

NS_CC_BEGIN
PUEmitterManager::PUEmitterManager()
{
    
}

PUEmitterManager::~PUEmitterManager()
{
    
}

PUEmitterManager* PUEmitterManager::Instance()
{
    static PUEmitterManager ptm;
    return &ptm;
}

PUScriptTranslator* PUEmitterManager::getTranslator( const std::string &type )
{
    if (type == "Box"){
        return &_boxEmitterTranlator;
    }else if (type == "Circle"){
        return &_circleEmitterTranlator;
    }else if (type == "Line"){
        return &_lineEmitterTranlator;
    }else if (type == "MeshSurface"){
        return &_meshSurfaceEmitterTranlator;
    }else if (type == "Point"){
        return &_pointEmitterTranlator;
    }else if (type == "Position"){
        return &_positionEmitterTranlator;
    }else if (type == "Slave"){
        return &_slaveEmitterTranlator;
    }else if (type == "SphereSurface"){
        return &_sphereSurfaceEmitterTranlator;
    }else if (type == "Vertex"){
        return nullptr;
    }
    return nullptr;
}

PUEmitter* PUEmitterManager::createEmitter( const std::string &type )
{
    if (type == "Box"){
        return CCPUBoxEmitter::create();
    }else if (type == "Circle"){
        return PUCircleEmitter::create();
    }else if (type == "Line"){
        return PULineEmitter::create();
    }else if (type == "MeshSurface"){
        return PUMeshSurfaceEmitter::create();
    }else if (type == "Point"){
        return PUPointEmitter::create();
    }else if (type == "Position"){
        return PUPositionEmitter::create();
    }else if (type == "Slave"){
        return PUSlaveEmitter::create();
    }else if (type == "SphereSurface"){
        return PUSphereSurfaceEmitter::create();
    }else if (type == "Vertex"){
        return nullptr;
    }
    return nullptr;
}

NS_CC_END