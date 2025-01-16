// texture.cpp
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
#include <neogfx_app.hpp>
#include <neogfx_texture.hpp>
#include <neogfx_texture_manager.hpp>
#include "native/i_native_texture.hpp"

namespace neogfx
{
	texture::texture()
	{
	}

	texture::texture(const neogfx::size& aExtents, texture_sampling aSampling, const optional_colour& aColour) :
		iNativeTexture(app::instance().rendering_engine().texture_manager().create_texture(aExtents, aSampling, aColour))
	{
	}

	texture::texture(const i_texture& aTexture) :
		iNativeTexture(!aTexture.is_empty() ? aTexture.native_texture() : std::shared_ptr<i_native_texture>())
	{
	}

	texture::texture(const i_image& aImage) :
		iNativeTexture(app::instance().rendering_engine().texture_manager().create_texture(aImage))
	{
	}

	texture::~texture()
	{
	}

	i_texture::type_e texture::type() const
	{
		return Texture;
	}

	texture_sampling texture::sampling() const
	{
		if (is_empty())
			return texture_sampling::NormalMipmap;
		return native_texture()->sampling();
	}

	bool texture::is_empty() const
	{
		return iNativeTexture == nullptr;
	}

	size texture::extents() const
	{
		if (is_empty())
			return size{};
		return native_texture()->extents();
	}

	size texture::storage_extents() const
	{
		if (is_empty())
			return size{};
		return native_texture()->storage_extents();
	}

	void texture::set_pixels(const rect& aRect, const void* aPixelData)
	{
		native_texture()->set_pixels(aRect, aPixelData);
	}

	std::shared_ptr<i_native_texture> texture::native_texture() const
	{
		if (is_empty())
			throw texture_empty();
		return iNativeTexture;
	}
}