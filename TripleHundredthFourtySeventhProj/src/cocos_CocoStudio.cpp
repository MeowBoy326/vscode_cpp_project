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


#include "cocos_CocoStudio.h"

#include "cocos_cocostudioNodeReader.h"
#include "cocos_cocostudioSingleNodeReader.h"
#include "cocos_cocostudioSpriteReader.h"
#include "cocos_cocostudioParticleReader.h"
#include "cocos_cocostudioGameMapReader.h"
#include "cocos_cocostudioProjectNodeReader.h"
#include "cocos_cocostudioComAudioReader.h"

#include "cocos_cocostudioButtonReader.h"
#include "cocos_cocostudioCheckBoxReader.h"
#include "cocos_cocostudioImageViewReader.h"
#include "cocos_cocostudioTextBMFontReader.h"
#include "cocos_cocostudioTextReader.h"
#include "cocos_cocostudioTextFieldReader.h"
#include "cocos_cocostudioTextAtlasReader.h"
#include "cocos_cocostudioLoadingBarReader.h"
#include "cocos_cocostudioSliderReader.h"
#include "cocos_cocostudioLayoutReader.h"
#include "cocos_cocostudioScrollViewReader.h"
#include "cocos_cocostudioPageViewReader.h"
#include "cocos_cocostudioListViewReader.h"
#include "cocos_cocostudioArmatureNodeReader.h"
#include "cocos_cocostudioNode3DReader.h"
#include "cocos_cocostudioSprite3DReader.h"
#include "cocos_cocostudioUserCameraReader.h"
#include "cocos_cocostudioParticle3DReader.h"


namespace cocostudio
{
    void destroyCocosStudio()
    {        
        NodeReader::destroyInstance();
        SingleNodeReader::destroyInstance();
        SpriteReader::destroyInstance();
        ParticleReader::destroyInstance();
        GameMapReader::destroyInstance();
        ProjectNodeReader::destroyInstance();
        ComAudioReader::destroyInstance();
        
        WidgetReader::destroyInstance();
        ButtonReader::destroyInstance();
        CheckBoxReader::destroyInstance();
        ImageViewReader::destroyInstance();
        TextBMFontReader::destroyInstance();
        TextReader::destroyInstance();
        TextFieldReader::destroyInstance();
        TextAtlasReader::destroyInstance();
        LoadingBarReader::destroyInstance();
        SliderReader::destroyInstance();
        LayoutReader::destroyInstance();
        ScrollViewReader::destroyInstance();
        PageViewReader::destroyInstance();
        ListViewReader::destroyInstance();
        
        ArmatureNodeReader::destroyInstance();
        Node3DReader::destroyInstance();
        Sprite3DReader::destroyInstance();
        UserCameraReader::destroyInstance();
        Particle3DReader::destroyInstance();
        
        cocos2d::CSLoader::destroyInstance();
        
        
        ArmatureDataManager::destroyInstance();
        SceneReader::destroyInstance();
        ActionManagerEx::destroyInstance();
        GUIReader::destroyInstance();
    }
}