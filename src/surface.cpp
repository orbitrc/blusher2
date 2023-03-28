#include <blusher/surface.h>

#include "surface-impl.h"

#include <stdio.h>

#include <linux/input.h>

#include <blusher/application.h>
#include <blusher/point.h>
#include <blusher/rect.h>
#include <blusher/utils.h>
#include <blusher/pointer-event.h>
#include <blusher/resize-event.h>

#ifdef emit
    #undef emit
#endif

namespace bl {

Surface::Surface(Surface *parent)
{
    fprintf(stderr, "Surface::Surface() - parent: %p\n", parent);
    this->_type = Surface::Type::Normal;
    this->_wl_subsurface = nullptr;

    if (parent != nullptr) {
        this->_impl = new SurfaceImpl(this);
        this->_type = Surface::Type::Subsurface;
    } else {
        this->_impl = new SurfaceImpl(this);
        this->_type = Surface::Type::Normal;
    }

    this->_parent = parent;
    this->_state = State::Normal;
    this->_current_view = nullptr;
    this->_pointer_state = std::make_shared<PointerState>();
    this->_update_requested = false;

    // Debug info.
    char pointer_id[15];
    sprintf(pointer_id, "%p", this);
    this->_debug_id = pr::String(pointer_id);

    app->add_surface(this);

    this->color_changed.connect([]() { fprintf(stderr, "Hello, color_changed!\n"); });
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

void Surface::set_size(uint32_t width, uint32_t height)
{
    this->_impl->setSize(width, height);
}

void Surface::show()
{
    this->_impl->show();
}

void Surface::hide()
{
    this->_impl->hide();
}

bool Surface::visible() const
{
    return this->_impl->visible();
}

Surface::Type Surface::type() const
{
    return this->_type;
}

bool Surface::is_toplevel() const
{
    fprintf(stderr, "[WARN] Surface::is_toplevel() is deprecated!\n");
    return this->_impl->isToplevel();
}

bool Surface::is_subsurface() const
{
    return this->_type == Surface::Type::Subsurface;
}

uint32_t Surface::width() const
{
    return this->_impl->width();
}

View* Surface::root_view()
{
    return this->_impl->rootView();
}

void Surface::process_pointer()
{
}

void Surface::request_update()
{
    this->_update_requested = true;
}

void Surface::update()
{
    this->_impl->update();
}

pr::String Surface::debug_id() const
{
    return this->_debug_id;
}

void Surface::set_debug_id(const pr::String& id)
{
    this->_debug_id = id;
}

std::shared_ptr<PointerState> Surface::pointer_state()
{
    return this->_pointer_state;
}

std::shared_ptr<WlSurface> Surface::wl_surface() const
{
    return this->_impl->wl_surface();
}

std::shared_ptr<WlSubsurface> Surface::wl_subsurface()
{
    return this->_wl_subsurface;
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

void Surface::resize_event(std::shared_ptr<ResizeEvent> event)
{
    (void)event;
}

//==================
// Private Methods
//==================

void Surface::pointer_enter_handler()
{
    this->_state = State::Hover;

    PointerButton button = PointerButton::None;

    auto event = std::make_shared<PointerEvent>(Event::Type::PointerEnter,
        button, 0, 0);

    this->pointer_enter_event(event);
}

void Surface::pointer_leave_handler()
{
    this->_state = State::Normal;

    PointerButton button = PointerButton::None;

    auto event = std::make_shared<PointerEvent>(Event::Type::PointerLeave,
        button, 0, 0);

    this->pointer_leave_event(event);

    // Send leave event to last entered view.
    View *entered_view = this->entered_view();
    this->set_entered_view(nullptr);
    app->event_dispatcher()->post_event(entered_view, event);
}

void Surface::pointer_press_handler(uint32_t impl_button, double x, double y)
{
    PointerButton button = libinput_btn_to_button(impl_button);

    // Surface.
    auto event = std::make_shared<PointerEvent>(Event::Type::PointerPress,
        button, x, y);

    this->pointer_press_event(event);

    // View.
    auto root_view = this->_impl->rootView();
    app->event_dispatcher()->post_event(root_view, event);
}

void Surface::pointer_release_handler(uint32_t impl_button, double x, double y)
{
    PointerButton button = libinput_btn_to_button(impl_button);

    auto event = std::make_shared<PointerEvent>(Event::Type::PointerRelease,
        button, x, y);

    this->pointer_release_event(event);

    // View.
    auto root_view = this->_impl->rootView();
    app->event_dispatcher()->post_event(root_view, event);
}

void Surface::pointer_move_handler(uint32_t impl_button, double x, double y)
{
    PointerButton button = libinput_btn_to_button(impl_button);

    // Surface.
    auto event = std::make_shared<PointerEvent>(Event::Type::PointerMove,
        button, x, y);

    // View.
    auto root_view = this->_impl->rootView();

    app->event_dispatcher()->post_event(root_view, event);
    return;
}

void Surface::resize_handler(int32_t width, int32_t height,
        int32_t old_width, int32_t old_height)
{
    Size size(width, height);
    Size old_size(old_width, old_height);

    std::shared_ptr<ResizeEvent> event =
        std::make_shared<ResizeEvent>(size, old_size);

    this->resize_event(event);
}


View* Surface::entered_view() const
{
    return this->_impl->entered_view();
}

void Surface::set_entered_view(View *view)
{
    this->_impl->set_entered_view(view);
}

} // namespace bl
