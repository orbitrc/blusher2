#include <blusher/title-bar.h>

// C
#include <stdio.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/color.h>
#include <blusher/point.h>
#include <blusher/size.h>
#include <blusher/svg.h>
#include <blusher/resource.h>

namespace bl {

//================
// Title Bar
//================

TitleBar::TitleBar(View *parent)
    : View(parent)
{
    fprintf(stderr, "TitleBar::TitleBar() - parent width: %d\n", (int)parent->width());
    this->set_debug_id("WindowTitleBar"_S);
    this->set_position(Point(0, 0));
    this->set_size(Size(parent->width(), 30));

    // Initialize.
    this->_body = nullptr;
    this->_window = nullptr;
    this->_main_view = nullptr;

    // Set main view.
    this->set_size(Size(this->width(), this->height()));
    this->fill(Color::from_rgb(0xc0, 0xc0, 0xc0));

    // Add buttons.
    TitleBarButton *close_button = new TitleBarButton(this);
    close_button->set_type(TitleBarButton::Type::Close);
    close_button->set_position(Point(5, 2));
}

Surface* TitleBar::body()
{
    return this->_body;
}

void TitleBar::set_body(Surface *surface)
{
    this->_body = surface;
}

Window* TitleBar::window()
{
    return this->_window;
}

void TitleBar::set_window(Window *window)
{
    this->_window = window;
}

//===============
// Events
//===============

void TitleBar::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    if (event->button() == PointerButton::Left) {
        fprintf(stderr, "TitleBar::pointer_press_event() - Button::Left\n");
        DesktopSurface *window =
            static_cast<DesktopSurface*>(this->window());
        window->toplevel_move();
    }

    event->set_propagation(false);

    return View::pointer_press_event(event);
}

//==================
// Title Bar View
//==================

TitleBarView::TitleBarView(View *parent)
    : View(parent)
{
    // TODO: Initialize.
}

//==========================
// Title Bar View: Events
//==========================

void TitleBarView::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    return View::pointer_press_event(event);
}

//=============
// Button
//=============

TitleBarButton::TitleBarButton(View *parent)
    : View(parent)
{
    this->_close_image = nullptr;

    this->set_fill_type(View::FillType::Image);

    this->set_debug_id("Title bar button"_S);

    this->set_width(26);
    this->set_height(26);

    Svg close_svg("brc:/io.orbitrc.blusher/hydrogen-close.svg"_S);
    auto close_image = close_svg.to_image(26, 26);
    this->_close_image = new Image(26, 26);
    this->_close_image->add(close_image, 0, 0);

    // Set image.
    this->fill(Color::from_rgb(0, 0, 0));
    this->draw_image(Point(0, 0), *(this->_close_image));
    this->update();
}

void TitleBarButton::set_type(TitleBarButton::Type type)
{
    this->_type = type;
}

//=================
// Button: Events
//=================

void TitleBarButton::pointer_enter_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "[LOG] TitleBarButton::pointer_enter_event()\n");
    this->fill(Color::from_rgb(150, 0, 0));
    this->update();

    static_cast<Surface*>(this->surface())->update();

    return View::pointer_enter_event(event);
}

void TitleBarButton::pointer_leave_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "[LOG] TitleBarButton::pointer_leave_event()\n");
    this->fill(Color::from_rgba(0, 0, 0, 0));
    this->draw_image(Point(0, 0), *this->_close_image);
    this->update();

    static_cast<Surface*>(this->surface())->update();

    return View::pointer_leave_event(event);
}

void TitleBarButton::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "Title bar button pressed.\n");

    event->set_propagation(false);

    return View::pointer_press_event(event);
}

void TitleBarButton::pointer_click_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "Title bar button clicked.\n");
    if (this->_type == TitleBarButton::Type::Close) {
        auto desktop_surface = static_cast<DesktopSurface*>(this->surface());
        desktop_surface->close();
    }

    return View::pointer_click_event(event);
}

} // namespace bl
