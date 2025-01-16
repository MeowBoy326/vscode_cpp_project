// texture_atlas.cpp
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
#include <neogfx_texture_atlas.hpp>
#include <neogfx_image.hpp>

namespace neogfx
{
	texture_atlas::texture_atlas(i_texture_manager& aTextureManager, const size& aPageSize) :
		iTextureManager(aTextureManager), iPageSize(aPageSize), iNextId(0u)
	{
	}

	const i_sub_texture& texture_atlas::sub_texture(i_sub_texture::id aSubTextureId) const
	{
		auto iterEntry = iEntries.find(aSubTextureId);
		if (iterEntry == iEntries.end())
			throw sub_texture_not_found();
		return iterEntry->second.second;
	}

	i_sub_texture& texture_atlas::sub_texture(i_sub_texture::id aSubTextureId)
	{
		auto iterEntry = iEntries.find(aSubTextureId);
		if (iterEntry == iEntries.end())
			throw sub_texture_not_found();
		return iterEntry->second.second;
	}

	i_sub_texture& texture_atlas::create_sub_texture(const size& aSize, texture_sampling aSampling)
	{
		auto newSpace = allocate_space(aSize, aSampling);
		++iNextId;
		auto entry = iEntries.insert(std::make_pair(iNextId, std::make_pair(newSpace.first, neogfx::sub_texture{ iNextId, newSpace.first->first, newSpace.second, aSize })));
		return entry.first->second.second;
	}

	i_sub_texture& texture_atlas::create_sub_texture(const i_image& aImage)
	{
		auto newSpace = allocate_space(aImage.extents(), aImage.sampling());
		++iNextId;
		auto entry = iEntries.insert(std::make_pair(iNextId, std::make_pair(newSpace.first, neogfx::sub_texture{ iNextId, newSpace.first->first, newSpace.second, aImage.extents() })));
		entry.first->second.second.set_pixels(aImage);
		return entry.first->second.second;
	}

	void texture_atlas::destroy_sub_texture(i_sub_texture& aSubTexture)
	{
		auto iterEntry = iEntries.find(aSubTexture.atlas_id());
		if (iterEntry == iEntries.end())
			throw sub_texture_not_found();
		auto rectEntry = iterEntry->second.second.atlas_location();
		auto space = iterEntry->second.first->second.used.find(rectEntry);
		if (space != iterEntry->second.first->second.used.end())
			iterEntry->second.first->second.used.erase(space);
		iterEntry->second.first->second.freed.insert(rectEntry);
		iEntries.erase(iterEntry);
	}

	const size& texture_atlas::page_size() const
	{
		return iPageSize;
	}

	texture_atlas::pages::iterator texture_atlas::create_page(texture_sampling aSampling)
	{
		return iPages.insert(iPages.end(), page{ texture{ page_size(), aSampling }, fragments{ page_size() } });
	}

	std::pair<texture_atlas::pages::iterator, rect> texture_atlas::allocate_space(const size& aSize, texture_sampling aSampling)
	{
		if (iPages.empty())
			create_page(aSampling);
		rect result;
		for (auto iterPage = iPages.begin(); iterPage != iPages.end(); ++iterPage)
			if (iterPage->first.sampling() == aSampling && iterPage->second.insert(size{ std::max(std::pow(2.0, std::ceil(std::log2(aSize.cx + 2.0))), 16.0), std::max(std::pow(2.0, std::ceil(std::log2(aSize.cy + 2.0))), 16.0) }, result))
				return std::make_pair(iterPage, result);
		auto iterPage = create_page(aSampling);
		if (iterPage->second.insert(size{ std::max(std::pow(2.0, std::ceil(std::log2(aSize.cx + 2.0))), 16.0), std::max(std::pow(2.0, std::ceil(std::log2(aSize.cy + 2.0))), 16.0) }, result))
			return std::make_pair(iterPage, result);
		iPages.erase(iterPage);
		throw texture_too_big_for_atlas();
	}
}