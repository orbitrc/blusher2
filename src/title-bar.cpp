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
    this->_close_image = nullptr;

    Svg close_svg("brc:/io.orbitrc.blusher/hydrogen-close.svg"_S);
    auto close_image = close_svg.to_image(26, 26);
    this->_close_image = new Image(26, 26);
    this->_close_image->add(close_image, 0, 0);

    // Add buttons.
    TitleBarButton *close_button = new TitleBarButton(this->root_view());
    close_button->set_x(5);
    close_button->set_y(2);
    close_button->set_width(26);
    close_button->set_height(26);
    close_button->set_color(Color::from_rgba(0, 0, 0, 0));
    close_button->draw_image(Point(0, 0), *(this->_close_image));
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
        for (Surface *it = this; it->parent() != nullptr; it = it->parent()) {
            Surface *win = it->parent();
            if (win->parent() == nullptr) {
                fprintf(stderr, "bl::TitleBar::pointer_press_event() - parent is NULL\n");
                win->move_if_window();
            }
        }
    }

    return Surface::pointer_press_event(event);
}

//=============
// Button
//=============

TitleBarButton::TitleBarButton(View *parent)
    : View(parent)
{
}

//==============
// Events
//==============

void TitleBarButton::pointer_enter_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "[LOG] TitleBarButton::pointer_enter_event()\n");
    // this->fill(Color::from_rgb(150, 0, 0));

    static_cast<TitleBar*>(this->surface())->body()->update();

    return View::pointer_enter_event(event);
}

void TitleBarButton::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "Title bar button pressed.\n");

    return View::pointer_press_event(event);
}

} // namespace bl
