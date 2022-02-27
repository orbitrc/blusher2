#include <blusher/surface.h>

#include "surface-impl.h"

#include <stdio.h>

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

    this->_parent = parent;
    this->_state = State::Normal;

    this->set_color(Color::from_rgb(255, 255, 255));

    this->color_changed.connect([]() { fprintf(stderr, "Hello, color_changed!\n"); });

    this->_impl->setBlSurface(this);

    // Set event handlers.
    this->_impl->setPointerEnterHandler(&Surface::pointer_enter_handler);
    this->_impl->setPointerLeaveHandler(&Surface::pointer_leave_handler);
    this->_impl->setPointerPressHandler(&Surface::pointer_press_handler);
    this->_impl->setPointerReleaseHandler(&Surface::pointer_release_handler);
}

//=================
// Public Methods
//=================

double Surface::height() const
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

void Surface::set_color(const Color &color)
{
    this->_color = color;
    this->_impl->setColor(color);
    /* DEBUG */ this->color_changed.emit();
}

void Surface::set_geometry(double x, double y, double width, double height)
{
    this->_impl->setX(x);
    this->_impl->setY(y);
    this->_impl->setWidth(width);
    this->_impl->setHeight(height);
}

void Surface::show()
{
    this->_impl->show();
}

double Surface::width() const
{
    return this->_impl->x();
}

void Surface::move_if_window()
{
    fprintf(stderr, "Surface::move_if_window() - parent: %p\n", this->_parent);
    if (this->_parent == nullptr) {
//        this->_impl->startSystemMove();
    }
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

    PointerEvent::Button button = PointerEvent::Button::None;

    auto event = std::make_shared<PointerEvent>(button, 0, 0);

    this->pointer_enter_event(event);
}

void Surface::pointer_leave_handler()
{
    this->_state = State::Normal;

    PointerEvent::Button button = PointerEvent::Button::None;

    auto event = std::make_shared<PointerEvent>(button, 0, 0);

    this->pointer_leave_event(event);
}

void Surface::pointer_press_handler(int impl_button, double x, double y)
{
    PointerEvent::Button button;
    switch (impl_button) {
    case SurfaceImplButtonLeft:
        button = PointerEvent::Button::Left;
        // Set surface state to active.
        this->_state = State::Active;
        break;
    case SurfaceImplButtonRight:
        button = PointerEvent::Button::Right;
        break;
    case SurfaceImplButtonMiddle:
        button = PointerEvent::Button::Middle;
        break;
    default:
        break;
    }

    auto event = std::make_shared<PointerEvent>(button, x, y);

    this->pointer_press_event(event);
}

void Surface::pointer_release_handler(int impl_button, double x, double y)
{
    PointerEvent::Button button;
    switch (impl_button) {
    case SurfaceImplButtonLeft:
        button = PointerEvent::Button::Left;
        break;
    case SurfaceImplButtonRight:
        button = PointerEvent::Button::Right;
        break;
    case SurfaceImplButtonMiddle:
        button = PointerEvent::Button::Middle;
        break;
    default:
        break;
    }

    auto event = std::make_shared<PointerEvent>(button, x, y);

    this->pointer_release_event(event);
}

} // namespace bl
