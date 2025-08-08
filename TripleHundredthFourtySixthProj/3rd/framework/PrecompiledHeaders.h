/**
* Copyright (c) 2013 David Young dayoung@goliathdesigns.com
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
*  1. The origin of this software must not be misrepresented; you must not
*  claim that you wrote the original software. If you use this software
*  in a product, an acknowledgment in the product documentation would be
*  appreciated but is not required.
*
*  2. Altered source versions must be plainly marked as such, and must not be
*  misrepresented as being the original software.
*
*  3. This notice may not be removed or altered from any source
*  distribution.
*/

#ifndef DEMO_FRAMEWORK_PRECOMPILED_HEADERS_H
#define DEMO_FRAMEWORK_PRECOMPILED_HEADERS_H

// STL C Headers
#include <assert.h>
#include <string.h>

// STL C++ Headers
#include <algorithm>

// C Library Headers
extern "C"
{
#include "lauxlib.h"
#include "lstate.h"
#include "lua.h"
#include "lualib.h"
};

// C++ Library Headers
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#include "btBulletDynamicsCommon.h"
#include "btShapeHull.h"
#pragma warning(pop)

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"

#include Ogre.h"
#include OgreTagPoint.h"

#pragma warning(push)
#pragma warning(disable : 4512)
#include Samples/SdkCameraMan.h"
#pragma warning(pop)

#include "OgreD3D9Plugin.h"
#include "Gorilla.h"
#include "OgreParticleFXPlugin.h"
#include "Procedural.h"

#pragma warning(push)
#pragma warning(disable : 4512)
#include "ois/include/OIS.h"
#pragma warning(pop)

#include "opensteer/include/Vec3.h"

#include "recast/include/Recast.h"

#include "zzip/_msvc.h"
#include "zzip/conf.h"
#include "zzip/types.h"
#include "zzip/zzip.h"
#include "zzip/plugin.h"

#endif  // DEMO_FRAMEWORK_PRECOMPILED_HEADERS_H
