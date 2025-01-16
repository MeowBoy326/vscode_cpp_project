// scrollbar.cpp
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
#include <neogfx_scrollbar.hpp>

namespace neogfx
{
	scrollbar::scrollbar(i_scrollbar_container& aContainer, scrollbar_type aType, scrollbar_style aStyle, bool aIntegerPositions) :
		iContainer(aContainer), 
		iType(aType), 
		iStyle(aStyle),
		iIntegerPositions(aIntegerPositions),
		iVisible(false),
		iPosition(0.0),
		iMinimum(0.0),
		iMaximum(0.0),
		iStep(1.0),
		iPage(0.0),
		iClickedElement(ElementNone),
		iHoverElement(ElementNone),
		iPaused(false)
	{
	}

	scrollbar_type scrollbar::type() const
	{
		return iType;
	}

	scrollbar_style scrollbar::style() const
	{
		return iStyle;
	}

	void scrollbar::show()
	{
		if (!iVisible && style() != scrollbar_style::Invisible)
		{
			iVisible = true;
			iContainer.scrollbar_updated(*this, Shown);
		}
	}

	void scrollbar::hide()
	{
		if (iVisible)
		{
			iVisible = false;
			iContainer.scrollbar_updated(*this, Hidden);
		}
	}

	bool scrollbar::visible() const
	{
		return iVisible;
	}

	scrollbar::value_type scrollbar::position() const
	{
		return iPosition;
	}

	bool scrollbar::set_position(value_type aPosition)
	{
		aPosition = std::max(std::min(aPosition, maximum() - page()), minimum());
		if (iIntegerPositions)
			aPosition = std::ceil(aPosition);
		bool changed = false;
		if (iPosition != aPosition)
		{
			changed = true;
			update_reason_e updateReason = (iPosition < aPosition ? ScrolledDown : ScrolledUp);
			iPosition = aPosition;
			iContainer.scrollbar_updated(*this, updateReason);
		}
		return changed;
	}

	scrollbar::value_type scrollbar::minimum() const
	{
		return iMinimum;
	}

	void scrollbar::set_minimum(value_type aMinimum)
	{
		if (iMinimum != aMinimum)
		{
			iMinimum = aMinimum;
			iContainer.scrollbar_updated(*this, AttributeChanged);
		}
	}

	scrollbar::value_type scrollbar::maximum() const
	{
		return iMaximum;
	}

	void scrollbar::set_maximum(value_type aMaximum)
	{
		if (iMaximum != aMaximum)
		{
			iMaximum = aMaximum;
			iContainer.scrollbar_updated(*this, AttributeChanged);
		}
	}

	scrollbar::value_type scrollbar::step() const
	{
		return iStep;
	}

	void scrollbar::set_step(value_type aStep)
	{
		if (iStep != aStep)
		{
			iStep = aStep;
			iContainer.scrollbar_updated(*this, AttributeChanged);
		}
	}

	scrollbar::value_type scrollbar::page() const
	{
		return iPage;
	}

	void scrollbar::set_page(value_type aPage)
	{
		if (iPage != aPage)
		{
			iPage = aPage;
			iContainer.scrollbar_updated(*this, AttributeChanged);
		}
	}

	dimension scrollbar::width(const i_units_context& aContext) const
	{
		if (style() == scrollbar_style::Invisible)
			return 0.0;
		units_converter uc(aContext);
		uc.set_units(UnitsMillimetres);
		dimension w = std::ceil(uc.to_device_units(4.0));
		if (style() == scrollbar_style::Menu || style() == scrollbar_style::Scroller)
		{
			const dimension margin = 3.0;
			w -= margin * 2.0;
		}
		if (static_cast<uint32_t>(w) % 2 == 0)
			++w;
		uc.set_units(uc.saved_units());
		return uc.from_device_units(w);
	}

	void scrollbar::render(graphics_context& aGraphicsContext) const
	{
		if (style() == scrollbar_style::Invisible)
			return;
		scoped_units su(aGraphicsContext, UnitsPixels);
		rect g = iContainer.scrollbar_geometry(aGraphicsContext, *this);
		point oldOrigin = aGraphicsContext.origin();
		aGraphicsContext.set_origin(point(0.0, 0.0));
		colour baseColour = iContainer.scrollbar_colour(*this);
		colour backgroundColour = baseColour.light() ? baseColour.darker(0x40) : baseColour.lighter(0x40);
		if (iContainer.as_widget().transparent_background())
			backgroundColour = iContainer.as_widget().container_background_colour();
		colour foregroundColour = baseColour.light() ? baseColour.darker(0x80) : baseColour.lighter(0x80);
		if (style() == scrollbar_style::Normal)
			aGraphicsContext.fill_rect(iContainer.scrollbar_geometry(aGraphicsContext, *this), backgroundColour);
		else if (style() == scrollbar_style::Menu)
		{
			auto g1 = g;
			auto g2 = g;
			if (type() == scrollbar_type::Vertical)
			{
				g1.cy = width(aGraphicsContext);
				g2.y = g2.y + g2.cy - width(aGraphicsContext);
				g2.cy = g1.cy;
			}
			else
			{
				g1.cx = width(aGraphicsContext);
				g2.x = g2.x + g2.cx - width(aGraphicsContext);
				g2.cx = g1.cx;
			}
			aGraphicsContext.fill_rect(g1, backgroundColour);
			aGraphicsContext.fill_rect(g2, backgroundColour);
		}
		else if (style() == scrollbar_style::Scroller)
		{
			auto g1 = g;
			auto g2 = g;
			if (type() == scrollbar_type::Vertical)
			{
				g1.y = g1.y + g1.cy - width(aGraphicsContext) * 2.0;
				g1.cy = width(aGraphicsContext);
				g2.y = g2.y + g2.cy - width(aGraphicsContext);
				g2.cy = g1.cy;
			}
			else
			{
				g1.x = g1.x + g1.cx - width(aGraphicsContext) * 2.0;
				g1.cx = width(aGraphicsContext);
				g2.x = g2.x + g2.cx - width(aGraphicsContext);
				g2.cx = g1.cx;
			}
			aGraphicsContext.fill_rect(g1, backgroundColour);
			aGraphicsContext.fill_rect(g2, backgroundColour);
			if (position() != minimum())
			{
				auto fadeRect = g;
				if (type() == scrollbar_type::Vertical)
					fadeRect.cy = width(aGraphicsContext);
				else
					fadeRect.cx = width(aGraphicsContext);
				aGraphicsContext.fill_rect(fadeRect, gradient
				{
					gradient::colour_stop_list{ { 0.0, backgroundColour }, { 1.0, backgroundColour } },
					gradient::alpha_stop_list{ { 0.0, 0xFF }, { 1.0, 0x00} },
					type() == scrollbar_type::Vertical ? gradient::Vertical : gradient::Horizontal
				});
			}
			if (position() != maximum() - page())
			{
				auto fadeRect = g;
				if (type() == scrollbar_type::Vertical)
				{
					fadeRect.y = fadeRect.y + fadeRect.cy - width(aGraphicsContext) * 3.0;
					fadeRect.cy = width(aGraphicsContext);
				}
				else
				{
					fadeRect.x = fadeRect.x + fadeRect.cx - width(aGraphicsContext) * 3.0;
					fadeRect.cx = width(aGraphicsContext);
				}
				aGraphicsContext.fill_rect(fadeRect, gradient
				{
					gradient::colour_stop_list{ { 0.0, backgroundColour }, { 1.0, backgroundColour } },
					gradient::alpha_stop_list{ { 0.0, 0x00 },{ 1.0, 0xFF } },
					type() == scrollbar_type::Vertical ? gradient::Vertical : gradient::Horizontal
				});
			}
		}

		const dimension margin = 3.0;
		rect rectUpButton = element_geometry(aGraphicsContext, ElementUpButton).deflate(margin, margin);
		rect rectDownButton = element_geometry(aGraphicsContext, ElementDownButton).deflate(margin, margin);
		if (type() == scrollbar_type::Vertical)
		{
			coordinate x = std::floor(rectUpButton.centre().x);
			coordinate w = 1.0;
			for (coordinate y = 0.0; y < rectUpButton.height(); ++y)
			{
				aGraphicsContext.fill_rect(rect(point(x, std::floor(y + rectUpButton.top())), size(w, 1.0)), baseColour.light() ?
					foregroundColour.darker(position() == minimum() ? 0x00 : iClickedElement == ElementUpButton ? 0x60 : iHoverElement == ElementUpButton ? 0x30 : 0x00) :
					foregroundColour.lighter(position() == minimum() ? 0x00 : iClickedElement == ElementUpButton ? 0x60 : iHoverElement == ElementUpButton ? 0x30 : 0x00));
				aGraphicsContext.fill_rect(rect(point(x, std::floor(rectDownButton.bottom() - y)), size(w, 1.0)), baseColour.light() ?
					foregroundColour.darker(position() == maximum() - page() ? 0x00 : iClickedElement == ElementDownButton ? 0x60 : iHoverElement == ElementDownButton ? 0x30 : 0x00) :
					foregroundColour.lighter(position() == maximum() - page() ? 0x00 : iClickedElement == ElementDownButton ? 0x60 : iHoverElement == ElementDownButton ? 0x30 : 0x00));
				x -= 1.0;
				w += 2.0;
			}
		}
		else
		{
			coordinate y = std::floor(rectUpButton.centre().y);
			coordinate h = 1.0;
			for (coordinate x = 0.0; x < rectUpButton.width(); ++x)
			{
				aGraphicsContext.fill_rect(rect(point(std::floor(x + rectUpButton.left()), y), size(1.0, h)), baseColour.light() ?
					foregroundColour.darker(position() == minimum() ? 0x00 : iClickedElement == ElementUpButton ? 0x60 : iHoverElement == ElementUpButton ? 0x30 : 0x00) :
					foregroundColour.lighter(position() == minimum() ? 0x00 : iClickedElement == ElementUpButton ? 0x60 : iHoverElement == ElementUpButton ? 0x30 : 0x00));
				aGraphicsContext.fill_rect(rect(point(std::floor(rectDownButton.right() - x), y), size(1.0, h)), baseColour.light() ?
					foregroundColour.darker(position() == maximum() - page() ? 0x00 : iClickedElement == ElementDownButton ? 0x60 : iHoverElement == ElementDownButton ? 0x30 : 0x00) :
					foregroundColour.lighter(position() == maximum() - page() ? 0x00 : iClickedElement == ElementDownButton ? 0x60 : iHoverElement == ElementDownButton ? 0x30 : 0x00));
				y -= 1.0;
				h += 2.0;
			}
		}
		if (style() == scrollbar_style::Normal)
			aGraphicsContext.fill_rect(element_geometry(aGraphicsContext, ElementThumb).deflate(iType == scrollbar_type::Vertical ? margin : 0.0, iType == scrollbar_type::Vertical ? 0.0 : margin), baseColour.light() ?
				foregroundColour.darker(iClickedElement == ElementThumb ? 0x60 : iHoverElement == ElementThumb ? 0x30 : 0x00) :
				foregroundColour.lighter(iClickedElement == ElementThumb ? 0x60 : iHoverElement == ElementThumb ? 0x30 : 0x00));
		aGraphicsContext.set_origin(oldOrigin);
	}

	rect scrollbar::element_geometry(const i_units_context& aContext, element_e aElement) const
	{
		if (style() == scrollbar_style::Invisible)
			return rect{};
		scoped_units su(aContext, UnitsPixels);
		rect g = iContainer.scrollbar_geometry(aContext, *this);
		const dimension margin = 3.0;
		switch (aElement)
		{
		case ElementUpButton:
			if (iType == scrollbar_type::Vertical)
			{
				if (iStyle == scrollbar_style::Normal)
					g.cy = std::ceil((g.cx - margin * 2.0) / 2.0 + margin * 2.0);
				else if (iStyle == scrollbar_style::Menu)
					g.cy = std::ceil(width(aContext));
				else if (iStyle == scrollbar_style::Scroller)
				{
					g.y = g.bottom() - std::ceil(width(aContext)) * 2.0;
					g.cy = std::ceil(width(aContext));
				}
			}
			else
			{
				if (iStyle == scrollbar_style::Normal)
					g.cx = std::ceil((g.cy - margin * 2.0) / 2.0 + margin * 2.0);
				else if (iStyle == scrollbar_style::Menu)
					g.cx = std::ceil(width(aContext));
				else if (iStyle == scrollbar_style::Scroller)
				{
					g.x = g.right() - std::ceil(width(aContext)) * 2.0;
					g.cx = std::ceil(width(aContext));;
				}
			}
			break;
		case ElementDownButton:
			if (iType == scrollbar_type::Vertical)
			{
				if (iStyle == scrollbar_style::Normal)
				{
					g.y = g.bottom() - std::ceil((g.cx - margin * 2.0) / 2.0 + margin * 2.0);
					g.cy = std::ceil((g.cx - margin * 2.0) / 2.0 + margin * 2.0);
				}
				else if (iStyle == scrollbar_style::Menu || iStyle == scrollbar_style::Scroller)
				{
					g.y = g.bottom() - std::ceil(width(aContext));
					g.cy = std::ceil(width(aContext));
				}
			}
			else
			{
				if (iStyle == scrollbar_style::Normal)
				{
					g.x = g.right() - std::ceil((g.cy - margin * 2.0) / 2.0 + margin * 2.0);
					g.cx = std::ceil((g.cy - margin * 2.0) / 2.0 + margin * 2.0);
				}
				else if (iStyle == scrollbar_style::Menu || iStyle == scrollbar_style::Scroller)
				{
					g.x = g.right() - std::ceil(width(aContext));
					g.cx = std::ceil(width(aContext));
				}
			}
			break;
		case ElementPageUpArea:
			if (iType == scrollbar_type::Vertical)
			{
				g.y = element_geometry(aContext, ElementUpButton).bottom() + 1.0;
				g.cy = element_geometry(aContext, ElementThumb).top() - 1.0 - g.y;
			}
			else
			{
				g.x = element_geometry(aContext, ElementLeftButton).right() + 1.0;
				g.cx = element_geometry(aContext, ElementThumb).left() - 1.0 - g.x;
			}
			break;
		case ElementPageDownArea:
			if (iType == scrollbar_type::Vertical)
			{
				g.y = element_geometry(aContext, ElementThumb).bottom() + 1.0;
				g.cy = element_geometry(aContext, ElementDownButton).top() - 1.0 - g.y;
			}
			else
			{
				g.x = element_geometry(aContext, ElementThumb).right() + 1.0;
				g.cx = element_geometry(aContext, ElementRightButton).left() - 1.0 - g.x;
			}
			break;
		case ElementThumb:
			if (iType == scrollbar_type::Vertical)
			{
				g.y = element_geometry(aContext, ElementUpButton).bottom() + 1.0;
				dimension available = element_geometry(aContext, ElementDownButton).top() - 1.0 - g.y;
				if ((maximum() - minimum()) != 0.0)
				{
					g.cy = std::ceil(available * static_cast<dimension>(page() / (maximum() - minimum())));
					units_converter uc(aContext);
					uc.set_units(UnitsMillimetres);
					dimension s = std::ceil(uc.to_device_units(2.0));
					uc.set_units(uc.saved_units());
					if (g.cy < s)
					{
						available -= (s - g.cy);
						g.cy = s;
					}
					g.y += std::ceil(static_cast<dimension>(position() / (maximum() - minimum())) * available);
				}
				else
				{
					g.y = 0;
					g.cy = 0.0;
				}
			}
			else
			{
				g.x = element_geometry(aContext, ElementLeftButton).right() + 1.0;
				dimension available = element_geometry(aContext, ElementRightButton).left() - 1.0 - g.x;
				if ((maximum() - minimum()) != 0.0)
				{
					g.cx = std::ceil(available * static_cast<dimension>(page() / (maximum() - minimum())));
					units_converter uc(aContext);
					uc.set_units(UnitsMillimetres);
					dimension s = std::ceil(uc.to_device_units(2.0));
					uc.set_units(uc.saved_units());
					if (g.cx < s)
					{
						available -= (s - g.cx);
						g.cx = s;
					}
					g.x += std::ceil(static_cast<dimension>(position() / (maximum() - minimum())) * available);
				}
				else
				{
					g.x = 0;
					g.cx = 0.0;
				}
			}
			break;
		}
		return convert_units(aContext, su.saved_units(), g);
	}

	scrollbar::element_e scrollbar::element_at(const i_units_context& aContext, const point& aPosition) const
	{
		if (style() == scrollbar_style::Invisible)
			return ElementNone;
		else if (element_geometry(aContext, ElementUpButton).contains(aPosition))
			return ElementUpButton;
		else if (element_geometry(aContext, ElementDownButton).contains(aPosition))
			return ElementDownButton;
		else if (style() == scrollbar_style::Normal)
		{
			if (element_geometry(aContext, ElementPageUpArea).contains(aPosition))
				return ElementPageUpArea;
			else if (element_geometry(aContext, ElementPageDownArea).contains(aPosition))
				return ElementPageDownArea;
			else if (element_geometry(aContext, ElementThumb).contains(aPosition))
				return ElementThumb;
			else
				return ElementNone;
		}
		else
			return ElementNone;
	}

	void scrollbar::update(const i_units_context& aContext, const update_params_t& aUpdateParams)
	{
		if (!visible())
			return;
		if (clicked_element() != i_scrollbar::ElementNone && clicked_element() != element_at(aContext, iContainer.as_widget().surface().mouse_position()))
			pause();
		else
			resume();
		if (clicked_element() == i_scrollbar::ElementThumb)
		{
			point delta = (aUpdateParams.is<point>() ? static_variant_cast<point>(aUpdateParams) : iContainer.as_widget().surface().mouse_position()) - iThumbClickedPosition;
			scoped_units su(aContext, UnitsPixels);
			rect g = iContainer.scrollbar_geometry(aContext, *this);
			if (iType == scrollbar_type::Vertical)
			{
				g.y = element_geometry(aContext, ElementUpButton).bottom() + 1.0;
				g.cy = element_geometry(aContext, ElementDownButton).top() - 1.0 - g.y;
				g.cy -= (element_geometry(aContext, ElementThumb).cy - std::ceil(g.cy * static_cast<dimension>(page() / (maximum() - minimum()))));
				set_position(static_cast<value_type>(delta.y / g.height()) * (maximum() - minimum()) + iThumbClickedValue);
			}
			else
			{
				g.x = element_geometry(aContext, ElementLeftButton).right() + 1.0;
				g.cx = element_geometry(aContext, ElementRightButton).left() - 1.0 - g.x;
				g.cx -= (element_geometry(aContext, ElementThumb).cx - std::ceil(g.cx * static_cast<dimension>(page() / (maximum() - minimum()))));
				set_position(static_cast<value_type>(delta.x / g.width()) * (maximum() - minimum()) + iThumbClickedValue);
			}
			iContainer.scrollbar_updated(*this, Updated);
		}
		if (clicked_element() == i_scrollbar::ElementNone)
			hover_element(element_at(aContext, iContainer.as_widget().surface().mouse_position()));
		else
			unhover_element();
	}

	scrollbar::element_e scrollbar::clicked_element() const
	{
		return iClickedElement;
	}

	void scrollbar::click_element(element_e aElement)
	{
		if (iClickedElement != ElementNone)
			throw element_already_clicked();
		iClickedElement = aElement;
		switch (aElement)
		{
		case ElementUpButton:
			set_position(position() - step());
			iTimer = std::make_shared<neolib::callback_timer>(app::instance(), [this](neolib::callback_timer& aTimer)
			{
				aTimer.set_duration(50);
				aTimer.again();
				if (!iPaused)
					set_position(position() - step());
			}, 500);
			break;
		case ElementDownButton:
			set_position(position() + step());
			iTimer = std::make_shared<neolib::callback_timer>(app::instance(), [this](neolib::callback_timer& aTimer)
			{
				aTimer.set_duration(50);
				aTimer.again();
				if (!iPaused)
					set_position(position() + step());
			}, 500);
			break;
		case ElementPageUpArea:
			set_position(position() - page());
			iTimer = std::make_shared<neolib::callback_timer>(app::instance(), [this](neolib::callback_timer& aTimer)
			{
				aTimer.set_duration(50);
				aTimer.again();
				if (!iPaused)
					set_position(position() - page());
			}, 500);
			break;
		case ElementPageDownArea:
			set_position(position() + page());
			iTimer = std::make_shared<neolib::callback_timer>(app::instance(), [this](neolib::callback_timer& aTimer)
			{
				aTimer.set_duration(50);
				aTimer.again();
				if (!iPaused)
					set_position(position() + page());
			}, 500);
			break;
		case ElementThumb:
			iThumbClickedPosition = iContainer.as_widget().surface().mouse_position();
			iThumbClickedValue = position();
			break;
		default:
			break;
		}
	}

	void scrollbar::unclick_element()
	{
		if (iClickedElement == ElementNone)
			throw element_not_clicked();
		iClickedElement = ElementNone;
		iTimer.reset();
		iPaused = false;
	}

	void scrollbar::hover_element(element_e aElement)
	{
		if (iHoverElement != aElement)
		{
			iHoverElement = aElement;
			iContainer.scrollbar_updated(*this, Updated);
		}
	}

	void scrollbar::unhover_element()
	{
		if (iHoverElement != ElementNone)
		{
			iHoverElement = ElementNone;
			iContainer.scrollbar_updated(*this, Updated);
		}
	}

	void scrollbar::pause()
	{
		iPaused = true;
	}

	void scrollbar::resume()
	{
		iPaused = false;
	}

	void scrollbar::track()
	{
		if (iScrollTrackPosition == boost::none)
		{
			iScrollTrackPosition = iContainer.as_widget().surface().mouse_position();
			iTimer = std::make_shared<neolib::callback_timer>(app::instance(), [this](neolib::callback_timer& aTimer)
			{
				aTimer.again();
				point delta = iContainer.as_widget().surface().mouse_position() - *iScrollTrackPosition;
				scoped_units su(iContainer.as_widget().surface(), UnitsPixels);
				rect g = iContainer.scrollbar_geometry(iContainer.as_widget().surface(), *this);
				if (iType == scrollbar_type::Vertical)
				{
					g.y = element_geometry(iContainer.as_widget().surface(), ElementUpButton).bottom() + 1.0;
					g.cy = element_geometry(iContainer.as_widget().surface(), ElementDownButton).top() - 1.0 - g.y;
					set_position(position() + static_cast<value_type>(delta.y * 0.25f / g.height()) * (maximum() - minimum()));
				}
				else
				{
					g.x = element_geometry(iContainer.as_widget().surface(), ElementUpButton).right() + 1.0;
					g.cx = element_geometry(iContainer.as_widget().surface(), ElementDownButton).left() - 1.0 - g.x;
					set_position(position() + static_cast<value_type>(delta.x * 0.25f / g.width()) * (maximum() - minimum()));
				}
			}, 50);
		}
	}

	void scrollbar::untrack()
	{
		if (iScrollTrackPosition != boost::none)
		{
			iScrollTrackPosition.reset();
			iTimer.reset();
		}
	}
}