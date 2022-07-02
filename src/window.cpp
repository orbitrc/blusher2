#include <blusher/window.h>

#include <stdio.h>

#include <blusher/title-bar.h>

#define BLUSHER_SHADOW_WIDTH 40
#define BLUSHER_RESIZE_WIDTH 5
#define BLUSHER_BORDER_WIDTH 1
#define BLUSHER_TITLE_BAR_HEIGHT 30

#ifdef __cplusplus
extern "C" {
#endif

static void register_resources()
{
    #include "../resources/resources.h"
    BL_REGISTER_RESOURCE()
}

#ifdef __cplusplus
}
#endif

namespace bl {

//=============
// Window
//=============

Window::Window()
    : Surface(nullptr)
{
    // Initialize.
    this->_body = nullptr;
    this->_title_bar = nullptr;
    this->_border = nullptr;
    this->_resize = nullptr;
    this->_decoration = nullptr;

    // Load resources.
    register_resources();

    // Init body.
    this->_body = static_cast<Surface*>(this);

    this->_body->set_geometry(0, 0, 200, 200);

    this->_title_bar = new TitleBar(this->_body);
    this->_title_bar->set_body(this);

    this->_border = new Surface(this->_body);
    this->_border->place_below(this->_body);

    this->_resize = new Resize(this->_body);
    this->_resize->place_below(this->_border);

    this->_decoration = new Surface(this->_body);
    this->_decoration->place_below(this->_resize);

    // Init title bar.
    this->update_title_bar();

    // Init decoration.
    this->update_decoration();

    // Init resize.
    this->update_resize();

    // Init border.
    this->update_border();
}

//===================
// Public Methods
//===================

void Window::show()
{
    this->_body->show();
    this->_body->paint();

    this->_title_bar->show();
    this->_title_bar->paint();

    this->_decoration->show();
    this->_decoration->paint();

    this->_resize->show();
    this->_resize->paint();

    this->_border->show();
    this->_border->paint();
}

void Window::move()
{
}

uint32_t Window::title_bar_height() const
{
    return BLUSHER_TITLE_BAR_HEIGHT;
}

//====================
// Private Methods
//====================

void Window::update_decoration()
{
    this->_decoration->set_geometry(
        0 - BLUSHER_SHADOW_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_SHADOW_WIDTH,
        this->width() + (BLUSHER_SHADOW_WIDTH * 2),
        this->height() + (BLUSHER_SHADOW_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_decoration->root_view()->set_color(Color::from_rgba(0, 0, 0, 100));
}

void Window::update_resize()
{
    this->_resize->set_geometry(
        0 - BLUSHER_RESIZE_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_RESIZE_WIDTH,
        this->width() + (BLUSHER_RESIZE_WIDTH * 2),
        this->height() + (BLUSHER_RESIZE_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_resize->root_view()->set_color(Color::from_rgba(255, 0, 0, 100));
}

void Window::update_border()
{
    this->_border->set_geometry(
        0 - BLUSHER_BORDER_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_BORDER_WIDTH,
        this->width() + (BLUSHER_BORDER_WIDTH * 2),
        this->height() + (BLUSHER_BORDER_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_border->root_view()->set_color(Color::from_rgb(0, 0, 0));
}

void Window::update_title_bar()
{
    this->_title_bar->set_geometry(
        0,
        -BLUSHER_TITLE_BAR_HEIGHT,
        this->width(),
        BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_title_bar->root_view()->set_color(Color::from_rgb(100, 100, 100));
}

//===============
// Events
//===============

void Window::resize_event(std::shared_ptr<ResizeEvent> event)
{
    (void)event;
    if (this->_border != nullptr) {
        this->update_border();
    }
    if (this->_resize != nullptr) {
        this->update_resize();
    }

    return Surface::resize_event(event);
}


//===============
// Resize
//===============

Resize::Resize(Surface *parent)
    : Surface(parent)
{
}

XdgToplevel::ResizeEdge Resize::resize_edge(const Point& pos) const
{
    if (pos.x() < 5 && pos.y() < 5) {
        return XdgToplevel::ResizeEdge::TopLeft;
    } else if (pos.x() > this->width() - 5 &&
            pos.y() < 5) {
        return XdgToplevel::ResizeEdge::TopRight;
    } else {
        return XdgToplevel::ResizeEdge::Bottom;
    }

    return XdgToplevel::ResizeEdge::None;
}

//=================
// Resize: Events
//=================

void Resize::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    if (event->button() == Button::Left) {
        for (Surface *it = this; it->parent() != nullptr; it = it->parent()) {
            Surface *win = it->parent();
            if (win->parent() == nullptr) {
                auto edge = this->resize_edge(Point(event->x(), event->y()));
                win->resize_if_window(edge);
            }
        }
    }

    return Surface::pointer_press_event(event);
}

} // namespace bl
