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

    this->_state = View::State::Normal;
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

void View::set_position(const Point &pos)
{
    this->_impl->setPosition(pos.x(), pos.y());
}

void View::set_width(double width)
{
    this->_impl->setWidth(width);
}

void View::set_height(double height)
{
    this->_impl->setHeight(height);
}

void View::set_size(const Size &size)
{
    this->_impl->setSize(size.width(), size.height());
}

Rect View::geometry() const
{
    return Rect(this->x(), this->y(), this->width(), this->height());
}

Color View::color() const
{
    return this->_impl->color();
}

void View::set_color(const Color &color)
{
    this->_impl->setColor(color);
}

void View::fill(const Color& color)
{
    this->_impl->fill(color);
}

void View::draw_image(const Point& pos, const Image& image)
{
    this->_impl->drawImage(pos.x(), pos.y(), image);
}

View* View::child_at(const Point &pos)
{
    // Return nullptr if position is out of this view.
    if (pos.x() > this->width() ||
            pos.y() > this->height()) {
        return nullptr;
    }

    // Reverse for loop.
    for (uint64_t i = this->_children.length(); i > 0; --i) {
        View *child = this->_children[i - 1];
        if (child->geometry().contains(pos)) {
            return child;
        }
    }

    return nullptr;
}

Surface* View::surface()
{
    return this->_surface;
}

void View::set_surface(Surface *surface)
{
    this->_surface = surface;
}

void View::update()
{
    this->_impl->update();

    // Update parent.
    if (this->_parent != nullptr) {
        this->_parent->update();
    }

    if (this->_surface == nullptr) {
        fprintf(stderr, "[WARN] View::update() surface is null!\n");
        fprintf(stderr, " - this: %p, this->_parent: %p\n", this, this->_parent);
        return;
    }
    this->_surface->update();
}

//=================
// Events
//=================

void View::pointer_enter_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void View::pointer_leave_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void View::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
    this->_state = View::State::Active;
    fprintf(stderr, "View::pointer_press_event() - state now %d. %p\n",
        (int)this->_state, this);
}

void View::pointer_release_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr,
        "View::pointer_release_event() - State: %d. %p\n",
        (int)this->_state, this);
    if (this->_state == View::State::Active) {
        this->_state = View::State::Normal;

        fprintf(stderr, "View::pointer_release_event() - post click event.\n");
        this->pointer_click_event(event);
    }
}

void View::pointer_click_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void View::pointer_double_click_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void View::pointer_move_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

} // namespace bl
