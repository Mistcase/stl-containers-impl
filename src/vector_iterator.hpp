#pragma once

// This is styled __normal_iterator from gcc standard lib.

#include <iterator>

namespace stl_container_impl
{
    template<typename Iterator, typename Container>
    class pointer_wrapper_iterator
    {
    protected:
        Iterator m_ptr;

        using traits_type = std::iterator_traits<Iterator>;

        template<typename Iter>
        using convertible_from = std::enable_if_t<std::is_convertible<Iter, Iterator>::value>;

    public:
        using iterator_type = Iterator;
        using iterator_category = typename traits_type::iterator_category;
        using value_type = typename traits_type::value_type;
        using difference_type = typename traits_type::difference_type;
        using reference = typename traits_type::reference;
        using pointer = typename traits_type::pointer;

        pointer_wrapper_iterator() noexcept
            : m_ptr(Iterator()) { }

        explicit pointer_wrapper_iterator(const Iterator& i) noexcept
            : m_ptr(i) { }

        template<typename Iter, typename = convertible_from<Iter>>
        pointer_wrapper_iterator(const pointer_wrapper_iterator<Iter, Container>& i) noexcept
            : m_ptr(i.base()) { }

        // Forward iterator requirements
        reference operator*() const noexcept { return *m_ptr; }
        pointer operator->() const noexcept { return m_ptr; }

        pointer_wrapper_iterator& operator++()noexcept
        {
            ++m_ptr;
            return *this;
        }

        pointer_wrapper_iterator operator++(int)noexcept { return pointer_wrapper_iterator(m_ptr++); }

		bool operator!= (const pointer_wrapper_iterator& other) const noexcept { return m_ptr != other.m_ptr; }
		bool operator== (const pointer_wrapper_iterator& other) const noexcept { return m_ptr == other.m_ptr; }

        // Bidirectional iterator requirements
        pointer_wrapper_iterator& operator--() noexcept
        {
            --m_ptr;
            return *this;
        }

        pointer_wrapper_iterator operator--(int) noexcept { return pointer_wrapper_iterator(m_ptr--); }

        // Random access iterator requirements
        reference operator[](difference_type n) const noexcept { return m_ptr[n]; }
        pointer_wrapper_iterator& operator+=(difference_type n) noexcept { m_ptr += n; return *this; }
        pointer_wrapper_iterator operator+(difference_type n) const noexcept { return pointer_wrapper_iterator(m_ptr + n); }
        pointer_wrapper_iterator& operator-=(difference_type n) noexcept { m_ptr -= n; return *this; }
        pointer_wrapper_iterator operator-(difference_type n) const noexcept { return pointer_wrapper_iterator(m_ptr - n); }
        const Iterator& base() const noexcept { return m_ptr; }
    };

} // namespace stl_container_impl
