// opengl_error.hpp
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
#include <GL/glew.h>
#include <GL/GL.h>

std::string glErrorString(GLenum aErrorCode);
GLenum glCheckError(const char* file, unsigned int line);

#ifdef glCheck
#undef glCheck 
#endif
#define glCheck(x) x; glCheckError(__FILE__, __LINE__);

namespace neogfx
{
	struct opengl_error : std::runtime_error
	{
		opengl_error(const std::string& aMessage) : std::runtime_error("neogfx::opengl_error: " + aMessage) {};
	};
}