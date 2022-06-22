// C
#include <stdio.h>

// Blusher
#include <blusher/view.h>
#include <blusher/surface.h>

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

    // Add to children.
    if (parent != nullptr) {
        parent->_children.push(this);

        parent->_impl->appendChild(this);
    }

    // View reference for ViewImpl.
    this->_impl->m_view = this;

    // Initialize.
    if (parent != nullptr) {
        this->_surface = parent->_surface;
    } else {
        this->_surface = nullptr;
    }
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

Color View::color() const
{
    return this->_impl->color();
}

void View::set_color(const Color &color)
{
    this->_impl->setColor(color);
}

void View::set_surface(Surface *surface)
{
    this->_surface = surface;
}

void View::update()
{
    this->_impl->update();
    if (this->_surface == nullptr) {
        return;
    }
    this->_surface->update();
}

} // namespace bl