// text.hpp
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
#include <neogfx_i_widget.hpp>
#include <neogfx_font.hpp>
#include "shape.hpp"

namespace neogfx
{
	class text : public shape<>
	{
	public:
		text(i_shape_container& aContainer, const vec3& aPosition, const std::string& aText, const neogfx::font& aFont, const colour& aTextColour, neogfx::alignment aAlignment = alignment::Left, const optional_colour& aBackgroundColour = optional_colour());
	public:
		const colour& text_colour() const;
		void set_text_colour(const colour& aTextColour);
		const std::string& value() const;
		void set_value(const std::string& aText);
		const neogfx::font& font() const;
		void set_font(const neogfx::font& aFont);
		neogfx::alignment alignment() const;
		void set_alignment(neogfx::alignment aAlignment);
		const optional_colour& background_colour() const;
		void set_background_colour(const optional_colour& aBackgroundColour);
		const optional_dimension& border() const;
		void set_border(const optional_dimension& aBorder);
		const optional_margins& margins() const;
		void set_margins(const optional_margins& aMargins);
	public:
		vec3 extents() const override;
		rect bounding_box_2d(bool aWithPosition = true) const override;
	public:
		void paint(graphics_context& aGraphicsContext) const override;
	private:
		size text_extent() const;
	private:
		std::string iText;
		neogfx::font iFont;
		neogfx::alignment iAlignment;
		mutable optional_size iTextExtent;
		mutable glyph_text iGlyphTextCache;
		optional_colour iBackgroundColour;
		optional_dimension iBorder;
		optional_margins iMargins;
	};
}