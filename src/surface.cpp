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
    : _wl_surface(app->wl_compositor()->create_surface())
{
    fprintf(stderr, "Surface::Surface() - parent: %p\n", parent);
    this->_type = Surface::Type::Normal;
    this->_wl_subsurface = nullptr;

    if (parent != nullptr) {
        this->_impl = new SurfaceImpl(this, parent->_impl);
        this->_type = Surface::Type::Subsurface;
    } else {
        this->_impl = new SurfaceImpl(this);
        this->_type = Surface::Type::Normal;
    }

    this->_parent = parent;
    this->_state = State::Normal;
    this->_current_view = nullptr;
    this->_pointer_state = std::make_shared<PointerState>();

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

const WlSurface& Surface::wl_surface() const
{
    return this->_wl_surface;
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
    View *view = root_view->child_at(Point(x, y));
    if (view != nullptr) {
        fprintf(stderr, "view found. send event to %p\n", view);
        // TODO: Fix x, y position!
        auto event = std::make_shared<PointerEvent>(Event::Type::PointerPress,
            button, x, y);

        app->event_dispatcher()->post_event(view, event);
    }
}

void Surface::pointer_release_handler(uint32_t impl_button, double x, double y)
{
    PointerButton button = libinput_btn_to_button(impl_button);

    auto event = std::make_shared<PointerEvent>(Event::Type::PointerRelease,
        button, x, y);

    this->pointer_release_event(event);

    // View.
    auto root_view = this->_impl->rootView();
    auto view = root_view->child_at(Point(x, y));
    if (view != nullptr) {
        auto event = std::make_shared<PointerEvent>(
            Event::Type::PointerRelease,
            button, x, y);

        app->event_dispatcher()->post_event(view, event);
    }
}

void Surface::pointer_move_handler(uint32_t impl_button, double x, double y)
{
    PointerButton button = libinput_btn_to_button(impl_button);

    // Surface.
    auto event = std::make_shared<PointerEvent>(Event::Type::PointerMove,
        button, x, y);

    // View.
    auto root_view = this->_impl->rootView();
    View *view = root_view->child_at(Point(x, y));
    if (view != nullptr) {
        // Send pointer enter event to view.
        if (this->_current_view != view) {
            // Send pointer leave event to previous view.
            if (this->_current_view != nullptr) {
                auto event = std::make_shared<PointerEvent>(
                    Event::Type::PointerLeave,
                    PointerButton::None, x, y);

                app->event_dispatcher()->post_event(this->_current_view, event);
            }
            this->_current_view = view;

            auto event = std::make_shared<PointerEvent>(
                Event::Type::PointerEnter,
                PointerButton::None, x, y);

            app->event_dispatcher()->post_event(view, event);
        }

        // TODO: Fix x, y position!
        auto event = std::make_shared<PointerEvent>(Event::Type::PointerMove,
            button, x, y);

        app->event_dispatcher()->post_event(view, event);
    } else {
        // Send pointer leave event if leaved, and send pointer enter event
        // to the root view.
        {
            if (this->_current_view != nullptr) {
                auto event = std::make_shared<PointerEvent>(
                    Event::Type::PointerLeave,
                    PointerButton::None, x, y);

                this->_current_view->pointer_leave_event(event);
            }
            if (this->_current_view != root_view) {
                this->_current_view = root_view;

                auto event = std::make_shared<PointerEvent>(
                    Event::Type::PointerEnter,
                    PointerButton::None, x, y);

                app->event_dispatcher()->post_event(root_view, event);
            }
        }

        auto event = std::make_shared<PointerEvent>(
            Event::Type::PointerMove,
            button, x, y);

        app->event_dispatcher()->post_event(root_view, event);
    }
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

} // namespace bl
