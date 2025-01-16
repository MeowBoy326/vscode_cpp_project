// i_layout.hpp
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
#include "i_widget_geometry.hpp"
#include <neogfx_event.hpp>

namespace neogfx
{
	class i_widget;
	class i_spacer;

	class layout_item;

	class i_layout : public i_widget_geometry, public i_units_context
	{
	public:
		event<> alignment_changed;
	public:
		typedef uint32_t item_index;
		typedef boost::optional<item_index> optional_item_index;
	protected:
		class item;
	public:
		struct bad_item_index : std::logic_error { bad_item_index() : std::logic_error("neogfx::i_layout::bad_item_index") {} };
		struct no_widget : std::logic_error { no_widget() : std::logic_error("neogfx::i_layout::no_widget") {} };
		struct wrong_item_type : std::logic_error { wrong_item_type() : std::logic_error("neogfx::i_layout::wrong_item_type") {} };
		struct item_not_found : std::logic_error { item_not_found() : std::logic_error("neogfx::i_layout::item_not_found") {} };
	public:
		virtual ~i_layout() {}
	public:	
		virtual i_widget* owner() const = 0;
		virtual void set_owner(i_widget* aOwner) = 0;
		virtual i_layout* parent() const = 0;
		virtual void set_parent(i_layout* aParent) = 0;
		virtual void add_item(i_widget& aWidget) = 0;
		virtual void add_item_at(item_index aPosition, i_widget& aWidget) = 0;
		virtual void add_item(std::shared_ptr<i_widget> aWidget) = 0;
		virtual void add_item_at(item_index aPosition, std::shared_ptr<i_widget> aWidget) = 0;
		virtual void add_item(i_layout& aLayout) = 0;
		virtual void add_item_at(item_index aPosition, i_layout& aLayout) = 0;
		virtual void add_item(std::shared_ptr<i_layout> aLayout) = 0;
		virtual void add_item_at(item_index aPosition, std::shared_ptr<i_layout> aLayout) = 0;
		virtual void add_item(i_spacer& aSpacer) = 0;
		virtual void add_item_at(item_index aPosition, i_spacer& aSpacer) = 0;
		virtual void add_item(std::shared_ptr<i_spacer> aSpacer) = 0;
		virtual void add_item_at(item_index aPosition, std::shared_ptr<i_spacer> aSpacer) = 0;
		virtual void add_item(const	layout_item& aItem) = 0;
		virtual i_spacer& add_spacer() = 0;
		virtual i_spacer& add_spacer_at(item_index aPosition) = 0;
		virtual void remove_item_at(item_index aIndex) = 0;
		virtual bool remove_item(i_layout& aItem) = 0;
		virtual bool remove_item(i_widget& aItem) = 0;
		virtual void remove_items() = 0;
		virtual item_index item_count() const = 0;
		virtual optional_item_index find_item(i_layout& aItem) const = 0;
		virtual optional_item_index find_item(i_widget& aItem) const = 0;
		virtual optional_item_index find_item(const layout_item& aItem) const = 0;
		virtual bool is_widget_at(item_index aIndex) const = 0;
		virtual const i_widget_geometry& get_item_at(item_index aIndex) const = 0;
		virtual i_widget_geometry& get_item_at(item_index aIndex) = 0;
		virtual const i_widget& get_widget_at(item_index aIndex) const = 0;
		virtual i_widget& get_widget_at(item_index aIndex) = 0;
		virtual const i_layout& get_layout_at(item_index aIndex) const = 0;
		virtual i_layout& get_layout_at(item_index aIndex) = 0;
	public:
		virtual size spacing() const = 0;
		virtual void set_spacing(const size& sSpacing) = 0;
		virtual bool always_use_spacing() const = 0;
		virtual void set_always_use_spacing(bool aAlwaysUseSpacing) = 0;
		virtual neogfx::alignment alignment() const = 0;
		virtual void set_alignment(neogfx::alignment aAlignment, bool aUpdateLayout = true) = 0;
	public:
		virtual void enable() = 0;
		virtual void disable() = 0;
		virtual bool enabled() const = 0;
		virtual void layout_items(const point& aPosition, const size& aSize) = 0;
		virtual uint32_t layout_id() const = 0;
		virtual void next_layout_id() = 0;
		virtual bool invalidated() const = 0;
		virtual void invalidate() = 0;
		virtual void validate() = 0;
		// helpers
	public:
		template <typename ItemType>
		void replace_item_at(item_index aPosition, ItemType&& aItem)
		{
			if (aPosition < item_count())
				remove_item_at(aPosition);
			add_item_at(aPosition, aItem);
		}
		template <typename WidgetT>
		const WidgetT& get_widget_at(item_index aIndex) const
		{
			return static_cast<const WidgetT&>(get_widget_at(aIndex));
		}
		template <typename WidgetT>
		WidgetT& get_widget_at(item_index aIndex)
		{
			return static_cast<WidgetT&>(get_widget_at(aIndex));
		}
	};
}