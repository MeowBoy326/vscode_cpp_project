// controller.cpp
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
#include <neogfx_controller.hpp>
#include <neogfx_i_view.hpp>

namespace neogfx
{
	controller::controller(i_model& aModel, i_view_container& aContainer) :
		iModel(aModel), iContainer(aContainer)
	{
	}

	const i_model& controller::model() const
	{
		return iModel;
	}

	i_model& controller::model()
	{
		return iModel;
	}

	void controller::add_view(i_view& aView)
	{
		iViews.push_back(std::shared_ptr<i_view>(std::shared_ptr<i_view>(), &aView));
		view_added.trigger(aView);
	}

	void controller::add_view(std::shared_ptr<i_view> aView)
	{
		iViews.push_back(aView);
		view_added.trigger(*aView);
	}

	void controller::remove_view(i_view& aView)
	{
		for (auto v = iViews.begin(); v != iViews.end(); ++v)
		{
			if (&**v == &aView)
			{
				view_removed.trigger(aView);
				iViews.erase(v);
				return;
			}
		}
		throw view_not_found();
	}

	bool controller::only_weak_views() const
	{
		for (const auto& v : iViews)
			if (!v->is_weak())
				return false;
		return true;
	}

	const i_view_container& controller::container() const
	{
		return iContainer;
	}

	i_view_container& controller::container()
	{
		return iContainer;
	}
}