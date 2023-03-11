#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>

#include "vector_iterator.hpp"

namespace stl_container_impl
{
    template <class T, class Allocator = std::allocator<T>>
    class Vector
    {
        using Allocator_traits = std::allocator_traits<Allocator>;

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = typename Allocator_traits::size_type;
        using difference_type = typename Allocator_traits::difference_type;
        using pointer = typename Allocator_traits::pointer;
        using const_pointer = typename Allocator_traits::const_pointer;
        using reference = value_type&;
        using const_reference = const value_type&;

        using iterator = stl_container_impl::pointer_wrapper_iterator<pointer, Vector>;
        using const_iterator = stl_container_impl::pointer_wrapper_iterator<const_pointer, Vector>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    public:
        Vector() = default;

        Vector(const Vector& other)
        {
            m_allocator = Allocator_traits::select_on_container_copy_construction(other.get_allocator());

            const auto newCapacity = other.capacity();
            auto buff = Allocator_traits::allocate(m_allocator, newCapacity);

            try
            {
                construct_range(other.m_buffer, other.m_finish, buff);
            }
            catch (...)
            {
                // Destroy constructed?

                Allocator_traits::deallocate(m_allocator, buff, newCapacity);
                throw;
            }

            m_buffer = buff;
            m_finish = m_buffer + other.size();
            m_endOfStorage = m_buffer + newCapacity;
        }

        Vector(Vector&& other) noexcept
        {
            /*---------------------------------------------------------------------------------------------
            * Constructs the container with the contents of other using move semantics.
            * Allocator is obtained by move-construction from the allocator belonging to other.
            * After the move, other is guaranteed to be empty().
            -----------------------------------------------------------------------------------------------*/

            m_allocator = std::move(other.m_allocator);
            m_buffer = other.m_buffer;
            m_finish = other.m_finish;
            m_endOfStorage = other.m_endOfStorage;

            other.m_buffer = nullptr;
            other.m_finish = nullptr;
            other.m_endOfStorage = nullptr;
        }

        ~Vector()
        {
            destroy_range(m_buffer, m_finish);
            Allocator_traits::deallocate(m_allocator, m_buffer, capacity());
        }

        void reserve(size_type count)
        {
            const auto capacity = this->capacity();
            if (count <= capacity)
            {
                return;
            }

            auto buff = Allocator_traits::allocate(m_allocator, count);
            auto finish = buff;
            move_construct_range_if_noexcept(m_buffer, m_finish, finish);
            destroy_range(m_buffer, m_finish);

            m_buffer = buff;
            m_finish = finish;
            m_endOfStorage = m_buffer + count;
        }

        void resize(size_type count)
        {
            _resize(count);
        }

        void resize(size_type count, const value_type& value)
        {
            _resize(count, value);
        }

		void assign(size_type count, const T& value)
		{
			resize(count, value);
		}

		// TODO: Check if InputIt is LegacyInputIterator
		template <typename InputIt>
		void assign(InputIt first, InputIt last)
		{
			clear();
			for (; first != last; ++first)
			{
				emplace_back(*first);
			}
		}

		void assign(std::initializer_list<T> ilist)
		{
			assign(ilist.begin(), ilist.end());
		}

        template <typename... Args>
        void emplace_back(Args&& ...args)
        {
            if (m_finish != m_endOfStorage)
            {
                Allocator_traits::construct(m_allocator, m_finish, std::forward<Args>(args)...);
                m_finish++;
            }
            else
            {
                realloc_insert(std::forward<Args>(args)...);
            }
        }

        void push_back(const T& value)
        {
            if (m_finish != m_endOfStorage)
            {
                Allocator_traits::construct(m_allocator, m_finish, value);
                ++m_finish;
            }
            else
            {
                realloc_insert(value);
            }
        }

        void push_back(value_type&& value)
        {
            emplace_back(std::move(value));
        }

        void pop_back() noexcept
        {
            --m_finish;
            Allocator_traits::destroy(m_allocator, m_finish);
        }

        iterator erase(iterator pos) noexcept
        {
            auto dest = pos.base();
            auto src = dest + 1;

            for (; src != m_finish; ++src, ++dest)
            {
                *dest = std::move(*src);
            }

            pop_back();
            return iterator{ dest };
        }

        void clear() noexcept
        {
            destroy_range(m_buffer, m_finish);
            m_finish = m_buffer;
        }

		void shrink_to_fit() noexcept
		{
			const auto size = this->size();
			const auto capacity = this->capacity();

			if (size == capacity)
			{
				return;
			}

			pointer buffer;
			pointer finish;

			try
			{
				finish = buffer = Allocator_traits::allocate(m_allocator, size);
				move_construct_range_if_noexcept(m_buffer, m_finish, finish);

				m_buffer = buffer;
				m_finish = finish;
				m_endOfStorage = m_finish;
			}
			catch (std::bad_alloc)
			{
			}
			catch (...)
			{
				destroy_range(buffer, finish);
				Allocator_traits::deallocate(m_allocator, buffer, size);
			}
		}

    public:
        Vector& operator= (const Vector& other)
        {
            if (std::addressof(other) == this) // operator& can be overloaded
            {
                return *this;
            }

            if constexpr (Allocator_traits::propagate_on_container_copy_assignment::value)
            {
                m_allocator = other.m_allocator;
            }

            const auto oldCap = capacity();
            const auto newCapacity = other.capacity();
            const auto newSize = other.size();

            auto otherBuff = other.m_buffer;
            auto otherFinish = other.m_finish;

            if (newSize > oldCap)
            {
                destroy_range(m_buffer, m_finish);
                Allocator_traits::deallocate(m_allocator, m_buffer, oldCap);

                auto newBuff = Allocator_traits::allocate(m_allocator, newCapacity); // TODO: Can it be allocated "other.capacity()" space by standard
                construct_range(otherBuff, otherFinish, m_buffer);

                m_finish = m_buffer + newSize;
                m_endOfStorage = m_buffer + newCapacity;

                return *this;
            }

            const auto oldSize = size();
            auto newFinish = m_buffer + newSize;

            if (newSize > oldSize)
            {
                copy_assign_range(otherBuff, otherBuff + oldSize, m_buffer);
                construct_range(otherBuff + oldSize, otherBuff + newSize, m_finish);
            }
            else
            {
                copy_assign_range(otherBuff, otherBuff + newSize, m_buffer);
                destroy_range(newFinish, m_finish);
            }

            m_finish = newFinish;

            return *this;
        }

        Vector& operator= (Vector&& other) noexcept
        {
            if (std::addressof(other) == this) // operator& can be overloaded
            {
                return *this;
            }

            /*---------------------------------------------------------------------------------------------
            * https://en.cppreference.com/w/cpp/container/vector/operator%3D
            *
            * If std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value is true,
            * the allocator of *this is replaced by a copy of that of other.
            * If it is false and the allocators of *this and other do not compare equal,
            * *this cannot take ownership of the memory owned by otherand must move-assign each element individually,
            * allocating additional memory using its own allocator as needed.
            * In any case, all elements originally belonging to* this are either destroyed or replaced by element-wise move-assignment.
            -----------------------------------------------------------------------------------------------*/

            if constexpr (Allocator_traits::propagate_on_container_move_assignment::value)
            {
                m_allocator = std::move(other.m_allocator);
            }

            if (m_allocator == other.m_allocator)
            {
                destroy_range(m_buffer, m_finish);
                Allocator_traits::deallocate(m_allocator, m_buffer, capacity());

                new (this) Vector(std::move(other));
            }
            else
            {
                const auto oldSize = size();
                const auto oldCapacity = capacity();
                const auto newSize = other.size();
                const auto newCapacity = other.capacity();

                if (newSize > oldCapacity)
                {
                    destroy_range(m_buffer, m_finish);
                    Allocator_traits::deallocate(m_allocator, m_buffer, oldCapacity);

                    m_finish = m_buffer = Allocator_traits::allocate(m_allocator, newCapacity);
                    move_construct_range_if_noexcept(other.m_buffer, other.m_finish, m_finish);
                    m_endOfStorage = m_buffer + newCapacity;

                    return *this;
                }

                auto newFinish = m_buffer + other.size();
                if (newSize > oldSize)
                {
                    move_assign_range_if_noexcept(other.m_buffer, other.m_finish, m_buffer);
                    move_construct_range_if_noexcept(other.m_buffer + oldSize, other.m_buffer + newSize, m_finish);
                }
                else
                {
                    copy_assign_range(other.m_buffer, other.m_finish, m_buffer);
                    destroy_range(newFinish, m_finish);
                }

                m_finish = newFinish;
            }

            return *this;
        }

    public:
        bool empty() const noexcept { return m_buffer == m_finish; }

		size_type max_size() const noexcept { return std::numeric_limits<difference_type>::max(); }
        size_type size() const noexcept { return m_finish - m_buffer; }
        size_type capacity() const noexcept { return m_endOfStorage - m_buffer; }

        iterator       begin() noexcept { return iterator{ m_buffer }; }
        const_iterator cbegin() const noexcept { return const_iterator{ m_buffer }; }
        iterator       end() noexcept { return iterator{ m_finish }; }
        const_iterator cend() const noexcept { return const_iterator{ m_finish }; }

		Allocator get_allocator() const noexcept { return m_allocator; }

		reference front() { return *m_buffer; }
		const_reference front() const { return *m_buffer; }

		reference back() { return *(m_finish - 1); }
		const_reference back() const { return *(m_finish - 1); }

		pointer data() noexcept { return m_buffer; }
		const_pointer data() const noexcept { return m_buffer; };

        reference operator[] (size_type pos) { return m_buffer[pos]; }

		reference at(size_type pos)
		{
			if (pos >= size())
			{
				throw std::out_of_range("Vector::at");
			}

			return m_buffer[pos];
		}

		const_reference at(size_type pos) const
		{
			if (pos >= size())
			{
				throw std::out_of_range("Vector::at");
			}

			return m_buffer[pos];
		}

    private:
        template <typename... Args>
        void realloc_insert(Args&& ...args)
        {
            const auto oldSize = size();
            const auto oldCap = capacity();
            const auto newCapacity = oldCap + std::max(size_type(1), oldCap);
            pointer buff;
            pointer finish;

            try
            {
                finish = buff = Allocator_traits::allocate(m_allocator, newCapacity);
                move_construct_range_if_noexcept(m_buffer, m_finish, finish);
                Allocator_traits::construct(m_allocator, buff + oldSize, std::forward<Args>(args)...);
                ++finish;

                destroy_range(m_buffer, m_finish);
                Allocator_traits::deallocate(m_allocator, m_buffer, capacity());

                m_buffer = buff;
                m_endOfStorage = m_buffer + newCapacity;
                m_finish = finish;
            }
            catch (...)
            {
                Allocator_traits::deallocate(m_allocator, buff, newCapacity);
                throw;
            }
        }

        void move_construct_range_if_noexcept(pointer fromFirst, pointer fromLast, pointer& to)
        {
            // Destroy previously constructed by copy ctor objects (if move is inaccessible)
            auto _to = to;
            try
            {
                auto ptr = fromFirst;
                for (; ptr != fromLast; ++ptr, ++to)
                {
                    Allocator_traits::construct(m_allocator, to, std::move_if_noexcept(*ptr));
                }
            }
            catch (...)
            {
                destroy_range(_to, to);
                throw;
            }
        }

        void move_assign_range_if_noexcept(pointer srcFirst, pointer srcLast, pointer dst) noexcept
        {
            for (; srcFirst != srcLast; ++srcFirst, ++dst)
            {
                *dst = std::move_if_noexcept(*srcFirst);
            }
        }

        template <typename... Args>
        void construct_range(pointer first, pointer last, Args&& ...args)
        {
            auto _first = first;
            try
            {
                for (; first != last; ++first)
                {
                    Allocator_traits::construct(m_allocator, first, std::forward(args)...);
                }
            }
            catch (...)
            {
                destroy_range(_first, first);
                throw;
            }
        }

        void construct_range(pointer srcFirst, pointer srcLast, pointer dst)
        {
            for (; srcFirst != srcLast; ++srcFirst, ++dst)
            {
                Allocator_traits::construct(m_allocator, dst, *srcFirst);
            }
        }

        void copy_assign_range(pointer srcFirst, pointer srcLast, pointer dst)
        {
            for (; srcFirst != srcLast; ++srcFirst, ++dst)
            {
                *dst = *srcFirst;
            }
        }

        void destroy_range(pointer first, pointer last)
        {
            // Objects have to not to throw exceptions in destructor
#if defined(WIN32) || defined(__APPLE__)
            for (auto ptr = first; ptr != last; ++ptr)
            {
                Allocator_traits::destroy(m_allocator, ptr);
            }
#else
            std::_Destroy(first, last, m_allocator);
#endif
        }

        template <typename... Args>
        void _resize(size_type count, const Args& ...constructArgs)
        {
            const auto size = this->size();
            if (count > size)
            {
                pointer newBuff;
                pointer finish;

                try
                {
                    finish = newBuff = Allocator_traits::allocate(m_allocator, count);
                    auto endOfStorage = newBuff + count;

                    move_construct_range_if_noexcept(m_buffer, m_finish, finish);
                    construct_range(finish, endOfStorage, constructArgs...);
                    destroy_range(m_buffer, m_finish);

                    Allocator_traits::deallocate(m_allocator, m_buffer, capacity());

                    m_buffer = newBuff;
                    m_finish = finish;
                    m_endOfStorage = endOfStorage;
                }
                catch (std::bad_alloc)
                {
                    throw;
                }
                catch (...)
                {
                    destroy_range(newBuff, finish);
                    throw;
                }
            }
            else
            {
                auto newFinish = m_buffer + count - size;
                destroy_range(newFinish, m_finish);

                m_finish = newFinish;
            }
        }

    private:
        pointer m_buffer = nullptr;
        pointer m_finish = nullptr;
        pointer m_endOfStorage = nullptr;

        Allocator m_allocator;
    };

} // namespace stl_container_impl
