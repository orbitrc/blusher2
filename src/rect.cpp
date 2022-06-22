#include <blusher/rect.h>

namespace bl {

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

} // namespace bl
