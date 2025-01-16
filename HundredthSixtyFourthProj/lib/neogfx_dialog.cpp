// dialog.hpp
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
#include <neogfx_dialog.hpp>

namespace neogfx
{
	dialog::dialog(window_style aStyle) :
		window(size{}, aStyle)
	{
		init();
	}

	dialog::dialog(const std::string& aDialogTitle, window_style aStyle) :
		window(size{}, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::dialog(const size& aDimensions, window_style aStyle) :
		window(aDimensions, aStyle)
	{
		init();
	}

	dialog::dialog(const size& aDimensions, const std::string& aDialogTitle, window_style aStyle) :
		window(aDimensions, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::dialog(const point& aPosition, const size& aDimensions, window_style aStyle) :
		window(aPosition, aDimensions, aStyle)
	{
		init();
	}

	dialog::dialog(const point& aPosition, const size& aDimensions, const std::string& aDialogTitle, window_style aStyle) :
		window(aPosition, aDimensions, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, window_style aStyle) :
		window(aParent, size{}, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, const std::string& aDialogTitle, window_style aStyle) :
		window(aParent, size{}, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, const size& aDimensions, window_style aStyle) :
		window(aParent, aDimensions, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, const size& aDimensions, const std::string& aDialogTitle, window_style aStyle) :
		window(aParent, aDimensions, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, const point& aPosition, const size& aDimensions, window_style aStyle) :
		window(aParent, aPosition, aDimensions, aStyle)
	{
		init();
	}

	dialog::dialog(i_widget& aParent, const point& aPosition, const size& aDimensions, const std::string& aDialogTitle, window_style aStyle) :
		window(aParent, aPosition, aDimensions, aDialogTitle, aStyle)
	{
		init();
	}

	dialog::~dialog()
	{
	}

	void dialog::accept()
	{
		if (iResult != Accepted)
		{
			bool canAccept = true;
			try_accept.trigger(canAccept);
			if (canAccept)
				iResult = Accepted;
			else
				app::instance().basic_services().system_beep();
		}
	}

	void dialog::reject()
	{
		if (iResult != Rejected)
		{
			bool canReject = true;
			try_reject.trigger(canReject);
			if (canReject)
				iResult = Rejected;
			else
				app::instance().basic_services().system_beep();
		}
	}

	dialog_button_box& dialog::button_box()
	{
		if (iButtonBox == boost::none)
		{
			if (!has_layout())
				set_layout(std::make_shared<vertical_layout>());
			iButtonBox.emplace(layout());
			iButtonBox->accepted([this]()
			{
				accept();
			});
			iButtonBox->rejected([this]()
			{
				reject();
			});
		}
		return *iButtonBox;
	}

	dialog::result_code_e dialog::exec()
	{
		app::event_processing_context epc(app::instance(), "neogfx::dialog");
		while (iResult == boost::none)
		{
			app::instance().process_events(epc);
			if (surface().destroyed() && iResult == boost::none)
				iResult = Rejected;
		}
		return *iResult;
	}

	neogfx::size_policy dialog::size_policy() const
	{
		if (widget::has_size_policy())
			return widget::size_policy();
		return neogfx::size_policy::Minimum;
	}

	bool dialog::can_close() const
	{
		bool canReject = true;
		try_reject.trigger(canReject);
		return canReject;
	}

	bool dialog::key_pressed(scan_code_e aScanCode, key_code_e aKeyCode, key_modifiers_e aKeyModifiers)
	{
		switch (aScanCode)
		{
		case ScanCode_RETURN:
			accept();
			return true;
		case ScanCode_ESCAPE:
			reject();
			return true;
		default:
			return window::key_pressed(aScanCode, aKeyCode, aKeyModifiers);
		}
	}

	void dialog::init()
	{
	}
}