#include "vector.hpp"

#include <vector>
#include <iostream>

struct Point
{
    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {
        std::cout << "Point ctor: " << this << std::endl;

        static int idx = 1;
        id = idx++;
    }

    ~Point()
    {
        std::cout << "Destructor\n";
    }

    Point(Point&& other)
    {
        x = other.x;
        y = other.y;
        id = other.id;
        std::cout << "Move ctor: " << this << std::endl;
    }

    Point(const Point& other)
    {
        x = other.x;
        y = other.y;
        id = other.id;
        std::cout << "Copy ctor: " << this << std::endl;
    }

    Point& operator= (const Point& other)
    {
        x = other.x;
        y = other.y;
        id = other.id;

        std::cout << "Copy operator=: " << this << std::endl;

        static size_t i = 0;
        if (i > 0)
        {
            // throw 4;
        }
        i++;

        return *this;
    }

    Point& operator= (Point&& other) noexcept
    {
        x = other.x;
        y = other.y;
        id = other.id;

        std::cout << "Move operator=: " << this << std::endl;

        return *this;
    }

    int x, y;
    int id;
};

int main()
{
    stl_container_impl::Vector<Point> v;
    std::vector<Point> v1;

    stl_container_impl::Vector<Point> _v;
    _v.emplace_back();
    _v.emplace_back();
    _v.emplace_back();
    _v.emplace_back();
    _v.emplace_back();
    _v.emplace_back();

    Point toInsert{};
    toInsert.id = -1;

    std::cout << "Start testing...\n\n";

    // Copy aasign tests:
    // 1. NewSize < oldCapacity && NewSize > oldSize    +
    // 2. NewSize < oldCapacity && NewSize < oldSize    +
    // 3. NewSize > oldCapacity                         +

    try
    {
        v.emplace_back();
        // Insert here
        v.emplace_back();
        v.emplace_back();
        v.emplace_back();
        v.reserve(8);

        // std::cout << "erase...\n";
        // v.erase(v.begin() + 1);

       /* std::cout << "Copy assign\n";
        v = _v;*/

        // std::cout << "Move assign\n";
        // v = std::move(_v);

        std::cout << "Before insertion:\n";
        for (const auto& n : v)
            std::cout << n.id << std::endl;

        std::cout << "Insertion...\n";
        v.insert(v.begin() + 2, 2, toInsert);

        std::cout << "After insertion:\n";
        for (const auto& n : v)
            std::cout << n.id << std::endl;

        // v.clear();
    }
    catch(...)
    {
    }

    std::cout << "\n";

    std::vector<Point> v2(6);

    try
    {
        v1.emplace_back();
        // Insert here
        v1.emplace_back();
        v1.emplace_back();
        v1.emplace_back();
        v1.reserve(8);

        // std::cout << "erase...\n";
        // v1.erase(v1.begin() + 1);

       /* std::cout << "Copy assign\n";
        v1 = v2;*/

        // std::cout << "Move assign\n";
        // v1 = std::move(v2);

        std::cout << "Before insertion:\n";
        for (const auto& n : v1)
            std::cout << n.id << std::endl;

        std::cout << "Insertion...\n";
        v1.insert(v1.begin() + 1, std::move(toInsert));

        std::cout << "After insertion:\n";
        for (const auto& n : v1)
            std::cout << n.id << std::endl;

        // v1.clear();
    }
    catch(...)
    {
    }

    std::cout << "\n\n";

    std::cout << "std::vector::size = " << v1.size() << std::endl;
    std::cout << "std::vector::capacity = " << v1.capacity() << std::endl;
    std::cout << "stl_lib_impl::vector::size = " << v.size() << std::endl;
    std::cout << "stl_lib_impl::vector::capacity = " << v.capacity() << std::endl;

    std::cout << "\n\n";

	return 0;
}
