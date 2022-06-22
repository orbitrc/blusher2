#ifndef _BL_RECT_H
#define _BL_RECT_H

#include <blusher/point.h>
#include <blusher/size.h>

namespace bl {

class Rect
{
public:
    Rect(double x, double y, double width, double height);
    Rect(const Point& pos, const Size& size);

    double x() const;
    double y() const;
    double width() const;
    double height() const;

    Size size() const;

    /// Returns true if this rect contains given point.
    bool contains(const Point& point);

private:
    Point _pos;
    Size _size;
};

} // namespace bl

#endif // _BL_RECT_H
