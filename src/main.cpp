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
        throw std::exception();
    }

    Point& operator= (const Point other)
    {
        return *this;
    }

    int x, y;
    int id;
};

template <typename T>
class MyClass
{
    void foo(int s, const T& v = T())
    {
        T dfsd = v;
    }

    void foo(int s)
    {
        T sdf;
    }
};

int main()
{
    stl_container_impl::Vector<Point> v;
    std::vector<Point> v1;

    try
    {
        v.emplace_back();
        v.emplace_back();
    }
    catch(...)
    {
        std::cout << v.size() << std::endl;
    }

    std::cout << "\n";

    try
    {
        v1.emplace_back();
        v1.emplace_back();
    }
    catch(...)
    {
        std::cout << v1.size() << std::endl;
    }

    std::cout << "\n\n";

	return 0;
}
