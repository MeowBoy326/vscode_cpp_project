// view.hpp
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
#include <neogfx_scrollable_widget.hpp>
#include "i_view.hpp"

namespace neogfx
{
	class i_controller;

	class view : public i_view, public scrollable_widget
	{
	public:
		view(i_controller& aController, i_model& aModel);
	public:
		const i_widget& as_widget() const override;
		i_widget& as_widget() override;
		const i_model& model() const override;
		i_model& model() override;
	public:
		bool is_active() const override;
		void activate() override;
		void deactivate() override;
	private:
		i_controller& iController;
		i_model& iModel;
		bool iActive;
	};
}