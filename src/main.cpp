#include "Vector.hpp"

#include <vector>
#include <iostream>

struct Point
{
    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {
        std::cout << "Point ctor!\n";
    }

    Point(Point&& other) noexcept
    {
        std::cout << "Move ctor!\n";
    }

    int x, y;
};

int main()
{

	return 0;
}
