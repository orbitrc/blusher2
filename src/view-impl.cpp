#include "view-impl.h"

namespace bl {

ViewImpl::ViewImpl(QObject *parent)
    : QObject(parent)
{
    this->m_x = 0.0;
    this->m_y = 0.0;
    this->m_width = 10.0;
    this->m_height = 10.0;

    this->m_color = Color::from_rgba(255, 255, 255, 255);

    this->m_image = new Image(this->m_width, this->m_height);
    this->m_image->fill(this->m_color);
}

ViewImpl::~ViewImpl()
{
    // delete this->m_image;
}

double ViewImpl::x() const
{
    return this->m_x;
}

double ViewImpl::y() const
{
    return this->m_y;
}

double ViewImpl::width() const
{
    return this->m_width;
}

double ViewImpl::height() const
{
    return this->m_height;
}

void ViewImpl::setX(double x)
{
    if (this->m_x != x) {
        this->m_x = x;
    }
}

void ViewImpl::setY(double y)
{
    if (this->m_y != y) {
        this->m_y = y;
    }
}

void ViewImpl::setWidth(double width)
{
    if (this->m_width != width) {
        this->m_width = width;
    }
}

void ViewImpl::setHeight(double height)
{
    if (this->m_height != height) {
        this->m_height = height;
    }
}

} // namespace bl
