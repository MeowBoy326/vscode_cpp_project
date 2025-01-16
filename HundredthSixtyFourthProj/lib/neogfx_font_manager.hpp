// font_manager.hpp
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
#include <set>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <neolib_string_utils.hpp>
#include <neogfx_texture_atlas.hpp>
#include <neogfx_emoji_atlas.hpp>
#include "i_font_manager.hpp"

namespace neogfx
{
	class native_font;
	class i_rendering_engine;

	class fallback_font_info : public i_fallback_font_info
	{
	public:
		fallback_font_info(std::vector<std::string> aFallbackFontFamilies);
	public:
		virtual const std::string& fallback_for(const std::string& aFontFamilyName) const;
	private:
		std::vector<std::string> iFallbackFontFamilies;
	};

	class font_manager : public i_font_manager
	{
		friend class native_font_face;
	private:
		typedef std::list<native_font> native_font_list;
		typedef std::map<neolib::ci_string, std::vector<native_font_list::iterator>> font_family_list;
	public:
		struct error_initializing_font_library : std::runtime_error { error_initializing_font_library() : std::runtime_error("neogfx::font_manager::error_initializing_font_library") {} };
		struct no_matching_font_found : std::runtime_error { no_matching_font_found() : std::runtime_error("neogfx::font_manager::no_matching_font_found") {} };
		struct failed_to_allocate_glyph_space : std::runtime_error { failed_to_allocate_glyph_space() : std::runtime_error("neogfx::font_manager::failed_to_allocate_glyph_space") {} };
	public:
		font_manager(i_rendering_engine& aRenderingEngine, i_screen_metrics& aScreenMetrics);
		~font_manager();
	public:
		virtual void* font_library_handle() const;
		virtual const font_info& default_system_font_info() const;
		virtual const i_fallback_font_info& default_fallback_font_info() const;
		virtual std::unique_ptr<i_native_font_face> create_default_font(const i_device_resolution& aDevice);
		virtual bool has_fallback_font(const i_native_font_face& aExistingFont) const;
		virtual std::unique_ptr<i_native_font_face> create_fallback_font(const i_native_font_face& aExistingFont);
		virtual std::unique_ptr<i_native_font_face> create_font(const std::string& aFamilyName, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> create_font(const std::string& aFamilyName, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> create_font(const font_info& aInfo, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> create_font(i_native_font& aFont, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> create_font(i_native_font& aFont, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice);
		virtual bool is_font_file(const std::string& aFileName) const;
		virtual std::unique_ptr<i_native_font_face> load_font_from_file(const std::string& aFileName, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> load_font_from_file(const std::string& aFileName, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> load_font_from_file(const std::string& aFileName, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> load_font_from_memory(const void* aData, std::size_t aSizeInBytes, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> load_font_from_memory(const void* aData, std::size_t aSizeInBytes, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice);
		virtual std::unique_ptr<i_native_font_face> load_font_from_memory(const void* aData, std::size_t aSizeInBytes, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice);
	public:
		virtual const i_texture_atlas& glyph_atlas() const;
		virtual i_texture_atlas& glyph_atlas();
		virtual const i_emoji_atlas& emoji_atlas() const;
		virtual i_emoji_atlas& emoji_atlas();
	private:
		i_native_font& find_font(const std::string& aFamilyName, const std::string& aStyleName, font::point_size aSize);
		i_native_font& find_best_font(const std::string& aFamilyName, font::style_e aStyle, font::point_size aSize);
	private:
		i_rendering_engine& iRenderingEngine;
		font_info iDefaultSystemFontInfo;
		fallback_font_info iDefaultFallbackFontInfo;
		FT_Library iFontLib;
		native_font_list iNativeFonts;
		font_family_list iFontFamilies;
		texture_atlas iGlyphAtlas;
		neogfx::emoji_atlas iEmojiAtlas;
	};
}