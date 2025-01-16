// rectangle.cpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2016 Leigh Johnston
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <neogfx.hpp>
#include <neogfx_rectangle.hpp>

namespace neogfx
{
	rectangle::rectangle(const vec3& aPosition, const vec2& aExtents)
	{
		set_position(aPosition);
		set_extents(vec3{ aExtents.x, aExtents.y, 0.0 });
	}

	rectangle::rectangle(const vec3& aPosition, const vec2& aExtents, const colour& aColour) :
		shape(aColour)
	{
		set_position(aPosition);
		set_extents(vec3{ aExtents.x, aExtents.y, 0.0 });
	}

	rectangle::rectangle(const vec3& aPosition, const vec2& aExtents, const i_texture& aTexture, const optional_rect& aTextureRect) :
		shape(aTexture, aTextureRect)
	{
		set_position(aPosition);
		set_extents(vec3{ aExtents.x, aExtents.y, 0.0 });
	}

	rectangle::rectangle(const vec3& aPosition, const vec2& aExtents, const i_image& aImage, const optional_rect& aTextureRect) :
		shape(aImage, aTextureRect)
	{
		set_position(aPosition);
		set_extents(vec3{ aExtents.x, aExtents.y, 0.0 });
	}

	rectangle::rectangle(const rectangle& aOther) :
		shape(aOther)
	{
	}
}