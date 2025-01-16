// emoji_atlas.hpp
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
#include <unordered_map>
#include <neogfx_i_texture_manager.hpp>
#include <neogfx_i_texture_atlas.hpp>
#include "i_emoji_atlas.hpp"

namespace neogfx
{
	class emoji_atlas : public i_emoji_atlas
	{
	private:
		typedef std::map<dimension, std::string> sets;
		typedef std::map<std::u32string, sets> emojis;
	public:
		emoji_atlas(i_texture_manager& aTextureManager);
	public:
		virtual bool is_emoji(char32_t aCodePoint) const;
		virtual bool is_emoji(const std::u32string& aCodePoints) const;
		virtual emoji_id emoji(char32_t aCodePoint, dimension aDesiredSize) const;
		virtual emoji_id emoji(const std::u32string& aCodePoints, dimension aDesiredSize = 64) const;
		virtual const i_texture& emoji_texture(emoji_id aId) const;
	private:
		const std::string kFilePath;
		std::unique_ptr<i_texture_atlas> iTextureAtlas;
		emojis iEmojis;
		mutable std::unordered_map<std::u32string, boost::optional<emoji_id>> iEmojiMap;
	};
}