#include <blusher/view.h>

#include "view-impl.h"

namespace bl {

View::View(View *parent)
{
    if (parent != nullptr) {
        this->_impl = new ViewImpl(parent->_impl);
    } else {
        this->_impl = new ViewImpl();
    }

    this->_parent = parent;
}

double View::x() const
{
    return this->_impl->x();
}

double View::y() const
{
    return this->_impl->y();
}

double View::width() const
{
    return this->_impl->width();
}

double View::height() const
{
    return this->_impl->height();
}

void View::set_x(double x)
{
    this->_impl->setX(x);
}

void View::set_y(double y)
{
    this->_impl->setY(y);
}

void View::set_width(double width)
{
    this->_impl->setWidth(width);
}

void View::set_height(double height)
{
    this->_impl->setHeight(height);
}

} // namespace bl
