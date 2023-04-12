#include <blusher/rect.h>

#include <math.h>

namespace bl {

Rect::Rect()
    : _pos(0.0, 0.0),
    _size(0.0, 0.0)
{
}

Rect::Rect(double x, double y, double width, double height)
    : _pos(x, y),
      _size(width, height)
{
}

Rect::Rect(const Point& pos, const Size& size)
    : _pos(pos),
      _size(size)
{
}

double Rect::x() const
{
    return this->_pos.x();
}

double Rect::y() const
{
    return this->_pos.y();
}

double Rect::width() const
{
    return this->_size.width();
}

double Rect::height() const
{
    return this->_size.height();
}

Size Rect::size() const
{
    return this->_size;
}

bool Rect::contains(const Point& point)
{
    // Check top left.
    if (point.x() < this->x() || point.y() < this->y()) {
        return false;
    }

    // Check bottom right.
    if (point.x() > this->x() + this->width() ||
            point.y() > this->y() + this->height()) {
        return false;
    }

    // Check width and height.
    if (point.x() <= this->x() + this->width() &&
            point.y() <= this->y() + this->height()) {
        return true;
    }

    return false;
}

Rect Rect::intersection(const Rect& other) const
{
    double x1 = this->x();
    double y1 = this->y();
    double x2 = this->width() + x1;
    double y2 = this->height() + y1;

    double x3 = other.x();
    double y3 = other.y();
    double x4 = other.width() + x3;
    double y4 = other.height() + y3;

    if (x1 >= x4 || x2 <= x3 || y1 >= y4 || y2 <= y3) {
        return Rect();
    }

    double x = fmax(x1, x3);
    double y = fmax(y1, y3);

    // Completely contains.
    if ((x1 < x3 && y1 < y3 && x2 > x4 && y2 > y4) ||
            (x3 < x1 && y1 > y3 && x2 < x4 && y2 < y4)) {
        double width = fmin(this->width(), other.width());
        double height = fmin(this->height(), other.height());

        return Rect(x, y, width, height);
    }

    double width = 0; // distance_x - min_x;
    // One width is in other.
    if ((x3 <= x1 && x4 >= x2) || (x3 >= x1 && x4 <= x2)) {
        width = fmin(this->width(), other.width());
    } else {
        if (x1 < x3) {
            // This rect is left one.
            width = x2 - x;
        } else {
            // Other rect is left one.
            width = x4 - x;
        }
    }

    double height = 0; // distance_y - min_y;
    // One height is in other.
    if ((y3 <= y1 && y4 >= y2) || (y3 >= y1 && y4 <= y2)) {
        height = fmin(this->height(), other.height());
    } else {
        if (y1 < y3) {
            // This rect is higher one.
            height = y2 - y;
        } else {
            // Other rect is higher one.
            height = y4 - y;
        }
    }

    return Rect(x, y, width, height);
}

bool Rect::operator==(const Rect& other) const
{
    return this->x() == other.x() &&
       this->y() == other.y() &&
       this->width() == other.width() &&
       this->height() == other.height();
}

} // namespace bl
