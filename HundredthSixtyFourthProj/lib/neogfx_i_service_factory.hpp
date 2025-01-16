// i_service_factory.hpp
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
#include <neolib_io_task.hpp>
#include <neogfx_i_basic_services.hpp>
#include <neogfx_i_keyboard.hpp>
#include <neogfx_i_rendering_engine.hpp>

namespace neogfx
{
	class i_service_factory
	{
	public:
		virtual std::unique_ptr<i_basic_services> create_basic_services(neolib::io_task& aAppTask) = 0;
		virtual std::unique_ptr<i_keyboard> create_keyboard() = 0;
		virtual std::unique_ptr<i_rendering_engine> create_rendering_engine(renderer aRenderer, bool aDoubleBufferedWindows, i_basic_services& aBasicServices, i_keyboard& aKeyboard) = 0;
	};

	i_service_factory& default_service_factory();
}