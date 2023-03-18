#include <blusher/point.h>

namespace bl {

Point::Point(double x, double y)
{
    this->_x = x;
    this->_y = y;
}

double Point::x() const
{
    return this->_x;
}

double Point::y() const
{
    return this->_y;
}

void Point::set_x(double x)
{
    if (this->_x != x) {
        this->_x = x;
    }
}

void Point::set_y(double y)
{
    if (this->_y != y) {
        this->_y = y;
    }
}

Point operator+(const Point& p1, const Point& p2)
{
    return {p1.x() + p2.x(), p1.y() + p2.y()};
}

} // namespace bl
