#include "vector.hpp"

#include <vector>
#include <iostream>

struct Point
{
    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {
        // std::cout << "Point ctor!\n";
    }

    Point(Point&& other) noexcept
    {
        std::cout << "Move ctor!\n";
    }

    Point(const Point& other)
    {
        std::cout << "Copy ctor!\n";
    }

    int x, y;
};

int main()
{
    stl_container_impl::Vector<Point> v;
    v.emplace_back();
    v.emplace_back();
    v.emplace_back();
    v.emplace_back();

    std::cout << "\n\n";

    std::vector<Point> v1;
    v1.emplace_back();
    v1.emplace_back();
    v1.emplace_back();
    v1.emplace_back();

	return 0;
}
