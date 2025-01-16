/*
*  indexitor.hpp
*
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
#include <memory>
#include <iterator>
#include <type_traits>
#include "index_array_tree.hpp"

namespace neolib
{
	template <typename T, typename ForeignIndex, typename Alloc = std::allocator<std::pair<T, const ForeignIndex>>>
	class indexitor : private index_array_tree<ForeignIndex, Alloc>
	{
	public:
		typedef T data_type;
		typedef ForeignIndex foreign_index_type;
		typedef std::pair<data_type, const foreign_index_type> value_type;
		typedef std::pair<const foreign_index_type, const foreign_index_type> skip_type;
		typedef Alloc allocator_type;
		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::const_reference const_reference;
		typedef typename allocator_type::const_pointer const_pointer;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
	private:
		typedef index_array_tree<ForeignIndex, Alloc> base;
		class node : public base::node
		{
		public:
			node(const value_type& aValue, const skip_type& aSkip = skip_type{}) :
				iValue{aValue}, iSkip{aSkip}
			{
			}

		public:
			const value_type& value() const
			{
				return iValue;
			}
			value_type& value()
			{
				return iValue;
			}
			const skip_type& skip() const
			{
				return iSkip;
			}
			skip_type& skip()
			{
				return iSkip;
			}

		private:
			value_type iValue;
			skip_type iSkip;
		};
		typedef typename allocator_type:: template rebind<node>::other node_allocator_type;
	public:
		class iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference>
		{
			friend indexitor;
			friend class const_iterator;
		private:
			typedef std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> base;

		public:
			iterator() :
				iContainer{}, iNode{}, iContainerPosition{ 0 }
			{
			}
			iterator(const iterator& aOther) :
				iContainer{ aOther.iContainer }, iNode{ aOther.iNode }, iContainerPosition{ aOther.iContainerPosition }
			{
			}
			iterator& operator=(const iterator& aOther)
			{
				iContainer = aOther.iContainer;
				iNode = aOther.iNode;
				iContainerPosition = aOther.iContainerPosition;
				return *this;
			}
		private:
			iterator(indexitor& aContainer, size_type aContainerPosition) :
				iContainer{ &aContainer }, iNode{ nullptr }, iContainerPosition{ aContainerPosition }
			{
				iNode = iContainer->find_node(aContainerPosition);
				if (iNode->is_nil())
					*this = iContainer->end();
			}
			iterator(indexitor& aContainer, node* aNode) :
				iContainer{ &aContainer }, iNode{ aNode }, iContainerPosition{ iContainer->index(*this) }
			{
			}
			iterator(indexitor& aContainer, node* aNode, size_type aContainerPosition) :
				iContainer{ &aContainer }, iNode{ aNode }, iContainerPosition{ aContainerPosition }
			{
			}

		public:
			iterator& operator++()
			{
				iNode = static_cast<node*>(iNode->next());
				++iContainerPosition;
				return *this;
			}
			iterator& operator--()
			{
				iNode = static_cast<node*>(iNode != nullptr ? iNode->previous() : iContainer->back_node());
				--iContainerPosition;
				return *this;
			}
			iterator operator++(int) { iterator ret{*this}; operator++(); return ret; }
			iterator operator--(int) { iterator ret{*this}; operator--(); return ret; }
			iterator& operator+=(difference_type aDifference)
			{
				if (aDifference < 0)
					return operator-=(-aDifference);
				*this = iterator{*iContainer, container_position() + aDifference};
				return *this;
			}
			iterator& operator-=(difference_type aDifference)
			{
				if (aDifference < 0)
					return operator+=(-aDifference);
				*this = iterator{*iContainer, container_position() - aDifference};
				return *this;
			}
			iterator operator+(difference_type aDifference) const { iterator result{*this}; result += aDifference; return result; }
			iterator operator-(difference_type aDifference) const { iterator result{*this}; result -= aDifference; return result; }
			reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
			difference_type operator-(const iterator& aOther) const { return static_cast<difference_type>(container_position())-static_cast<difference_type>(aOther.container_position()); }
			reference operator*() const { return iNode->value(); }
			pointer operator->() const { return &operator*(); }
			bool operator==(const iterator& aOther) const { return container_position() == aOther.container_position(); }
			bool operator!=(const iterator& aOther) const { return container_position() != aOther.container_position(); }
			bool operator<(const iterator& aOther) const { return container_position() < aOther.container_position(); }
			bool operator<=(const iterator& aOther) const { return container_position() <= aOther.container_position(); }
			bool operator>(const iterator& aOther) const { return container_position() > aOther.container_position(); }
			bool operator>=(const iterator& aOther) const { return container_position() >= aOther.container_position(); }

		private:
			size_type container_position() const { return iContainerPosition; }

		private:
			indexitor* iContainer;
			node* iNode;
			size_type iContainerPosition;
		};
		class const_iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{
			friend indexitor;
		private:
			typedef std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> base;

		public:
			const_iterator() :
				iContainer{}, iNode{}, iContainerPosition{ 0 }
			{
			}
			const_iterator(const const_iterator& aOther) :
				iContainer{ aOther.iContainer }, iNode{ aOther.iNode }, iContainerPosition{ aOther.iContainerPosition }
			{
			}
			const_iterator(const typename indexitor::iterator& aOther) :
				iContainer{ aOther.iContainer }, iNode{ aOther.iNode }, iContainerPosition{ aOther.iContainerPosition }
			{
			}
			const_iterator& operator=(const const_iterator& aOther)
			{
				iContainer = aOther.iContainer;
				iNode = aOther.iNode;
				iContainerPosition = aOther.iContainerPosition;
				return *this;
			}
			const_iterator& operator=(const typename indexitor::iterator& aOther)
			{
				iContainer = aOther.iContainer;
				iNode = aOther.iNode;
				iContainerPosition = aOther.iContainerPosition;
				return *this;
			}
		private:
			const_iterator(const indexitor& aContainer, size_type aContainerPosition) :
				iContainer{ &aContainer }, iNode{ nullptr }, iContainerPosition{ aContainerPosition }
			{
				iNode = iContainer->find_node(aContainerPosition);
				if (iNode->is_nil())
					*this = iContainer->end();
			}
			const_iterator(const indexitor& aContainer, node* aNode) :
				iContainer{ &aContainer }, iNode{ aNode }, iContainerPosition{ iContainer->index(*this) }
			{
			}
			const_iterator(const indexitor& aContainer, node* aNode, size_type aContainerPosition) :
				iContainer{ &aContainer }, iNode{ aNode }, iContainerPosition{ aContainerPosition }
			{
			}

		public:
			const_iterator& operator++()
			{
				iNode = static_cast<node*>(iNode->next());
				++iContainerPosition;
				return *this;
			}
			const_iterator& operator--()
			{
				iNode = static_cast<node*>(iNode != nullptr ? iNode->previous() : iContainer->back_node());
				--iContainerPosition;
				return *this;
			}
			const_iterator operator++(int) { const_iterator ret{*this}; operator++(); return ret; }
			const_iterator operator--(int) { const_iterator ret{*this}; operator--(); return ret; }
			const_iterator& operator+=(difference_type aDifference)
			{
				if (aDifference < 0)
					return operator-=(-aDifference);
				*this = const_iterator{*iContainer, container_position() + aDifference};
				return *this;
			}
			const_iterator& operator-=(difference_type aDifference)
			{
				if (aDifference < 0)
					return operator+=(-aDifference);
				*this = const_iterator{*iContainer, container_position() - aDifference};
				return *this;
			}
			const_iterator operator+(difference_type aDifference) const { const_iterator result{*this}; result += aDifference; return result; }
			const_iterator operator-(difference_type aDifference) const { const_iterator result{*this}; result -= aDifference; return result; }
			const_reference operator[](difference_type aDifference) const { return *((*this) + aDifference); }
			friend difference_type operator-(const const_iterator& aLhs, const const_iterator& aRhs) { return static_cast<difference_type>(aLhs.container_position())-static_cast<difference_type>(aRhs.container_position()); }
			const_reference operator*() const { return iNode->value(); }
			const_pointer operator->() const { return &operator*(); }
			bool operator==(const const_iterator& aOther) const { return container_position() == aOther.container_position(); }
			bool operator!=(const const_iterator& aOther) const { return container_position() != aOther.container_position(); }
			bool operator<(const const_iterator& aOther) const { return container_position() < aOther.container_position(); }
			bool operator<=(const const_iterator& aOther) const { return container_position() <= aOther.container_position(); }
			bool operator>(const const_iterator& aOther) const { return container_position() > aOther.container_position(); }
			bool operator>=(const const_iterator& aOther) const { return container_position() >= aOther.container_position(); }

		private:
			size_type container_position() const { return iContainerPosition; }

		private:
			const indexitor* iContainer;
			node* iNode;
			size_type iContainerPosition;
		};
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	public:
		indexitor(const Alloc& aAllocator = Alloc()) :
			iAllocator(aAllocator), iSize(0)
		{
		}
		indexitor(const size_type aCount, const value_type& aValue, const Alloc& aAllocator = Alloc()) :
			iAllocator(aAllocator), iSize(0)
		{
			insert(begin(), aCount, aValue);
		}
		template <typename InputIterator>
		indexitor(InputIterator aFirst, InputIterator aLast, const Alloc& aAllocator = Alloc()) :
			iAllocator(aAllocator), iSize(0)
		{
			insert(begin(), aFirst, aLast);
		}
		indexitor(const indexitor& aOther, const Alloc& aAllocator = Alloc()) :
			iAllocator(aAllocator), iSize(0)
		{
			insert(begin(), aOther.begin(), aOther.end());
		}
		~indexitor()
		{
			erase(begin(), end());
		}
		indexitor& operator=(const indexitor& aOther)
		{
			indexitor newContents{aOther};
			newContents.swap(*this);
			return *this;
		}

	public:
		size_type size() const
		{
			return iSize;
		}
		bool empty() const
		{
			return iSize == 0;
		}
		const_iterator begin() const
		{
			return const_iterator{*this, static_cast<node*>(base::front_node()), 0};
		}
		const_iterator end() const
		{
			return const_iterator{*this, nullptr, size()};
		}
		iterator begin()
		{
			return iterator{*this, static_cast<node*>(base::front_node()), 0};
		}
		iterator end()
		{
			return iterator{*this, nullptr, size()};
		}
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator{end()};
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator{begin()};
		}
		reverse_iterator rbegin()
		{
			return reverse_iterator{end()};
		}
		reverse_iterator rend()
		{
			return reverse_iterator{begin()};
		}
		const_reference front() const
		{
			return *begin();
		}
		reference front()
		{
			return *begin();
		}
		const_reference back() const
		{
			return *--end();
		}
		reference back()
		{
			return *--end();
		}
		size_type index(const_iterator aPosition) const
		{
			if (aPosition.iNode != nullptr)
			{
				if (aPosition.iNode->parent() != nullptr)
					return do_index(aPosition.iNode);
				else
					return aPosition.iNode->left_size();
			}
			else
				return size();
		}
		size_type index(iterator aPosition) const
		{
			if (aPosition.iNode != nullptr)
			{
				if (aPosition.iNode->parent() != nullptr)
					return do_index(aPosition.iNode);
				else
					return aPosition.iNode->left_size();
			}
			else
				return size();
		}
		iterator insert(const_iterator aPosition, const value_type& aValue, const skip_type& aSkip = skip_type{})
		{
			return insert(aPosition, 1, aValue, aSkip);
		}
		const_reference operator[](size_type aIndex) const
		{
			return *(begin() + aIndex);
		}
		reference operator[](size_type aIndex)
		{
			return *(begin() + aIndex);
		}
		template <class InputIterator>
		typename std::enable_if<!std::is_integral<InputIterator>::value, iterator>::type
			insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast)
		{
			return do_insert(aPosition, aFirst, aLast);
		}
		template <class InputIterator, class SkipIterator>
		typename std::enable_if<!std::is_integral<InputIterator>::value, iterator>::type
		insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast, SkipIterator aSkipFirst, SkipIterator aSkipSecond)
		{
			return do_insert(aPosition, aFirst, aLast, aSkipFirst, aSkipSecond);
		}
		iterator insert(const_iterator aPosition, size_type aCount, const value_type& aValue, const skip_type& aSkip = skip_type{})
		{
			if (aCount == 0)
				return iterator{*this, aPosition.iNode};
			auto pos = aPosition.container_position();
			while (aCount > 0)
			{
				aPosition = insert(aPosition, &aValue, &aValue+1, &aSkip, &aSkip+1);
				++aPosition;
				--aCount;
			}
			return iterator{*this, pos};
		}
		void clear()
		{
			erase(begin(), end());
		}
		void push_front(const value_type& aValue, const skip_type& aSkip = skip_type{})
		{
			insert(begin(), aValue, aSkip);
		}
		void push_back(const value_type& aValue, const skip_type& aSkip = skip_type{})
		{
			insert(end(), aValue, aSkip);
		}
		void resize(std::size_t aNewSize, const value_type& aValue = value_type(), const skip_type& aSkip = skip_type{})
		{
			if (size() < aNewSize)
				insert(end(), aNewSize - size(), aValue, aSkip);
			else
				erase(begin() + aNewSize, end());
		}
		iterator erase(const_iterator aPosition)
		{
			auto pos = aPosition.container_position();
			erase(aPosition, aPosition + 1);
			return iterator{*this, pos};
		}
		iterator erase(const_iterator aFirst, const_iterator aLast)
		{
			if (aFirst == aLast)
				return iterator{*this, aFirst.container_position()};
			auto pos = aFirst.container_position();
			for (node* n = aFirst.iNode; n != aLast.iNode;)
			{
				node* next = static_cast<node*>(n->next());
				free_node(n);
				--iSize;
				n = next;
			}
			return iterator{*this, pos};
		}
		void pop_front()
		{
			erase(begin());
		}
		void pop_back()
		{
			erase(--end());
		}
		void swap(indexitor& aOther)
		{
			base::swap(aOther);
			std::swap(iAllocator, aOther.iAllocator);
			std::swap(iSize, aOther.iSize);
		}

	public:
		void update_foreign_index(const_iterator aPosition, const foreign_index_type& aForeignIndex, const skip_type& aSkip = skip_type{})
		{
			value_type currentValue = *aPosition;
			insert(erase(aPosition), value_type{ currentValue.first, aForeignIndex }, aSkip);
		}
		template <typename Pred = std::less<foreign_index_type>>
		std::pair<const_iterator, foreign_index_type> find_by_foreign_index(foreign_index_type aForeignIndex, Pred aPred = Pred{}) const
		{
			size_type nodeIndex{};
			foreign_index_type nodeForeignIndex{};
			auto n = base::find_node_by_foreign_index(aForeignIndex, nodeIndex, nodeForeignIndex, aPred);
			if (!n->is_nil() &&
				!aPred(aForeignIndex - nodeForeignIndex, static_cast<node*>(n)->skip().first) &&
				aPred(aForeignIndex - nodeForeignIndex, static_cast<node*>(n)->foreign_index() - static_cast<node*>(n)->skip().second))
				return std::make_pair(const_iterator{*this, static_cast<node*>(n)}, nodeForeignIndex + static_cast<node*>(n)->skip().first);
			else
				return std::make_pair(end(), foreign_index(end()));
		}
		template <typename Pred = std::less<foreign_index_type>>
		std::pair<iterator, foreign_index_type> find_by_foreign_index(foreign_index_type aForeignIndex, Pred aPred = Pred{})
		{
			size_type nodeIndex{};
			foreign_index_type nodeForeignIndex{};
			auto n = base::find_node_by_foreign_index(aForeignIndex, nodeIndex, nodeForeignIndex, aPred);
			if (!n->is_nil() && 
				!aPred(aForeignIndex - nodeForeignIndex, static_cast<node*>(n)->skip().first) &&
				aPred(aForeignIndex - nodeForeignIndex, static_cast<node*>(n)->foreign_index() - static_cast<node*>(n)->skip().second))
				return std::make_pair(iterator{*this, static_cast<node*>(n)}, nodeForeignIndex + static_cast<node*>(n)->skip().first);
			else
				return std::make_pair(end(), foreign_index(end()));
		}
		foreign_index_type foreign_index(const_iterator aPosition) const
		{
			if (aPosition.iNode != nullptr)
				return do_foreign_index(aPosition.iNode) + aPosition.iNode->skip().first;
			else
				return empty() ? foreign_index_type{} : do_foreign_index(static_cast<const node*>(base::back_node())) + base::back_node()->foreign_index();
		}
		foreign_index_type skip_before(const_iterator aPosition) const
		{
			if (aPosition.iNode != nullptr)
				return aPosition.iNode->skip().first;
			else
				return foreign_index_type{};
		}
		foreign_index_type skip_after(const_iterator aPosition) const
		{
			if (aPosition.iNode != nullptr)
				return aPosition.iNode->skip().second;
			else
				return foreign_index_type{};
		}

	private:
		template <class InputIterator>
		iterator do_insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast)
		{
			node* before = aPosition.iNode;
			size_type pos = aPosition.container_position();
			auto nextPos = pos;
			while (aFirst != aLast)
			{
				base::insert_node(allocate_node(before, *aFirst++), nextPos++);
				++iSize;
			}
			return iterator{*this, pos};
		}
		template <class InputIterator, class SkipIterator>
		iterator do_insert(const_iterator aPosition, InputIterator aFirst, InputIterator aLast, SkipIterator aSkipFirst, SkipIterator aSkipLast)
		{
			node* before = aPosition.iNode;
			size_type pos = aPosition.container_position();
			auto nextPos = pos;
			while (aFirst != aLast && aSkipFirst != aSkipLast)
			{
				base::insert_node(allocate_node(before, *aFirst++, *aSkipFirst++), nextPos++);
				++iSize;
			}
			return iterator{ *this, pos };
		}
		size_type do_index(const node* aNode) const
		{
			if (aNode != base::root_node())
			{
				if (aNode == aNode->parent()->left())
					return do_index(static_cast<const node*>(aNode->parent())) - aNode->size() + aNode->left_size();
				else
					return do_index(static_cast<const node*>(aNode->parent())) + aNode->parent()->centre_size() + aNode->left_size();
			}
			else
				return base::root_node()->left_size();
		}
		foreign_index_type do_foreign_index(const node* aNode) const
		{
			if (aNode != base::root_node())
			{
				if (aNode == aNode->parent()->left())
					return do_foreign_index(static_cast<const node*>(aNode->parent())) - aNode->foreign_index() + aNode->left_foreign_index();
				else
					return do_foreign_index(static_cast<const node*>(aNode->parent())) + aNode->parent()->centre_foreign_index() + aNode->left_foreign_index();
			}
			else
				return base::root_node()->left_foreign_index();
		}
		node* find_node(size_type aContainerPosition) const
		{
			return static_cast<node*>(base::find_node(aContainerPosition));
		}
		node* allocate_node(node* aBefore, const value_type& aValue, const skip_type& aSkip = skip_type{})
		{
			node* newNode = std::allocator_traits<node_allocator_type>::allocate(iAllocator, 1);
			try
			{
				std::allocator_traits<node_allocator_type>::construct(iAllocator, newNode, node(aValue, aSkip));
			}
			catch (...)
			{
				std::allocator_traits<node_allocator_type>::deallocate(iAllocator, newNode, 1);
				throw;
			}
			if (empty())
			{
				base::set_front_node(newNode);
				base::set_back_node(newNode);
			}
			else
			{
				newNode->set_next(aBefore);
				if (aBefore)
				{
					if (aBefore->previous())
					{
						newNode->set_previous(aBefore->previous());
						aBefore->previous()->set_next(newNode);
					}
					aBefore->set_previous(newNode);
					if (base::front_node() == aBefore)
						base::set_front_node(newNode);
				}
				else
				{
					base::back_node()->set_next(newNode);
					newNode->set_previous(base::back_node());
					base::set_back_node(newNode);
				}
			}
			newNode->set_size(1);
			newNode->set_foreign_index(aSkip.first + aValue.second + aSkip.second);
			return newNode;
		}
		void free_node(node* aNode)
		{
			if (aNode)
			{
				if (aNode->next())
					aNode->next()->set_previous(aNode->previous());
				if (aNode->previous())
					aNode->previous()->set_next(aNode->next());
				if (base::back_node() == aNode)
					base::set_back_node(aNode->previous());
				if (base::front_node() == aNode)
					base::set_front_node(aNode->next());
				base::delete_node(aNode);
			}
			std::allocator_traits<node_allocator_type>::destroy(iAllocator, aNode);
			std::allocator_traits<node_allocator_type>::deallocate(iAllocator, aNode, 1);
		}

	private:
		node_allocator_type iAllocator;
		size_type iSize;
	};
}
