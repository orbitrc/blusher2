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
#include <blusher/svg.h>
#include <blusher/resource.h>

namespace bl {

//================
// Title Bar
//================

TitleBar::TitleBar(Surface *parent)
    : Surface(parent)
{
    fprintf(stderr, "TitleBar::TitleBar() - parent width: %d\n", parent->width());
    this->set_geometry(0, 0, parent->width(), 30);

    // Initialize.
    this->_body = nullptr;
    this->_main_view = nullptr;

    // Add main view.
    TitleBarView *v = new TitleBarView(this->root_view());
    this->_main_view = v;
    this->_main_view->set_size(Size(this->width(), this->height()));
    this->_main_view->fill(Color::from_rgb(0xc0, 0xc0, 0xc0));

    // Add buttons.
    TitleBarButton *close_button = new TitleBarButton(this->root_view());
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

//===============
// Events
//===============

void TitleBar::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    if (event->button() == Button::Left) {
        fprintf(stderr, "bl::TitleBar::pointer_press_event() - Button::Left\n");
        if (this->_body != nullptr) {
            DesktopSurface *body = static_cast<DesktopSurface*>(this->_body);
            // body->toplevel_move();
        }
    }

    return Surface::pointer_press_event(event);
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
    if (event->button() == Button::Left) {
        fprintf(stderr, "TitleBarView::pointer_press_event() - Button::Left\n");
        TitleBar *title_bar_surface = static_cast<TitleBar*>(this->surface());
        DesktopSurface *body =
            static_cast<DesktopSurface*>(title_bar_surface->body());
        body->toplevel_move();
    }

    return View::pointer_press_event(event);
}

//=============
// Button
//=============

TitleBarButton::TitleBarButton(View *parent)
    : View(parent)
{
    this->_close_image = nullptr;

    this->set_width(26);
    this->set_height(26);

    Svg close_svg("brc:/io.orbitrc.blusher/hydrogen-close.svg"_S);
    auto close_image = close_svg.to_image(26, 26);
    this->_close_image = new Image(26, 26);
    this->_close_image->add(close_image, 0, 0);

    // Set image.
    this->fill(Color::from_rgb(255, 0, 0));
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

    static_cast<TitleBar*>(this->surface())->body()->update();

    return View::pointer_enter_event(event);
}

void TitleBarButton::pointer_leave_event(std::shared_ptr<PointerEvent> event)
{
    this->fill(Color::from_rgba(0, 0, 0, 0));
    this->draw_image(Point(0, 0), *this->_close_image);
    this->update();

    static_cast<TitleBar*>(this->surface())->body()->update();

    return View::pointer_leave_event(event);
}

void TitleBarButton::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "Title bar button pressed.\n");

    return View::pointer_press_event(event);
}

void TitleBarButton::pointer_click_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "Title bar button clicked.\n");

    return View::pointer_click_event(event);
}

} // namespace bl
