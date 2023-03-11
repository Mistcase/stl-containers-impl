#include "vector.hpp"

#include <vector>
#include <iostream>

struct Point
{
    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {
        std::cout << "Point ctor: " << this << std::endl;;

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
        std::cout << "Move ctor: " << this << std::endl;;
    }

    Point(const Point& other)
    {
        x = other.x;
        y = other.y;
        std::cout << "Copy ctor: " << this << std::endl;;
    }

    Point& operator= (const Point other)
    {
        return *this;
    }

    int x, y;
    int id;
};

int main()
{
    stl_container_impl::Vector<Point> v;
    std::vector<Point> v1;

    try
    {
        v.emplace_back();
        v.reserve(5);

        v.clear();
    }
    catch(...)
    {
    }

    std::cout << "\n";

    try
    {
        v1.emplace_back();
        v1.reserve(5);

        v1.clear();
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
