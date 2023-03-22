#include "view-impl.h"

// C
#include <stdio.h>

#include <blusher/view.h>
#include <blusher/application.h>

namespace bl {

View* ViewImpl::_pointer_entered_view = nullptr;

ViewImpl::ViewImpl()
{
    this->m_x = 0.0;
    this->m_y = 0.0;
    this->m_width = 10.0;
    this->m_height = 10.0;

    this->_fill_type = View::FillType::Color;

    this->m_image = new Image(this->m_width, this->m_height);
    this->m_image->fill(Color::from_rgba(0, 0, 0, 0));

    this->m_composedImage = new Image(this->m_width, this->m_height);
    this->m_composedImage->fill(Color::from_rgba(0, 0, 0, 0));
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

        this->_view->update();
    }
}

void ViewImpl::setY(double y)
{
    if (this->m_y != y) {
        this->m_y = y;

        this->_view->update();
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
        this->_view->update();
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

View::FillType ViewImpl::fill_type() const
{
    return this->_fill_type;
}

void ViewImpl::set_fill_type(View::FillType fill_type)
{
    this->_fill_type = fill_type;
}

void ViewImpl::fill(const Color& color)
{
    this->m_image->fill(color);

    this->_color = color;
}

Color ViewImpl::color() const
{
    return this->_color;
}

void ViewImpl::drawImage(uint64_t x, uint64_t y, const Image& image)
{
    if (this->fill_type() != View::FillType::Image) {
        fprintf(stderr, "[WARN] View - fill_type is not Image.\n");
        return;
    }
    this->m_image->add(image, x, y);
}

const Image& ViewImpl::image() const
{
    return *this->m_image;
}

void ViewImpl::appendChild(View *view)
{
    (void)view;
    this->update();
}

void ViewImpl::process_pointer_move_event(
        std::shared_ptr<PointerEvent> event)
{
    auto x = event->x();
    auto y = event->y();
    View *child = this->_view->child_at(Point(x, y));
    if (child != nullptr) {
        auto child_x = x - child->x();
        auto child_y = y - child->y();
        auto evt = std::make_shared<PointerEvent>(Event::Type::PointerMove,
            event->button(), child_x, child_y);
        app->event_dispatcher()->post_event(child, evt);
        return;
    }
    // Pointer enter event.
    if (this->_view != ViewImpl::_pointer_entered_view) {
        auto evt = std::make_shared<PointerEvent>(Event::Type::PointerEnter,
            event->button(), 0, 0);
        app->event_dispatcher()->post_event(this->_view, evt);
        ViewImpl::_pointer_entered_view = this->_view;
    }

    fprintf(stderr, "POINTER MOVE %s - (%f, %f)\n",
        this->_view->debug_id().c_str(),
        x, y);
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
    for (auto& child: this->_view->_children) {
        this->m_composedImage->add(*(child->_impl->m_composedImage),
            child->x(), child->y());
    }
}

} // namespace bl
