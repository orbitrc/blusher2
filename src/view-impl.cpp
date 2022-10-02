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

    this->m_image = new Image(this->m_width, this->m_height);
    this->m_image->fill(Color::from_rgba(0, 0, 0, 0));

    this->m_composedImage = new Image(this->m_width, this->m_height);
    this->m_composedImage->fill(Color::from_rgba(0, 0, 0, 0));

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

        this->m_view->update();
    }
}

void ViewImpl::setY(double y)
{
    if (this->m_y != y) {
        this->m_y = y;

        this->m_view->update();
    }
}

void ViewImpl::setPosition(double x, double y)
{
    double old_x = this->m_x;
    double old_y = this->m_y;

    if (this->m_x != x) {
        this->m_x = x;
    }
    if (this->m_y != y) {
        this->m_y = y;
    }

    if (old_x != x || old_y != y) {
        this->m_view->update();
    }
}

void ViewImpl::setWidth(double width)
{
    if (this->m_width != width) {
        this->m_width = width;

        this->m_image->resize(width, this->m_height);

        this->update();
    }
}

void ViewImpl::setHeight(double height)
{
    if (this->m_height != height) {
        this->m_height = height;

        this->m_image->resize(this->m_width, height);

        this->update();
    }
}

void ViewImpl::setSize(double width, double height)
{
    double old_width = this->m_width;
    double old_height = this->m_height;

    if (this->m_height != height) {
        this->m_height = height;
    }
    if (this->m_width != width) {
        this->m_width = width;
    }

    if (old_width != width || old_height != height) {
        this->m_image->resize(this->m_width, this->m_height);

        this->update();
    }
}

void ViewImpl::fill(const Color& color)
{
    this->m_image->fill(color);
}

void ViewImpl::drawImage(uint64_t x, uint64_t y, const Image& image)
{
    this->m_image->add(image, x, y);
}

void ViewImpl::appendChild(View *view)
{
    (void)view;
    this->update();
}

const Image* ViewImpl::image()
{
    return this->m_composedImage;
}

//=================
// Private Slots
//=================

void ViewImpl::update()
{
    if (this->m_composedImage->width() != this->m_image->width() ||
            this->m_composedImage->height() != this->m_image->height()) {
        this->m_composedImage->resize(
            this->m_image->width(),
            this->m_image->height()
        );
    }
    this->m_composedImage->fill(Color::from_rgba(0, 0, 0, 0));
    this->m_composedImage->add(*this->m_image, 0, 0);
    for (auto& child: this->m_view->_children) {
        this->m_composedImage->add(*(child->_impl->m_composedImage),
            child->x(), child->y());
    }
}

} // namespace bl
