// i_set.hpp - v1.0
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
#include "i_container.hpp"

namespace neolib
{
	template <typename T>
	class i_set : public i_container<T, i_const_iterator<T>, i_iterator<T> >
	{
	private:
		typedef i_container<T, i_const_iterator<T>, i_iterator<T> > base;
	public:
		typedef T abstract_key_type;
		typedef T abstract_value_type;
	protected:
		typedef typename base::abstract_const_iterator abstract_const_iterator;
		typedef typename base::abstract_iterator abstract_iterator;
	public:
		iterator insert(const abstract_value_type& aValue) { return do_insert(aValue); }
		const_iterator find(const abstract_key_type& aKey) const { return do_find(aKey); }
		iterator find(const abstract_key_type& aKey) { return do_find(aKey); }
	private:
		virtual abstract_iterator* do_insert(const abstract_value_type& aValue) = 0;
		virtual abstract_const_iterator* do_find(const abstract_key_type& aKey) const = 0;
		virtual abstract_iterator* do_find(const abstract_key_type& aKey) = 0;
	};

	template <typename T>
	class i_multiset : public i_container<T, i_const_iterator<T>, i_iterator<T> >
	{
	private:
		typedef i_container<T, i_const_iterator<T>, i_iterator<T> > base;
	public:
		typedef T abstract_key_type;
		typedef T abstract_value_type;
	protected:
		typedef typename base::abstract_const_iterator abstract_const_iterator;
		typedef typename base::abstract_iterator abstract_iterator;
	public:
		iterator insert(const abstract_value_type& aValue) { return do_insert(aValue); }
		const_iterator find(const abstract_key_type& aKey) const { return do_find(aKey); }
		iterator find(const abstract_key_type& aKey) { return do_find(aKey); }
	private:
		virtual abstract_iterator* do_insert(const abstract_value_type& aValue) = 0;
		virtual abstract_const_iterator* do_find(const abstract_key_type& aKey) const = 0;
		virtual abstract_iterator* do_find(const abstract_key_type& aKey) = 0;
	};
}
