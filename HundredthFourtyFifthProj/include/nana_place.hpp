/*
 *	An Implementation of Place for Layout
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 *
 *	@contributions:
 *	min/max and splitter bar initial weight by Ariel Vina-Rodriguez.
 */

#ifndef NANA_GUI_PLACE_HPP
#define NANA_GUI_PLACE_HPP
#include <nana_basis.hpp>
#include <utility>
#include <memory>
#include <functional>

namespace nana
{
	class widget;
	namespace detail
	{
		class place_agent
		{
		public:
			virtual ~place_agent() = default;
			virtual std::unique_ptr<nana::widget> create(nana::window) const = 0;
		};
	}

	template<typename Widget>
	class agent
		: public detail::place_agent
	{
	public:
		agent(std::function<void(Widget&)> initializer)
			: init_(std::move(initializer))
		{}

		agent(const nana::char_t* text)
			: text_(text)
		{}

		agent(nana::string text, std::function<void(Widget&)> initializer = {})
			: text_(std::move(text)), init_(std::move(initializer))
		{}

	private:
		std::unique_ptr<nana::widget> create(nana::window handle) const override
		{
			std::unique_ptr<Widget> ptr(new Widget(handle));
			ptr->caption(text_);
			if (init_)
				init_(*ptr);
			return std::move(ptr);
		}
	private:
		nana::string text_;
		std::function<void(Widget&)> init_;
	};

    ///  Layout managment - an object of class place is attached to a widget, and it automatically positions and resizes the children widgets.
	class place
		: ::nana::noncopyable
	{
		struct implement;

		class field_interface
		{
			field_interface(const field_interface&) = delete;
			field_interface& operator=(const field_interface&) = delete;
			field_interface(field_interface&&) = delete;
			field_interface& operator=(field_interface&&) = delete;
		public:
			field_interface() = default;
			virtual ~field_interface() = default;
			virtual field_interface& operator<<(const nana::char_t* label) = 0;
			virtual field_interface& operator<<(nana::string label) = 0;
			virtual field_interface& operator<<(window) = 0;
			virtual field_interface& fasten(window) = 0;
			
			template<typename Widget>
			field_interface& operator<<(const agent<Widget>& ag)
			{
				_m_add_agent(ag);
				return *this;
			}
		private:
			virtual void _m_add_agent(const detail::place_agent&) = 0;
		};
	public:
        ///  reference to a field manipulator which refers to a field object created by place 
		typedef field_interface & field_reference;

		place();
		place(window);///< Attaches to a specified widget.
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void bind(window handle);
		window window_handle() const;
        
		void div(const char* s);              ///< Divides the attached widget into fields.
		field_reference field(const char* name);///< Returns a field with the specified name.

		void field_visible(const char* filed_name, bool visible); ///<<Shows/Hides an existing field.
		bool field_visible(const char* field_name) const;	///<Determines whether the specified field is visible.

		void field_display(const char* field_name, bool display); ///<Displays/Discards an existing field.
		bool field_display(const char* field_name) const;	///<Determines whether the specified field is displayed.

		void collocate();                     ///< Layouts the widgets.

 		void erase(window handle);				///< Erases a window from field.

		field_reference operator[](const char* name); ///< Returns a field with the specified name. Equal to field();
	private:
		implement * impl_;
	};
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
