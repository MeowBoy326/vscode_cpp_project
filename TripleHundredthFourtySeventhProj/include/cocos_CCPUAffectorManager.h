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

#ifndef __CC_PU_PARTICLE_3D_AFFECTOR_MANAGER_H__
#define __CC_PU_PARTICLE_3D_AFFECTOR_MANAGER_H__

#include "cocos_CCRef.h"
#include "cocos_CCPUScriptTranslator.h"
#include "cocos_CCPUAffector.h"
#include "cocos_CCPUAlignAffectorTranslator.h"
#include "cocos_CCPUBaseColliderTranslator.h"
#include "cocos_CCPUBaseForceAffectorTranslator.h"
#include "cocos_CCPUBoxColliderTranslator.h"
#include "cocos_CCPUCollisionAvoidanceAffectorTranslator.h"
#include "cocos_CCPUColorAffectorTranslator.h"
#include "cocos_CCPUFlockCenteringAffectorTranslator.h"
#include "cocos_CCPUForceFieldAffectorTranslator.h"
#include "cocos_CCPUGeometryRotatorTranslator.h"
#include "cocos_CCPUGravityAffectorTranslator.h"
#include "cocos_CCPUInterParticleColliderTranslator.h"
#include "cocos_CCPUJetAffectorTranslator.h"
#include "cocos_CCPULineAffectorTranslator.h"
#include "cocos_CCPULinearForceAffectorTranslator.h"
#include "cocos_CCPUParticleFollowerTranslator.h"
#include "cocos_CCPUPathFollowerTranslator.h"
#include "cocos_CCPUPlaneColliderTranslator.h"
#include "cocos_CCPURandomiserTranslator.h"
#include "cocos_CCPUScaleAffectorTranslator.h"
#include "cocos_CCPUScaleVelocityAffectorTranslator.h"
#include "cocos_CCPUSineForceAffectorTranslator.h"
#include "cocos_CCPUSphereColliderTranslator.h"
#include "cocos_CCPUTextureAnimatorTranslator.h"
#include "cocos_CCPUTextureRotatorTranslator.h"
#include "cocos_CCPUVelocityMatchingAffectorTranslator.h"
#include "cocos_CCPUVortexAffectorTranslator.h"

NS_CC_BEGIN

class PUAffectorManager
{
public:
    
    static PUAffectorManager* Instance();

    /**
     */
    PUScriptTranslator* getTranslator(const std::string &type);
    PUAffector* createAffector(const std::string &type);

    
CC_CONSTRUCTOR_ACCESS:
    PUAffectorManager();
    ~PUAffectorManager();

protected:

    PUAlignAffectorTranslator _alignAffectorTranslator;
    PUBaseColliderTranslator _baseColliderTranslator;
    PUBaseForceAffectorTranslator _baseForceAffectorTranslator;
    PUBoxColliderTranslator _boxColliderTranslator;
    PUCollisionAvoidanceAffectorTranslator _collisionAvoidanceAffectorTranstor;
    PUColorAffectorTranslator _colorAffectorTranslator; 
    PUFlockCenteringAffectorTranslator _flockCenteringAffectorTranslator;
    PUForceFieldAffectorTranslator _forceFieldAffectorTranslator;
    PUGeometryRotatorTranslator _geometryRotatorTranslator; 
    PUGravityAffectorTranslator _gravityAffectorTranslator;
    PUInterParticleColliderTranslator _interParticleColliderTranslator;
    PUJetAffectorTranslator _jetAffectorTranslator;
    PULineAffectorTranslator _lineAffectorTranslator;
    PULinearForceAffectorTranslator _linearForceAffectorTranslator;
    PUParticleFollowerTranslator _particleFollowerTranslator;
    PUPathFollowerTranslator _pathFollowerTranslator;
    PUPlaneColliderTranslator _planeColliderTranslator;
    PURandomiserTranslator _randomiserTranslator;
    PUScaleAffectorTranslator _scaleAffectorTranslator;
    PUScaleVelocityAffectorTranslator _scaleVelocityAffectorTranslator;
    PUSineForceAffectorTranslator _sineForceAffectorTranslator;
    PUSphereColliderTranslator _sphereColliderTranslator;
    PUTextureAnimatorTranslator _textureAnimatorTranslator;
    PUTextureRotatorTranslator _textureRotatorTranslator;
    PUVelocityMatchingAffectorTranslator _velocityMatchingAffectorTranslator;
    PUVortexAffectorTranslator _vortexAffectorTranslator;
};

NS_CC_END


#endif 
