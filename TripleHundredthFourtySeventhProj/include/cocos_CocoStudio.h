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

#ifndef __EDITOR_SUPPORT_COCOSTUDIO_H__
#define __EDITOR_SUPPORT_COCOSTUDIO_H__

#include "cocos_cocostudioCCActionFrame.h"
#include "cocos_cocostudioCCActionFrameEasing.h"
#include "cocos_cocostudioCCActionManagerEx.h"
#include "cocos_cocostudioCCActionNode.h"
#include "cocos_cocostudioCCActionObject.h"
#include "cocos_cocostudioCCArmature.h"
#include "cocos_cocostudioCCBone.h"
#include "cocos_cocostudioCCArmatureAnimation.h"
#include "cocos_cocostudioCCProcessBase.h"
#include "cocos_cocostudioCCTween.h"
#include "cocos_cocostudioCCDatas.h"
#include "cocos_cocostudioCCBatchNode.h"
#include "cocos_cocostudioCCDecorativeDisplay.h"
#include "cocos_cocostudioCCDisplayFactory.h"
#include "cocos_cocostudioCCDisplayManager.h"
#include "cocos_cocostudioCCSkin.h"
#include "cocos_cocostudioCCColliderDetector.h"
#include "cocos_cocostudioCCArmatureDataManager.h"
#include "cocos_cocostudioCCArmatureDefine.h"
#include "cocos_cocostudioCCDataReaderHelper.h"
#include "cocos_cocostudioCCTransformHelp.h"
#include "cocos_cocostudioCCUtilMath.h"
#include "cocos_cocostudioCCComBase.h"
#include "cocos_cocostudioCCComAttribute.h"
#include "cocos_cocostudioCCComAudio.h"
#include "cocos_cocostudioCCComController.h"
#include "cocos_cocostudioCCComRender.h"
#include "cocos_cocostudioCCInputDelegate.h"
#include "cocos_cocostudioDictionaryHelper.h"
#include "cocos_cocostudioCCSGUIReader.h"
#include "cocos_cocostudioCCSSceneReader.h"
#include "cocos_cocostudioTriggerBase.h"
#include "cocos_cocostudioCCActionTimelineCache.h"
#include "cocos_cocostudioCCFrame.h"
#include "cocos_cocostudioCCTimeLine.h"
#include "cocos_cocostudioCCActionTimeline.h"
#include "cocos_cocostudioCCActionTimelineNode.h"
#include "cocos_cocostudioCCBoneNode.h"
#include "cocos_cocostudioCCSkeletonNode.h"
#include "cocos_cocostudioCSLoader.h"
#include "cocos_CocosStudioExport.h"

namespace cocostudio
{
    void CC_STUDIO_DLL destroyCocosStudio();
}

#endif
