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

} // namespace bl
