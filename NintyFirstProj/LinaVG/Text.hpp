/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2022-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

#ifndef LINAVG_DISABLE_TEXT_SUPPORT

#include "Common.hpp"
#include <unordered_map>
#include <functional>
#include <mutex>

// #include <ft2build.h>
// #include FT_FREETYPE_H
#include "freetype_single.h"

namespace LinaVG
{
	typedef FT_ULong GlyphEncoding;

	struct TextCharacter
	{
		Vec4  m_uv12	= Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		Vec4  m_uv34	= Vec4(0.0f, 0.0f, 0.0f, 0.0f);
		Vec2  m_size	= Vec2(0.0f, 0.0f);
		Vec2  m_bearing = Vec2(0.0f, 0.0f);
		Vec2  m_advance = Vec2(0.0f, 0.0f);
		float m_ascent	= 0.0f;
		float m_descent = 0.0f;

		/// Cleaned after load.
		unsigned char* m_buffer = nullptr;
	};

	struct Callbacks
	{
		std::function<void(BackendHandle)>																					  fontTextureBind;
		std::function<BackendHandle(unsigned int width, unsigned int height)>												  fontTextureCreate;
		std::function<void(unsigned int glyphWidth, unsigned int glyphHeight, int offsetX, int offsetY, unsigned char* data)> fontTextureBufferData;
		std::function<void()>																								  fontTextureBufferEnd;
	};

	struct KerningInformation
	{
		LINAVG_MAP<unsigned long, unsigned long> xAdvances;
	};

	class LinaVGFont
	{
	public:
		BackendHandle								  m_texture			= 0;
		int											  m_size			= 0;
		float										  m_newLineHeight	= 0.0f;
		float										  m_spaceAdvance	= 0.0f;
		bool										  m_supportsUnicode = false;
		bool										  m_isSDF			= false;
		bool										  m_supportsKerning = false;
		LINAVG_MAP<GlyphEncoding, TextCharacter>	  m_characterGlyphs;
		LINAVG_MAP<unsigned long, KerningInformation> m_kerningTable;
	};

	struct FontAtlas
	{
		BackendHandle m_texture		   = 0;
		unsigned int  m_currentOffsetX = 0;
		unsigned int  m_currentOffsetY = 0;
		unsigned int  m_rowSizeY	   = 0;
	};

	/// <summary>
	/// Management for text rendering.
	/// </summary>
	struct TextData
	{
		Array<FontAtlas> m_createdAtlases;
	};

    extern LINAVG_API FT_Library g_ftLib;

    /// <summary>
    /// Call once during your app before loading any fonts to setup free type library.
    /// </summary>
    extern LINAVG_API bool InitializeText();

    /// <summary>
    /// Call before terminating the app, will clean up FreeType library.
    /// </summary>
    extern LINAVG_API void TerminateText();

	class Text
	{
    public:
        
        ~Text();
        
		/// <summary>
		/// Set these callbacks before you start loading any font to receive font atlas buffering data.
		/// </summary>
        LINAVG_API Callbacks& GetCallbacks() { return m_callbacks; }
	
		/// <summary>
		/// Loads the given font and generates textures based on given size.
		/// You can load the same font with different sizes to achieve varying text scales.
		/// Alternatively, you can use the scale modifier in TextOptions but it's not recommended to upscale.
		/// Best quality would be achieved by loading fonts with bigger sizes and scaling them (slightly) down using TextOptions.
		/// Its your responsibility to delete the returned font ptr.
		/// </summary>
		/// <param name="file">TTF or OTF file.</param>
		/// <param name="loadAsSDF">Creates an SDF font.</param>
		/// <param name="size">Font height, width will automatically adjust.</param>
		/// <param name="customRanges">Send custom ranges in UTF32 encoding, e.g. 0x1F028, to load specific characters or sets.</param>
		/// <param name="customRangesSize">Size of the range array, each 2 pair in the array is treated as a range. Needs to be power of 2! </param>
		/// <param name="useKerningIfAvailable">If the font face contains a kern table this font will be drawn using kerning information. </param>
		/// <returns></returns>
		LINAVG_API LinaVGFont* LoadFont(const char* file, bool loadAsSDF, int size = 48, GlyphEncoding* customRanges = nullptr, int customRangesSize = 0, bool useKerningIfAvailable = true);

		/// <summary>
		/// Loads the given font and generates textures based on given size.
		/// You can load the same font with different sizes to achieve varying text scales.
		/// Alternatively, you can use the scale modifier in TextOptions but it's not recommended to upscale.
		/// Best quality would be achieved by loading fonts with bigger sizes and scaling them (slightly) down using TextOptions.
		/// Its your responsibility to delete the returned font ptr.
		/// </summary>
		/// <param name="data">Binary font data.</param>
		/// <param name="size">Binary font data size.</param>
		/// <param name="loadAsSDF">Creates an SDF font.</param>
		/// <param name="size">Font height, width will automatically adjust.</param>
		/// <param name="customRanges">Send custom ranges in UTF32 encoding, e.g. 0x1F028, to load specific characters or sets.</param>
		/// <param name="customRangesSize">Size of the range array, each 2 pair in the array is treated as a range. Needs to be power of 2! </param>
		/// <param name="useKerningIfAvailable">If the font face contains a kern table this font will be drawn using kerning information. </param>
		/// <returns></returns>
		LINAVG_API LinaVGFont* LoadFontFromMemory(void* data, size_t dataSize, bool loadAsSDF, int size = 48, GlyphEncoding* customRanges = nullptr, int customRangesSize = 0, bool useKerningIfAvailable = true);

		/// <summary>
		/// Uses loaded face (from file or mem) to setup rest of the font data.
		/// </summary>
        LINAVG_API LinaVGFont* SetupFont(FT_Face& face, bool loadAsSDF, int size, GlyphEncoding* customRanges, int customRangesSize, bool useKerningIfAvailable);

		/// <summary>
		/// Returns the kerning vector between two given glphys.
		/// </summary>
		// LINAVG_API Vec2 GetKerning(LinaVGFont* font, int previousGlyph, int currentGlyph);
        
    private:
        
        Callbacks m_callbacks;
        TextData m_textData;

    }; // namespace Text

}; // namespace LinaVG

#endif
