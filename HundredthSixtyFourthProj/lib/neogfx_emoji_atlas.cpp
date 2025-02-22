// emoji_atlas.cpp
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

#include <neogfx.hpp>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <neolib_string_utils.hpp>
#include <neolib_file.hpp>
#include <neolib_zip.hpp>
#include <neogfx_image.hpp>
#include <neogfx_emoji_atlas.hpp>

namespace neogfx
{
	emoji_atlas::emoji_atlas(i_texture_manager& aTextureManager) : 
		kFilePath{ neolib::program_directory() + "/emoji.zip" },
		iTextureAtlas{ aTextureManager.create_texture_atlas(size{ 1024.0, 1024.0}) }
	{
		try
		{
			neolib::zip zipFile(kFilePath);
			std::istringstream metaDataFile{ zipFile.extract_to_string(zipFile.index_of("meta.json")) };
			boost::property_tree::ptree metaData;
			boost::property_tree::read_json(metaDataFile, metaData);
			for (auto const& set : metaData.get_child("sets"))
			{
				dimension size = set.second.get<dimension>("size");
				std::string location = set.second.get<std::string>("location");
				for (std::size_t i = 0; i < zipFile.file_count(); ++i)
				{
					auto const& filePath = zipFile.file_path(i);
					if (filePath.find(location) == 0)
					{
						std::u32string codePoints;
						std::vector<std::string> hexCodePoints;
						neolib::tokens(boost::filesystem::path(filePath).stem().string(), std::string("-"), hexCodePoints);
						for (auto const& hexCodePoint : hexCodePoints)
						{
							std::stringstream ss;
							ss << std::hex << hexCodePoint;
							uint32_t x;
							ss >> x;
							if (x < 256)
								break;
							codePoints.push_back(x);
						}
						if (!codePoints.empty())
						{
							iEmojis[codePoints][size] = filePath;
							iEmojiMap[codePoints] = boost::optional<emoji_id>{};
						}
					}
				}
			}
		}
		catch (...)
		{
		}
	}

	bool emoji_atlas::is_emoji(char32_t aCodePoint) const
	{
		std::u32string str;
		str += aCodePoint;
		return is_emoji(str);
	}

	bool emoji_atlas::is_emoji(const std::u32string& aCodePoints) const
	{
		return iEmojiMap.find(aCodePoints) != iEmojiMap.end();
	}

	emoji_atlas::emoji_id emoji_atlas::emoji(char32_t aCodePoint, dimension aDesiredSize) const
	{
		std::u32string str;
		str += aCodePoint;
		return emoji(str, aDesiredSize);
	}

	emoji_atlas::emoji_id emoji_atlas::emoji(const std::u32string& aCodePoints, dimension aDesiredSize) const
	{
		auto iterEmoji = iEmojiMap.find(aCodePoints);
		if (iterEmoji == iEmojiMap.end())
			throw emoji_not_found();
		if (!iterEmoji->second.is_initialized())
		{
			auto emojiFiles = iEmojis.find(aCodePoints);
			auto emojiFile = emojiFiles->second.lower_bound(aDesiredSize);
			if (emojiFile == emojiFiles->second.end())
				--emojiFile;
			iterEmoji->second = iTextureAtlas->create_sub_texture(neogfx::image("file:///" + kFilePath + "#" + emojiFile->second)).atlas_id();
		}
		return *iterEmoji->second;
	}

	const i_texture& emoji_atlas::emoji_texture(emoji_id aId) const
	{
		return iTextureAtlas->sub_texture(aId);
	}
}