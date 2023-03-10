#pragma once

#include <cstddef>

namespace std_lib_impl
{
    template <class T, class Allocator>
    class Vector
    {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    private:
        size_type m_size;
        size_type m_capacity;

        void* m_buffer;
    };

} // namespace std_lib_impl
