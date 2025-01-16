// i_scrollbar.hpp
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
#include <neolib_optional.hpp>
#include <neolib_timer.hpp>
#include "i_scrollbar.hpp"
#include <neogfx_graphics_context.hpp>

namespace neogfx
{
	class scrollbar : public i_scrollbar
	{
	public:
		scrollbar(i_scrollbar_container& aContainer, scrollbar_type aType, scrollbar_style aStyle = scrollbar_style::Normal, bool aIntegerPositions = true);
	public:
		scrollbar_type type() const override;
		scrollbar_style style() const override;
		void show() override;
		void hide() override;
		bool visible() const override;
		value_type position() const override;
		bool set_position(value_type aPosition) override;
		value_type minimum() const override;
		void set_minimum(value_type aMinimum) override;
		value_type maximum() const override;
		void set_maximum(value_type aMaximum) override;
		value_type step() const override;
		void set_step(value_type aStep) override;
		value_type page() const override;
		void set_page(value_type aPage) override;
	public:
		dimension width(const i_units_context& aContext) const override;
		void render(graphics_context& aGraphicsContext) const override;
	public:
		rect element_geometry(const i_units_context& aContext, element_e aElement) const override;
		element_e element_at(const i_units_context& aContext, const point& aPosition) const override;
	public:
		void update(const i_units_context& aContext, const update_params_t& aUpdateParams = update_params_t()) override;
		element_e clicked_element() const override;
		void click_element(element_e aElement) override;
		void unclick_element() override;
		void hover_element(element_e aElement) override;
		void unhover_element() override;
		void pause() override;
		void resume() override;
		void track() override;
		void untrack() override;
	private:
		i_scrollbar_container& iContainer;
		scrollbar_type iType;
		scrollbar_style iStyle;
		bool iIntegerPositions;
		bool iVisible;
		value_type iPosition;
		value_type iMinimum;
		value_type iMaximum;
		value_type iStep;
		value_type iPage;
		element_e iClickedElement;
		element_e iHoverElement;
		boost::optional<std::shared_ptr<neolib::callback_timer>> iTimer;
		bool iPaused;
		point iThumbClickedPosition;
		value_type iThumbClickedValue;
		optional_point iScrollTrackPosition;
	};
}