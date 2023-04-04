// C
#include <stdio.h>

// Blusher
#include <blusher/view.h>
#include <blusher/surface.h>
#include <blusher/application.h>
#include <blusher/wayland/wl-display.h>

#include "view-impl.h"

namespace bl {

View::View(View *parent)
{
    this->_impl = new ViewImpl();

    this->_parent = parent;

    // Add to children.
    if (parent != nullptr) {
        parent->_children.push(this);

        parent->_impl->appendChild(this);
    }

    // View reference for ViewImpl.
    this->_impl->_view = this;

    // Initialize.
    if (parent != nullptr) {
        this->_surface = parent->_surface;
    } else {
        this->_surface = nullptr;
    }

    this->_state = View::State::Normal;
    this->_painted = false;
}

View::~View()
{
    delete this->_impl;
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

View::FillType View::fill_type() const
{
    return this->_impl->fill_type();
}

void View::set_fill_type(FillType fill_type)
{
    this->_impl->set_fill_type(fill_type);
}

Rect View::geometry() const
{
    return Rect(this->x(), this->y(), this->width(), this->height());
}

View::State View::state() const
{
    return this->_state;
}

void View::fill(const Color& color)
{
    this->_impl->fill(color);

    this->_painted = false;
}

Color View::color() const
{
    return this->_impl->color();
}

void View::draw_image(const Point& pos, const Image& image)
{
    this->_impl->drawImage(pos.x(), pos.y(), image);

    this->_painted = false;
}

const Image& View::image() const
{
    return this->_impl->image();
}

pr::Vector<View*> View::children() const
{
    return this->_children;
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
    // Paint first if not painted.
    if (this->_painted == false) {
        this->paint();
    }

    // Update parent.
    if (this->_parent != nullptr) {
        // Root view may not painted manually. So force paint if root view.
        this->_parent->paint();
        this->_parent->update();
    }

    if (this->_surface == nullptr) {
        fprintf(stderr, "[WARN] View::update() surface is null!\n");
        fprintf(stderr, " - this: %p, this->_parent: %p\n", this, this->_parent);
        return;
    }
    this->_surface->request_update();

    // Update event.
    auto event = std::make_shared<UpdateEvent>();
    app->event_dispatcher()->post_event(this, event);
}

void View::paint()
{
    this->_impl->update();

    this->_painted = true;
}

pr::String View::debug_id() const
{
    return this->_debug_id;
}

void View::set_debug_id(const pr::String& debug_id)
{
    this->_debug_id = debug_id;
}

//=================
// Events
//=================

void View::pointer_enter_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
    if (this->_debug_id == ""_S) {
        fprintf(stderr, "View::pointer_enter_event() - %p\n", this);
    } else {
        fprintf(stderr, "View::pointer_enter_event() - %s\n",
            this->debug_id().c_str());
    }
}

void View::pointer_leave_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
    if (this->_debug_id == ""_S) {
        fprintf(stderr, "View::pointer_leave_event() - %p\n", this);
    } else {
        fprintf(stderr, "View::pointer_leave_event() - %s\n",
            this->debug_id().c_str());
    }
}

void View::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    this->_impl->process_pointer_press_event(event);
}

void View::pointer_release_event(std::shared_ptr<PointerEvent> event)
{
    auto x = event->x();
    auto y = event->y();

    View *child = this->child_at({x, y});
    if (child != nullptr) {
        auto child_x = x - child->x();
        auto child_y = y - child->y();
        auto evt = std::make_shared<PointerEvent>(Event::Type::PointerRelease,
            event->button(), child_x, child_y);
        app->event_dispatcher()->post_event(child, evt);
        return;
    }

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
    this->_impl->process_pointer_move_event(event);
}

void View::update_event(std::shared_ptr<UpdateEvent> event)
{
    (void)event;
    fprintf(stderr, "[LOG] View::update_event()\n");
    WlDisplay::instance()->dispatch();
}

} // namespace bl
