/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
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
#include "cocos_ccShaders.h"

NS_CC_BEGIN
//

//backend shader file
#include "cocos_positionColor.vert"
#include "cocos_positionColor.frag"
#include "cocos_positionTexture.vert"
#include "cocos_positionTexture.frag"
#include "cocos_positionTextureColor.vert"
#include "cocos_positionTextureColor.frag"
#include "cocos_positionTextureColorAlphaTest.frag"
#include "cocos_label_normal.frag"
#include "cocos_label_distanceNormal.frag"
#include "cocos_label_outline.frag"
#include "cocos_label_distanceFieldGlow.frag"
#include "cocos_positionColorLengthTexture.vert"
#include "cocos_positionColorLengthTexture.frag"
#include "cocos_positionColorTextureAsPointsize.vert"
#include "cocos_position.vert"
#include "cocos_positionNoMVP.vert"
#include "cocos_layer_radialGradient.frag"
#include "cocos_ui_Gray.frag"
#include "cocos_positionTextureUColor.vert"
#include "cocos_positionTextureUColor.frag"
#include "cocos_positionUColor.vert"
#include "cocos_positionUColor.frag"
#include "cocos_etc1.frag"
#include "cocos_etc1_Gray.frag"
#include "cocos_cameraClear.vert"
#include "cocos_cameraClear.frag"


#include "cocos_3D_color.frag"
#include "cocos_3D_colorNormal.frag"
#include "cocos_3D_colorNormalTexture.frag"
#include "cocos_3D_colorTexture.frag"
#include "cocos_3D_particle.vert"
#include "cocos_3D_particle.frag"
#include "cocos_3D_positionNormalTexture.vert"
#include "cocos_3D_positionTexture.vert"
#include "cocos_3D_skybox.vert"
#include "cocos_3D_skybox.frag"
#include "cocos_3D_terrain.frag"
#include "cocos_3D_terrain.vert"

#include "cocos_lineColor.frag"
#include "cocos_lineColor.vert"

NS_CC_END
