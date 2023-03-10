#pragma once

// TODO: Exception safety
// TODO: Implement iterators

#include <cstddef>
#include <memory>
#include <utility>

#include "stl_iterator.h"

namespace std_lib_impl
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

        using iterator = __gnu_cxx::__normal_iterator<pointer, Vector>; // This is only wrapper of pointer
        using const_iterator = __gnu_cxx::__normal_iterator<const_pointer, Vector>; // This is only wrapper of pointer
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    public:
        Vector() = default;

        ~Vector()
        {
            std::_Destroy(m_buffer, m_finish, m_allocator);
            Allocator_traits::deallocate(m_allocator, m_buffer, capacity());
        }

        void resize(size_type newSize, const value_type& value)
        {
            if (newSize > size())
            {
                std::_Destroy(m_buffer, m_finish, m_allocator);
                Allocator_traits::deallocate(m_allocator, m_buffer, capacity());

                const auto newCap = newSize;
                m_buffer = Allocator_traits::allocate(m_allocator, newCap);
                m_finish = m_buffer + newCap;
                m_endOfStorage = m_finish;

                for (auto ptr = m_buffer; ptr != m_finish; ++ptr)
                {
                    Allocator_traits::construct(m_allocator, ptr, value);
                }
            }
            else
            {
                auto newFinish = m_buffer + newSize - size();
                std::_Destroy(newFinish, m_finish, m_allocator);

                m_finish = newFinish;
            }
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

        void pop_back()
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
            auto newBuff = Allocator_traits::allocate(m_allocator, newCap);
            const auto sz = size();
            for (size_t i = 0; i < sz; ++i)
            {
                new (newBuff + i) T(std::move_if_noexcept(m_buffer[i]));
            }
            Allocator_traits::deallocate(m_allocator, m_buffer, capacity());
            m_buffer = newBuff;
            m_endOfStorage = m_buffer + newCap;
            m_finish = m_buffer + sz;

            Allocator_traits::construct(m_allocator, m_finish, std::forward<Args>(args)...);
            ++m_finish;
        }

    private:
        pointer m_buffer = nullptr;
        pointer m_finish = nullptr;
        pointer m_endOfStorage = nullptr;

        Allocator m_allocator;
    };

} // namespace std_lib_impl
