// graphics_operations.hpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2017 Leigh Johnston
  
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
#include <neolib_variant.hpp>
#include <neolib_vecarray.hpp>
#include <neogfx_geometry.hpp>
#include <neogfx_path.hpp>
#include <neogfx_graphics_context.hpp>
#include <neogfx_texture.hpp>
#include <neogfx_mesh.hpp>
#include <neogfx_i_native_texture.hpp>
#include <neogfx_i_native_font_face.hpp>

namespace neogfx
{
	namespace graphics_operation
	{
		struct set_logical_coordinate_system
		{
			logical_coordinate_system system;
		};

		struct set_logical_coordinates
		{
			std::pair<vec2, vec2> coordinates;
		};

		struct scissor_on
		{
			rect rect;
		};

		struct scissor_off
		{
		};

		struct clip_to_rect
		{
			rect rect;
		};

		struct clip_to_path
		{
			path path;
			dimension pathOutline;
		};

		struct reset_clip
		{
		};

		struct set_smoothing_mode
		{
			smoothing_mode smoothingMode;
		};

		struct push_logical_operation
		{
			logical_operation logicalOperation;
		};

		struct pop_logical_operation
		{
		};

		struct line_stipple_on
		{
			uint32_t factor;
			uint16_t pattern;
		};

		struct line_stipple_off
		{
		};

		struct subpixel_rendering_on
		{
		};

		struct subpixel_rendering_off
		{
		};

		struct clear
		{
			colour colour;
		};

		struct set_pixel
		{
			point point;
			colour colour;
		};

		struct draw_pixel
		{
			point point;
			colour colour;
		};

		struct draw_line
		{
			point from;
			point to;
			pen pen;
		};

		struct draw_rect
		{
			rect rect;
			pen pen;
		};

		struct draw_rounded_rect
		{
			rect rect;
			dimension radius;
			pen pen;
		};

		struct draw_circle
		{
			point centre;
			dimension radius;
			pen pen;
			angle startAngle;
		};

		struct draw_arc
		{
			point centre;
			dimension radius;
			angle startAngle;
			angle endAngle;
			pen pen;
		};

		struct draw_path
		{
			path path;
			pen pen;
		};

		struct draw_shape
		{
			neogfx::mesh mesh;
			pen pen;
		};

		struct fill_rect
		{
			rect rect;
			fill fill;
		};

		struct fill_rounded_rect
		{
			rect rect;
			dimension radius;
			fill fill;
		};

		struct fill_circle
		{
			point centre;
			dimension radius;
			fill fill;
		};

		struct fill_arc
		{
			point centre;
			dimension radius;
			angle startAngle;
			angle endAngle;
			fill fill;
		};

		struct fill_path
		{
			path path;
			fill fill;
		};

		struct fill_shape
		{
			neogfx::mesh mesh;
			fill fill;
		};

		struct draw_glyph
		{
			point point;
			glyph glyph;
			font font;
			colour colour;
		};

		struct draw_texture
		{
			neogfx::mesh mesh;
			texture texture;
			rect textureRect;
			optional_colour colour;
			shader_effect shaderEffect;
		};

		typedef neolib::variant <
			set_logical_coordinate_system,
			set_logical_coordinates,
			scissor_on,
			scissor_off,
			clip_to_rect,
			clip_to_path,
			reset_clip,
			set_smoothing_mode,
			push_logical_operation,
			pop_logical_operation,
			line_stipple_on,
			line_stipple_off,
			subpixel_rendering_on,
			subpixel_rendering_off,
			clear,
			set_pixel,
			draw_pixel,
			draw_line,
			draw_rect,
			draw_rounded_rect,
			draw_circle,
			draw_arc,
			draw_path,
			draw_shape,
			fill_rect,
			fill_rounded_rect,
			fill_circle,
			fill_arc,
			fill_path,
			fill_shape,
			draw_glyph,
			draw_texture
		> operation;

		enum operation_type
		{
			Invalid = 0,
			SetLogicalCoordinateSystem,
			SetLogicalCoordinates,
			ScissorOn,
			ScissorOff,
			ClipToRect,
			ClipToPath,
			ResetClip,
			SetSmoothingMode,
			PushLogicalOperation,
			PopLogicalOperation,
			LineStippleOn,
			LineStippleOff,
			SubpixelRenderingOn,
			SubpixelRenderingOff,
			Clear,
			SetPixel,
			DrawPixel,
			DrawLine,
			DrawRect,
			DrawRoundedRect,
			DrawCircle,
			DrawArc,
			DrawPath,
			DrawShape,
			FillRect,
			FillRoundedRect,
			FillCircle,
			FillArc,
			FillPath,
			FillShape,
			DrawGlyph,
			DrawTexture
		};

		bool inline batchable(const operation& aLeft, const operation& aRight)
		{
			if (aLeft.which() != aRight.which())
				return false;
			switch (static_cast<operation_type>(aLeft.which()))
			{
			case operation_type::SetPixel:
			case operation_type::DrawPixel:
				return true;
			case operation_type::DrawLine:
			{
				auto& left = static_variant_cast<const draw_line&>(aLeft);
				auto& right = static_variant_cast<const draw_line&>(aRight);
				return left.pen.width() == right.pen.width() &&
					left.pen.anti_aliased() == right.pen.anti_aliased();
			}
			case operation_type::FillRect:
			{
				auto& left = static_variant_cast<const fill_rect&>(aLeft);
				auto& right = static_variant_cast<const fill_rect&>(aRight);
				return left.fill.which() == right.fill.which() && left.fill.is<colour>();
			}
			case operation_type::FillShape:
			{
				auto& left = static_variant_cast<const fill_shape&>(aLeft);
				auto& right = static_variant_cast<const fill_shape&>(aRight);
				return left.fill.which() == right.fill.which() && left.fill.is<colour>();
			}
			case operation_type::DrawGlyph:
			{
				auto& left = static_variant_cast<const draw_glyph&>(aLeft);
				auto& right = static_variant_cast<const draw_glyph&>(aRight);
				if (left.glyph.is_emoji() || right.glyph.is_emoji())
					return false;
				const i_glyph_texture& leftGlyphTexture = !left.glyph.use_fallback() ? left.font.native_font_face().glyph_texture(left.glyph) : 
					left.glyph.fallback_font(left.font).native_font_face().glyph_texture(left.glyph);
				const i_glyph_texture& rightGlyphTexture = !right.glyph.use_fallback() ? right.font.native_font_face().glyph_texture(right.glyph) :
					right.glyph.fallback_font(right.font).native_font_face().glyph_texture(right.glyph);
				return leftGlyphTexture.texture().native_texture()->handle() == rightGlyphTexture.texture().native_texture()->handle() &&
					left.glyph.subpixel() == right.glyph.subpixel();
			}
			case operation_type::DrawTexture:
			{
				auto& left = static_variant_cast<const draw_texture&>(aLeft);
				auto& right = static_variant_cast<const draw_texture&>(aRight);
				return left.texture.native_texture()->handle() == right.texture.native_texture()->handle() && left.shaderEffect == right.shaderEffect;
			}
			default:
				return false;
			}
		}

		typedef neolib::vecarray<operation, 1, -1> batch;

		typedef std::deque<batch> queue;
	}
}