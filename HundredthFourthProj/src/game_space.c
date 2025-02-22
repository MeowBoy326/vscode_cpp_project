﻿/* ****************************************************************************
 * Punch And Kick -- a simple 2D Fighting Game.
 *
 * Copyright (C) 2013 by
 * Liu Chao
 *
 * This file is part of the Punch And Kick project, and may only be used,
 * modified, and distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The Punch And Kick project is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for
 * more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * Punch And Kick -- 一个普通的2D格斗游戏
 *
 * 版权所有 (C) 2013 归属于
 * 刘超
 *
 * 这个文件是 Punch And Kick 项目的一部分，并且只可以根据GPLv2许可协议来使用、
 * 更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * Punch And Kick 项目是基于技术交流目的而加以散布的，但不负任何担保责任，例
 * 如：稳定性、可玩性、趣味性等的担保，甚至没有适销性或特定用途的隐含担保，详
 * 情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

// 游戏空间

#include <LCUI_Build.h>
#include LC_LCUI_H

#include "game_space.h"

/** 设置空间边界 */
void GameSpace_SetBound( GameSpaceData *space, int x, int x_width, int y, int y_width )
{
	space->space_x = x;
	space->space_y = y;
	space->space_x_width = x_width;
	space->space_y_width = y_width;
}

/** 获取Y轴空间范围 */
void GameSpace_GetXBound( GameSpaceData *space, int* x, int* x_width )
{
	if( x ) {
		*x = space->space_x;
	}
	if( x_width ) {
		*x_width = space->space_x_width;
	}
}

/** 获取X轴空间范围 */
void GameSpace_GetYBound( GameSpaceData *space, int* y, int* y_width )
{
	if( y ) {
		*y = space->space_y;
	}
	if( y_width ) {
		*y_width = space->space_y_width;
	}
}

/** 处理空间内的对象的运动 */
void GameSpace_Step( GameSpaceData *space )
{
	int i, n;
	SpaceObject *obj;

	Queue_Lock( &space->space_object_list );
	n = Queue_GetTotal( &space->space_object_list );
	for(i=0; i<n; ++i) {
		obj = (SpaceObject*)Queue_Get( &space->space_object_list, i );
		if( !obj ) {
			continue;
		}
		/* 更新对象的空间坐标 */
		if( obj->x_speed < 0.0 ) {
			obj->x_speed += (obj->x_acc/FRAMES_PER_SEC);
			if( obj->x_acc > 0.0 ) {
				if( obj->x_speed > 0.0 ) {
					obj->x_speed = 0.0;
				}
			}
		}
		else if( obj->x_speed > 0.0 ) {
			obj->x_speed += (obj->x_acc/FRAMES_PER_SEC);
			if( obj->x_acc < 0.0 ) {
				if( obj->x_speed < 0.0 ) {
					obj->x_speed = 0.0;
				}
			}
		}
		if( obj->y_speed < 0.0 ) {
			obj->y_speed += (obj->y_acc/FRAMES_PER_SEC);
			if( obj->y_acc > 0.0 ) {
				if( obj->y_speed > 0.0 ) {
					obj->y_speed = 0.0;
				}
			}
		}
		else if( obj->y_speed > 0.0 ) {
			obj->y_speed += (obj->y_acc/FRAMES_PER_SEC);
			if( obj->y_acc < 0.0 ) {
				if( obj->y_speed < 0.0 ) {
					obj->y_speed = 0.0;
				}
			}
		}
		obj->z_speed += (obj->z_acc/FRAMES_PER_SEC);
		obj->x += (obj->x_speed/FRAMES_PER_SEC);
		obj->y += (obj->y_speed/FRAMES_PER_SEC);
		obj->z += (obj->z_speed/FRAMES_PER_SEC);
		if( obj->x < space->space_x ) {
			obj->x = space->space_x;
		}
		if( obj->x >= space->space_x + space->space_x_width ) {
			obj->x = space->space_x + space->space_x_width - 1;
		}
		if( obj->y < space->space_y ) {
			obj->y = space->space_y;
		}
		if( obj->y >= space->space_y + space->space_y_width ) {
			obj->y = space->space_y + space->space_y_width - 1;
		}
	}
	Queue_Unlock( &space->space_object_list );
}

/** 创建一个游戏空间 */
GameSpaceData *GameSpace_New(void)
{
	GameSpaceData *space;
	space = (GameSpaceData*)malloc( sizeof(GameSpaceData) );
	Queue_Init( &space->space_object_list, sizeof(SpaceObject), NULL ); 
	return space;
}

/** 销毁一个游戏空间 */
void GameSapce_Destroy( GameSpaceData *space )
{
	Queue_Destroy( &space->space_object_list );
	free( space );
}

/**
创建一个新的对象
@param x
	对象所在的X轴的坐标
@param y
	对象所在的Y轴的坐标
@param z
	对象所在的Z轴的坐标
@param x_width
	对象在X轴上的宽度
@param y_width
	对象在Y轴上的宽度
@param z_width
	对象在Z轴上的宽度
@return
	创建成功则返回该对象，失败则返回NULL
*/
SpaceObject* SpaceObject_New( GameSpaceData *space, int x, int y, int z, int x_width, int y_width, int z_width )
{
	SpaceObject *p, obj;

	obj.x = x;
	obj.y = y;
	obj.z = z;
	obj.x_width = x_width;
	obj.y_width = y_width;
	obj.z_width = z_width;
	obj.x_speed = 0;
	obj.y_speed = 0;
	obj.z_speed = 0;
	obj.x_acc = 0;
	obj.y_acc = 0;
	obj.z_acc = 0;
	obj.space = space;
	Queue_Lock( &space->space_object_list );
	p = (SpaceObject*)Queue_Add( &space->space_object_list, &obj );
	Queue_Unlock( &space->space_object_list );
	return p;
}

void SpaceObject_Destroy( SpaceObject* object )
{
	int i, n;
	SpaceObject* p;
	
	Queue_Lock( &object->space->space_object_list );
	n = Queue_GetTotal( &object->space->space_object_list );
	for(i=0; i<n; ++i) {
		p = (SpaceObject*)Queue_Get( &object->space->space_object_list, i );
		if( p == object ) {
			Queue_Delete( &object->space->space_object_list, i );
			break;
		}
	}
	Queue_Unlock( &object->space->space_object_list );
}
