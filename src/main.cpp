#include "vector.hpp"

#include <vector>
#include <iostream>

struct Point
{
    Point(int _x = 0, int _y = 0)
        : x(_x)
        , y(_y)
    {
        std::cout << "Point ctor!\n";

        static int idx = 1;
        id = idx++;
    }

    ~Point()
    {
        std::cout << "Destructor\n";
    }

    Point(Point&& other)
    {
        std::cout << "Move ctor!\n";
    }

    Point(const Point& other)
    {
        std::cout << "Copy ctor!\n";
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
        //v.emplace_back();
        //v.emplace_back();

        v.resize(10);
    }
    catch(...)
    { 
    }

    std::cout << "\n";

    try
    {
        //v1.emplace_back();
        //v1.emplace_back();

        v1.resize(10);
    }
    catch(...)
    {
    }

    std::cout << v.size() << std::endl;
    std::cout << v1.size() << std::endl;
    std::cout << "\n\n";

    system("pause");
	return 0;
}
