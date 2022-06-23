#include "view-impl.h"

// C
#include <stdio.h>

#include <blusher/view.h>

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

    this->m_composedImage = new Image(this->m_width, this->m_height);
    this->m_composedImage->fill(this->m_color);

    QObject::connect(this, &ViewImpl::colorChanged,
                     this, [this]() {
        this->update();
    });
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

        this->m_image->resize(width, this->m_height);
        this->m_image->fill(this->m_color);
    }
}

void ViewImpl::setHeight(double height)
{
    if (this->m_height != height) {
        this->m_height = height;

        this->m_image->resize(this->m_width, height);
        this->m_image->fill(this->m_color);
    }
}

Color ViewImpl::color() const
{
    return this->m_color;
}

void ViewImpl::setColor(const Color& color)
{
    this->m_color = color;

    this->m_image->fill(color);

    emit this->colorChanged();
}

void ViewImpl::appendChild(View *view)
{
    (void)view;
    this->update();
}

//=================
// Private Slots
//=================

void ViewImpl::update()
{
    if (this->m_composedImage->width() != this->m_image->width() &&
            this->m_composedImage->height() != this->m_image->height()) {
        this->m_composedImage->resize(
            this->m_image->width(),
            this->m_image->height()
        );
    }
    this->m_composedImage->fill(this->m_color);
    for (auto& child: this->m_view->_children) {
        this->m_composedImage->add(*(child->_impl->m_image),
            child->x(), child->y());
    }
}

} // namespace bl
