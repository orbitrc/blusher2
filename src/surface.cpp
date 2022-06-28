#include <blusher/surface.h>

#include "surface-impl.h"

#include <stdio.h>

#include <linux/input.h>

#include <blusher/point.h>
#include <blusher/rect.h>
#include <blusher/utils.h>

#ifdef emit
    #undef emit
#endif

namespace bl {

Surface::Surface(Surface *parent)
{
    if (parent != nullptr) {
        this->_impl = new SurfaceImpl(parent->_impl);
    } else {
        this->_impl = new SurfaceImpl();
    }

    this->_impl->setBlSurface(this);

    this->_parent = parent;
    this->_state = State::Normal;
    this->_current_view = nullptr;

    this->color_changed.connect([]() { fprintf(stderr, "Hello, color_changed!\n"); });

    // Set event handlers.
    this->_impl->setPointerEnterHandler(&Surface::pointer_enter_handler);
    this->_impl->setPointerLeaveHandler(&Surface::pointer_leave_handler);
    this->_impl->setPointerPressHandler(&Surface::pointer_press_handler);
    this->_impl->setPointerReleaseHandler(&Surface::pointer_release_handler);
    this->_impl->setPointerMoveHandler(&Surface::pointer_move_handler);
}

//=================
// Public Methods
//=================

uint32_t Surface::height() const
{
    return this->_impl->height();
}

void Surface::paint()
{
    this->_impl->paint();
}

Surface* Surface::parent()
{
    return this->_parent;
}

void Surface::place_above(Surface *surface)
{
    this->_impl->placeAbove(surface->_impl);
}

void Surface::place_below(Surface *surface)
{
    this->_impl->placeBelow(surface->_impl);
}

void Surface::set_geometry(double x, double y, double width, double height)
{
    this->_impl->setX(x);
    this->_impl->setY(y);
    this->_impl->setSize(width, height);
}

void Surface::show()
{
    this->_impl->show();
}

bool Surface::toplevel() const
{
    return this->_impl->toplevel();
}

uint32_t Surface::width() const
{
    return this->_impl->width();
}

void Surface::move_if_window()
{
    fprintf(stderr, "Surface::move_if_window() - parent: %p\n", this->_parent);
    if (this->_parent == nullptr) {
//        this->_impl->startSystemMove();
        this->_impl->moveIfToplevel();
    }
}

void Surface::resize_if_window()
{
    if (this->_parent == nullptr) {
        this->_impl->resizeIfToplevel();
    }
}

View* Surface::root_view()
{
    return this->_impl->rootView();
}

void Surface::update()
{
    this->_impl->update();
}

//=================
// Events
//=================

void Surface::pointer_enter_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void Surface::pointer_leave_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void Surface::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void Surface::pointer_release_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

void Surface::pointer_move_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;
}

//=================
// Private Slots
//=================

void Surface::on_clicked()
{
}

//==================
// Private Methods
//==================

void Surface::pointer_enter_handler()
{
    this->_state = State::Hover;

    Button button = Button::None;

    auto event = std::make_shared<PointerEvent>(button, 0, 0);

    this->pointer_enter_event(event);
}

void Surface::pointer_leave_handler()
{
    this->_state = State::Normal;

    Button button = Button::None;

    auto event = std::make_shared<PointerEvent>(button, 0, 0);

    this->pointer_leave_event(event);
}

void Surface::pointer_press_handler(uint32_t impl_button, double x, double y)
{
    Button button;
    switch (impl_button) {
    case BTN_LEFT:
        button = Button::Left;
        // Set surface state to active.
        this->_state = State::Active;
        break;
    case BTN_RIGHT:
        button = Button::Right;
        break;
    case BTN_MIDDLE:
        button = Button::Middle;
        break;
    default:
        break;
    }

    // Surface.
    auto event = std::make_shared<PointerEvent>(button, x, y);

    this->pointer_press_event(event);

    // View.
    auto root_view = this->_impl->rootView();
    View *view = root_view->child_at(Point(x, y));
    if (view != nullptr) {
        fprintf(stderr, "view found. send event to %p\n", view);
        // TODO: Fix x, y position!
        auto event = std::make_shared<PointerEvent>(button, x, y);

        view->pointer_press_event(event);
    } else {
        fprintf(stderr, "view is nullptr. send event to root_view.\n");
        auto event = std::make_shared<PointerEvent>(button, x, y);

        root_view->pointer_press_event(event);
    }
}

void Surface::pointer_release_handler(uint32_t impl_button, double x, double y)
{
    Button button;
    switch (impl_button) {
    case BTN_LEFT:
        button = Button::Left;
        break;
    case BTN_RIGHT:
        button = Button::Right;
        break;
    case BTN_MIDDLE:
        button = Button::Middle;
        break;
    default:
        break;
    }

    auto event = std::make_shared<PointerEvent>(button, x, y);

    this->pointer_release_event(event);
}

void Surface::pointer_move_handler(uint32_t impl_button, double x, double y)
{
    Button button;
    switch (impl_button) {
    case BTN_LEFT:
        button = Button::Left;
        break;
    case BTN_RIGHT:
        button = Button::Right;
        break;
    case BTN_MIDDLE:
        button = Button::Middle;
        break;
    default:
        break;
    }

    // Surface.
    auto event = std::make_shared<PointerEvent>(button, x, y);

    this->pointer_move_event(event);

    // View.
    auto root_view = this->_impl->rootView();
    View *view = root_view->child_at(Point(x, y));
    if (view != nullptr) {
        // Send pointer enter event to view.
        if (this->_current_view != view) {
            this->_current_view = view;

            auto event = std::make_shared<PointerEvent>(Button::None, x, y);

            view->pointer_enter_event(event);
        }

        // TODO: Fix x, y position!
        auto event = std::make_shared<PointerEvent>(button, x, y);

        view->pointer_move_event(event);
    } else {
        // Send pointer leave event if leaved, and send pointer enter event
        // to the root view.
        {
            if (this->_current_view != nullptr) {
                auto event = std::make_shared<PointerEvent>(Button::None, x, y);

                this->_current_view->pointer_leave_event(event);
            }
            this->_current_view = root_view;

            auto event = std::make_shared<PointerEvent>(Button::None, x, y);

            root_view->pointer_enter_event(event);
        }

        auto event = std::make_shared<PointerEvent>(button, x, y);

        root_view->pointer_move_event(event);
    }
}

} // namespace bl
