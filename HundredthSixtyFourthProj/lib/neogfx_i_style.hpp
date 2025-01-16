// i_style.hpp
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
#include <neogfx_event.hpp>
#include <neogfx_geometry.hpp>
#include <neogfx_colour.hpp>
#include <neogfx_font.hpp>
#include "i_palette.hpp"

namespace neogfx
{
	class i_style
	{
	public:
		event<> changed;
	public:
		virtual ~i_style() {}
	public:
		virtual const std::string& name() const = 0;
		virtual const neogfx::margins& margins() const = 0;
		virtual void set_margins(const neogfx::margins& aMargins) = 0;
		virtual const size& spacing() const = 0;
		virtual void set_spacing(const size& aSpacing) = 0;
		virtual const i_palette& palette() const = 0;
		virtual i_palette& palette() = 0;
		virtual void set_palette(const i_palette& aPalette) = 0;
		virtual const font_info& font_info() const = 0;
		virtual void set_font_info(const neogfx::font_info& aFontInfo) = 0;
		virtual const neogfx::font& font() const = 0;
	};
}