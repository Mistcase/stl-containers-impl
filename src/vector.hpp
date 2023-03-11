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

        ~Vector()
        {
            destroy_range(m_buffer, m_finish);
            Allocator_traits::deallocate(m_allocator, m_buffer, capacity());
        }

        void resize(size_type count)
        {
            _resize(count);
        }

        void resize(size_type count, const value_type& value)
        {
            _resize(count, value);
        }

        template <typename... Args>
        void emplace_back(Args&& ...args)
        {
            try
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
            catch(...)
            {
                throw;
            }
        }

        void push_back(const T& value)
        {
            try
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
            catch(...)
            {
                throw;
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

        size_type size() const { return m_finish - m_buffer; }
        size_type capacity() const { return m_endOfStorage - m_buffer; }

        iterator       begin() { return iterator{ m_buffer }; }
        const_iterator cbegin() const { return const_iterator{ m_buffer }; }
        iterator       end() { return iterator { m_finish }; }
        const_iterator cend() const { return const_iterator{ m_finish }; }

        reference operator[] (size_type pos) { return m_buffer[pos]; }

    private:
        template <typename... Args>
        void realloc_insert(Args&& ...args)
        {
            const auto oldCap = capacity();
            const auto newCap = oldCap + std::max(size_type(1), oldCap);
            pointer buff;
            pointer finish;

            try
            {
                finish = buff = Allocator_traits::allocate(m_allocator, newCap);
                move_range_if_noexcept(m_buffer, m_finish, finish);
                destroy_range(m_buffer, m_finish);
                Allocator_traits::deallocate(m_allocator, m_buffer, capacity());
                Allocator_traits::construct(m_allocator, finish, std::forward<Args>(args)...);
                ++finish;

                m_buffer = buff;
                m_endOfStorage = m_buffer + newCap;
                m_finish = finish;
            }
            catch(...)
            {
                Allocator_traits::deallocate(m_allocator, buff, newCap);
                throw;
            }
        }

        void move_range_if_noexcept(pointer fromFirst, pointer fromLast, pointer& to)
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
            catch(...)
            {
                destroy_range(_to, to);
                throw;
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

                    move_range_if_noexcept(m_buffer, m_finish, finish);
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

} // namespace std_lib_impl
