// font_manager.cpp
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

#include <neogfx.hpp>
#include <neolib_string_utils.hpp>
#include <boost/filesystem.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include <neogfx_app.hpp>
#include <neogfx_font_manager.hpp>
#include "../../gfx/text/native/native_font_face.hpp"
#include "../../gfx/text/native/native_font.hpp"

namespace neogfx
{
	namespace detail
	{
		class native_font_face_wrapper : public i_native_font_face
		{
		public:
			native_font_face_wrapper(i_native_font_face& aFontFace) : iFontFace(aFontFace) { add_ref(); }
			~native_font_face_wrapper() { release(); }
		public:
			i_native_font& native_font() override { return iFontFace.native_font(); }
			const std::string& family_name() const override { return iFontFace.family_name(); }
			font::style_e style() const override { return iFontFace.style(); }
			const std::string& style_name() const override { return iFontFace.style_name(); }
			font::point_size size() const override { return iFontFace.size(); }
			dimension horizontal_dpi() const override { return iFontFace.horizontal_dpi(); }
			dimension vertical_dpi() const override { return iFontFace.vertical_dpi(); }
			dimension height() const override { return iFontFace.height(); }
			dimension descender() const override { return iFontFace.descender(); }
			dimension underline_position() const override { return iFontFace.underline_position(); }
			dimension underline_thickness() const override { return iFontFace.underline_thickness(); }
			dimension line_spacing() const override { return iFontFace.line_spacing(); }
			dimension kerning(uint32_t aLeftGlyphIndex, uint32_t aRightGlyphIndex) const override { return iFontFace.kerning(aLeftGlyphIndex, aRightGlyphIndex); }
			bool has_fallback() const override { return iFontFace.has_fallback(); }
			bool fallback_cached() const override { return iFontFace.fallback_cached(); }
			i_native_font_face& fallback() const override { return iFontFace.fallback(); }
			void* handle() const override { return iFontFace.handle(); }
			void update_handle(void* aHandle) override { iFontFace.update_handle(aHandle); }
			void* aux_handle() const override { return iFontFace.aux_handle(); }
			uint32_t glyph_index(char32_t aCodePoint) const override { return iFontFace.glyph_index(aCodePoint); }
			i_glyph_texture& glyph_texture(const glyph& aGlyph) const override { return iFontFace.glyph_texture(aGlyph); }
		public:
			void add_ref() override { iFontFace.add_ref(); }
			void release() override { iFontFace.release(); }
		private:
			i_native_font_face& iFontFace;
		};

		namespace platform_specific
		{
			font_info default_system_font_info()
			{
#ifdef WIN32
				std::wstring defaultFontFaceName = L"Microsoft Sans Serif";
				HKEY hkeyDefaultFont;
				if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes",
					0, KEY_READ, &hkeyDefaultFont) == ERROR_SUCCESS)
				{
					DWORD dwType;
					wchar_t byteBuffer[LF_FACESIZE + 1];
					DWORD dwBufferSize = sizeof(byteBuffer);
					if (RegQueryValueEx(hkeyDefaultFont, L"MS Shell Dlg 2", NULL, &dwType,
						(LPBYTE)&byteBuffer, &dwBufferSize) == ERROR_SUCCESS)
					{
						defaultFontFaceName = (LPCTSTR)byteBuffer;
					}
					else if (RegQueryValueEx(hkeyDefaultFont, L"MS Shell Dlg", NULL, &dwType,
						(LPBYTE)&byteBuffer, &dwBufferSize) == ERROR_SUCCESS)
					{
						defaultFontFaceName = (LPCTSTR)byteBuffer;
					}
					::RegCloseKey(hkeyDefaultFont);
				}
				return font_info(neolib::utf16_to_utf8(reinterpret_cast<const char16_t*>(defaultFontFaceName.c_str())), font::Normal, 8);
#else
				throw std::logic_error("neogfx::detail::platform_specific::default_system_font_info: Unknown system");
#endif
			}

			std::string get_system_font_directory()
			{
#ifdef WIN32
				std::string windowsDirectory;
				windowsDirectory.resize(MAX_PATH);
				GetWindowsDirectoryA(&windowsDirectory[0], windowsDirectory.size());
				windowsDirectory.resize(std::strlen(windowsDirectory.c_str()));
				return windowsDirectory + "\\fonts";
#else
				throw std::logic_error("neogfx::detail::platform_specific::get_system_font_directory: Unknown system");
#endif
			}

			fallback_font_info default_fallback_font_info()
			{
#ifdef WIN32
				return fallback_font_info{ {"Segoe UI Symbol", "Arial Unicode MS" } };
#else
				throw std::logic_error("neogfx::detail::platform_specific::default_fallback_font_info: Unknown system");
#endif
			}
		}
	}

	fallback_font_info::fallback_font_info(std::vector<std::string> aFallbackFontFamilies) :
		iFallbackFontFamilies(std::move(aFallbackFontFamilies))
	{
	}

	const std::string& fallback_font_info::fallback_for(const std::string& aFontFamilyName) const
	{
		auto f = std::find(iFallbackFontFamilies.begin(), iFallbackFontFamilies.end(), aFontFamilyName);
		if (f == iFallbackFontFamilies.end())
			return *iFallbackFontFamilies.begin();
		++f;
		if (f == iFallbackFontFamilies.end())
			--f;
		return *f;
	}

	font_manager::font_manager(i_rendering_engine& aRenderingEngine, i_screen_metrics&) :
		iRenderingEngine{ aRenderingEngine },
		iDefaultSystemFontInfo{ detail::platform_specific::default_system_font_info() },
		iDefaultFallbackFontInfo{ detail::platform_specific::default_fallback_font_info() },
		iGlyphAtlas{ aRenderingEngine.texture_manager(), size{1024.0, 1024.0} },
		iEmojiAtlas{ aRenderingEngine.texture_manager() }
	{
		FT_Error error = FT_Init_FreeType(&iFontLib);
		if (error)
		{
			throw error_initializing_font_library();
		}
		std::string fontsDirectory = detail::platform_specific::get_system_font_directory();
		for (boost::filesystem::directory_iterator file(fontsDirectory); file != boost::filesystem::directory_iterator(); ++file)
		{
			if (!boost::filesystem::is_regular_file(file->status())) 
				continue;
			try
			{
				if (is_font_file(file->path().string()))
				{
					auto font = iNativeFonts.emplace(iNativeFonts.end(), iRenderingEngine, iFontLib, file->path().string());
					iFontFamilies[neolib::make_ci_string(font->family_name())].push_back(font);
				}
			}
			catch (native_font::failed_to_load_font&)
			{
			}
			catch (...)
			{
				throw;
			}
		}
	}

	font_manager::~font_manager()
	{
		iFontFamilies.clear();
		iNativeFonts.clear();
		FT_Done_FreeType(iFontLib);
	}

	void* font_manager::font_library_handle() const
	{
		return iFontLib;
	}

	const font_info& font_manager::default_system_font_info() const
	{
		return iDefaultSystemFontInfo;
	}

	const i_fallback_font_info& font_manager::default_fallback_font_info() const
	{
		return iDefaultFallbackFontInfo;
	}

	std::unique_ptr<i_native_font_face> font_manager::create_default_font(const i_device_resolution& aDevice)
	{
		return create_font(app::instance().current_style().font_info(), aDevice);
	}

	bool font_manager::has_fallback_font(const i_native_font_face& aExistingFont) const
	{
		auto fallbackFontFamilyName = default_fallback_font_info().fallback_for(aExistingFont.family_name());
		return aExistingFont.family_name() != fallbackFontFamilyName && iFontFamilies.find(neolib::make_ci_string(fallbackFontFamilyName)) != iFontFamilies.end();
	}
		
	std::unique_ptr<i_native_font_face> font_manager::create_fallback_font(const i_native_font_face& aExistingFont)
	{
		if (aExistingFont.fallback_cached())
			return std::unique_ptr<i_native_font_face>(new detail::native_font_face_wrapper(aExistingFont.fallback()));
		struct : i_device_resolution
		{
			size iResolution;
			virtual dimension horizontal_dpi() const { return iResolution.cx; }
			virtual dimension vertical_dpi() const { return iResolution.cy; }
		} deviceResolution;
		deviceResolution.iResolution = size(aExistingFont.horizontal_dpi(), aExistingFont.vertical_dpi());
		auto fallbackFont = create_font(default_fallback_font_info().fallback_for(aExistingFont.family_name()), aExistingFont.style(), aExistingFont.size(), deviceResolution);
		fallbackFont->add_ref();
		return fallbackFont;
	}

	std::unique_ptr<i_native_font_face> font_manager::create_font(const std::string& aFamilyName, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice)
	{
		return std::unique_ptr<i_native_font_face>(new detail::native_font_face_wrapper(find_best_font(aFamilyName, aStyle, aSize).create_face(aStyle, aSize, aDevice)));
	}

	std::unique_ptr<i_native_font_face> font_manager::create_font(const std::string& aFamilyName, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice)
	{
		return std::unique_ptr<i_native_font_face>(new detail::native_font_face_wrapper(find_font(aFamilyName, aStyleName, aSize).create_face(aStyleName, aSize, aDevice)));
	}

	std::unique_ptr<i_native_font_face> font_manager::create_font(const font_info& aInfo, const i_device_resolution& aDevice)
	{
		if (aInfo.style_name_available())
			return create_font(aInfo.family_name(), aInfo.style_name(), aInfo.size(), aDevice);
		else
			return create_font(aInfo.family_name(), aInfo.style(), aInfo.size(), aDevice);
	}

	std::unique_ptr<i_native_font_face> font_manager::create_font(i_native_font& aFont, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice)
	{
		return std::unique_ptr<i_native_font_face>(new detail::native_font_face_wrapper(aFont.create_face(aStyle, aSize, aDevice)));
	}

	std::unique_ptr<i_native_font_face> font_manager::create_font(i_native_font& aFont, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice)
	{
		return std::unique_ptr<i_native_font_face>(new detail::native_font_face_wrapper(aFont.create_face(aStyleName, aSize, aDevice)));
	}

	bool font_manager::is_font_file(const std::string& aFileName) const
	{
		FT_Face face;
		FT_Error error = FT_New_Face(iFontLib, aFileName.c_str(), 0, &face);
		if (error)
			return false;
		FT_Done_Face(face);
		return true;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_file(const std::string& aFileName, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_file function overload not yet implemented");
		(void)aFileName;
		(void)aDevice;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_file(const std::string& aFileName, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_file function overload not yet implemented");
		(void)aFileName;
		(void)aStyle;
		(void)aSize;
		(void)aDevice;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_file(const std::string& aFileName, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_file function overload not yet implemented");
		(void)aFileName;
		(void)aStyleName;
		(void)aSize;
		(void)aDevice;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_memory(const void* aData, std::size_t aSizeInBytes, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_memory function overload not yet implemented");
		(void)aData;
		(void)aSizeInBytes;
		(void)aDevice;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_memory(const void* aData, std::size_t aSizeInBytes, font::style_e aStyle, font::point_size aSize, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_memory function overload not yet implemented");
		(void)aData;
		(void)aSizeInBytes;
		(void)aStyle;
		(void)aSize;
		(void)aDevice;
	}

	std::unique_ptr<i_native_font_face> font_manager::load_font_from_memory(const void* aData, std::size_t aSizeInBytes, const std::string& aStyleName, font::point_size aSize, const i_device_resolution& aDevice)
	{
		throw std::logic_error("neogfx::font_manager::load_font_from_memory function overload not yet implemented");
		(void)aData;
		(void)aSizeInBytes;
		(void)aStyleName;
		(void)aSize;
		(void)aDevice;
	}

	const i_texture_atlas& font_manager::glyph_atlas() const
	{
		return iGlyphAtlas;
	}

	i_texture_atlas& font_manager::glyph_atlas()
	{
		return iGlyphAtlas;
	}

	const i_emoji_atlas& font_manager::emoji_atlas() const
	{
		return iEmojiAtlas;
	}

	i_emoji_atlas& font_manager::emoji_atlas()
	{
		return iEmojiAtlas;
	}

	i_native_font& font_manager::find_font(const std::string& aFamilyName, const std::string& aStyleName, font::point_size aSize)
	{
		auto family = iFontFamilies.find(neolib::make_ci_string(aFamilyName));
		if (family == iFontFamilies.end())
			family = iFontFamilies.find(neolib::make_ci_string(default_system_font_info().family_name()));
		if (family == iFontFamilies.end())
			throw no_matching_font_found();
		std::multimap<uint32_t, native_font_list::iterator> matches;
		for (auto& f : family->second)
		{
			for (std::size_t s = 0; s < f->style_count(); ++s)
				if (neolib::make_ci_string(f->style_name(s)) == neolib::make_ci_string(aStyleName))
					return *f;
		}
		return find_best_font(aFamilyName, font::Normal, aSize);
	}

	namespace
	{
		uint32_t matching_bits(uint32_t lhs, uint32_t rhs)
		{
			if (lhs == rhs)
				return 32;
			uint32_t matches = 0;
			uint32_t test = 1;
			while (test != 0)
			{
				if ((lhs & rhs) & test)
					++matches;
				test <<= 1;
			}
			return matches;
		}
	}

	i_native_font& font_manager::find_best_font(const std::string& aFamilyName, font::style_e aStyle, font::point_size)
	{
		auto family = iFontFamilies.find(neolib::make_ci_string(aFamilyName));
		if (family == iFontFamilies.end())
			family = iFontFamilies.find(neolib::make_ci_string(default_system_font_info().family_name()));
		if (family == iFontFamilies.end())
			throw no_matching_font_found();
		std::multimap<std::pair<uint32_t, uint32_t>, native_font_list::iterator> matches;
		uint32_t pos = std::numeric_limits<uint32_t>::max();
		for (auto& f : family->second)
		{
			for (std::size_t s = 0; s < f->style_count(); ++s)
				matches.insert(std::make_pair(std::make_pair(matching_bits(f->style(s), aStyle), pos--), f));
		}
		if (matches.empty())
			throw no_matching_font_found();
		return *matches.rbegin()->second;
	}
}