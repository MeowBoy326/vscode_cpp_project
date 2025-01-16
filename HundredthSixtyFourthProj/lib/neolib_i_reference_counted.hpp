// i_reference_counted.hpp - v1.0
/*
 *  Copyright (c) 2007 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "neolib.hpp"

namespace neolib
{
	class i_reference_counted
	{
	public:
		class i_object_destruction_watcher
		{
		public:
			virtual void object_being_destroyed(i_reference_counted& aObject) = 0;
		};
	public:
		struct too_many_references : std::logic_error { too_many_references() : std::logic_error("neolib::i_reference_counted::too_many_references") {} };
		struct release_during_destruction : std::logic_error { release_during_destruction() : std::logic_error("neolib::i_reference_counted::release_during_destruction") {} };
		struct destruction_watcher_already_subscribed : std::logic_error { destruction_watcher_already_subscribed() : std::logic_error("neolib::i_reference_counted::destruction_watcher_already_subscribed") {} };
		struct destruction_watcher_not_found : std::logic_error { destruction_watcher_not_found() : std::logic_error("neolib::i_reference_counted::destruction_watcher_not_found") {} };
	public:
		virtual ~i_reference_counted() {}
	public:
		virtual void add_ref() const = 0;
		virtual void release() const = 0;
		virtual const i_reference_counted* release_and_take_ownership() const = 0;
		virtual i_reference_counted* release_and_take_ownership() = 0;
		virtual void pin() const = 0;
		virtual void unpin() const = 0;
	public:
		virtual void subcribe_destruction_watcher(i_object_destruction_watcher& aWatcher) const = 0;
		virtual void unsubcribe_destruction_watcher(i_object_destruction_watcher& aWatcher) const = 0;
	};

	template <typename Interface>
	class i_auto_ref
	{
	public:
		struct no_object : std::logic_error { no_object() : std::logic_error("neolib::i_auto_ref::no_object") {} };
		struct interface_not_found : std::logic_error { interface_not_found() : std::logic_error("neolib::i_auto_ref::interface_not_found") {} };
	public:
		virtual bool reference_counted() const = 0;
		virtual void reset(Interface* aObject = 0, bool aReferenceCounted = true) = 0;
		virtual Interface* release() = 0;
		virtual bool valid() const = 0;
		virtual Interface* ptr() const = 0;
		virtual Interface* operator->() const = 0;
		virtual Interface& operator*() const = 0;
		template <typename Interface>
		bool operator==(const i_auto_ref<Interface>& aOther) const { return ptr() == aOther.ptr(); }
		template <typename Interface>
		bool operator!=(const i_auto_ref<Interface>& aOther) const { return ptr() != aOther.ptr(); }
		template <typename Interface>
		bool operator<(const i_auto_ref<Interface>& aOther) const { return ptr() < aOther.ptr(); }
	};

	template <typename Interface>
	class i_weak_auto_ref : public i_auto_ref<Interface>, protected i_reference_counted::i_object_destruction_watcher
	{
	public:
		struct bad_release : std::logic_error { bad_release() : std::logic_error("neolib::i_weak_auto_ref::bad_release") {} };
		struct wrong_object : std::logic_error { wrong_object() : std::logic_error("neolib::i_weak_auto_ref::wrong_object") {} };
	public:
		virtual void object_being_destroyed(i_reference_counted& aObject) = 0;
	};
}
