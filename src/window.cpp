#include <blusher/window.h>

#include <stdio.h>

#include <blusher/title-bar.h>

#define BLUSHER_SHADOW_WIDTH 40
#define BLUSHER_RESIZE_WIDTH 5
#define BLUSHER_BORDER_WIDTH 1
#define BLUSHER_TITLE_BAR_HEIGHT 30

namespace bl {

//=============
// Window
//=============

Window::Window()
    : DesktopSurface(DesktopSurface::Role::Toplevel, nullptr)
{
    // Initialize.
    this->_body = nullptr;
    this->_title_bar = nullptr;
    this->_border = nullptr;
    this->_resize = nullptr;
    this->_decoration = nullptr;
    this->_title = "Application"_S;

    // Init body.
    this->_body = static_cast<Surface*>(this);

    this->_body->set_geometry(0, 0, 200, 200);
    this->_body->root_view()->fill(Color::from_rgb(255, 255, 255));

    this->_title_bar = new TitleBar(this->_body);
    this->_title_bar->set_body(this);

    this->_border = new Surface(this->_body);
    this->_border->place_below(this->_body);

    this->_resize = new Resize(this->_body);
    this->_resize->set_body(this);
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
    this->_body->update();

    this->_title_bar->show();
    this->_title_bar->update();

    this->_decoration->show();
    this->_decoration->update();

    this->_resize->show();
    this->_resize->update();

    this->_border->show();
    this->_border->update();

    // Set geometry hint.
    this->set_geometry_hint(Rect(0, 0, this->width(), this->height()));
}

void Window::move()
{
}

uint32_t Window::title_bar_height() const
{
    return BLUSHER_TITLE_BAR_HEIGHT;
}

const pr::String& Window::title() const
{
    return this->_title;
}

void Window::set_title(const pr::String& title)
{
    if (this->_title != title) {
        this->_title = title;
    }
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
    this->_decoration->root_view()->fill(Color::from_rgba(0, 0, 0, 100));
    this->_decoration->root_view()->paint();
}

void Window::update_resize()
{
    this->_resize->set_geometry(
        0 - BLUSHER_RESIZE_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_RESIZE_WIDTH,
        this->width() + (BLUSHER_RESIZE_WIDTH * 2),
        this->height() + (BLUSHER_RESIZE_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_resize->root_view()->fill(Color::from_rgba(255, 0, 0, 100));
    this->_resize->root_view()->paint();
}

void Window::update_border()
{
    this->_border->set_geometry(
        0 - BLUSHER_BORDER_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_BORDER_WIDTH,
        this->width() + (BLUSHER_BORDER_WIDTH * 2),
        this->height() + (BLUSHER_BORDER_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_border->root_view()->fill(Color::from_rgb(0, 0, 0));
    this->_border->root_view()->paint();
}

void Window::update_title_bar()
{
    this->_title_bar->set_geometry(
        0,
        -BLUSHER_TITLE_BAR_HEIGHT,
        this->width(),
        BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_title_bar->root_view()->fill(Color::from_rgb(100, 100, 100));
    this->_title_bar->root_view()->paint();
}

//===============
// Events
//===============

void Window::resize_event(std::shared_ptr<ResizeEvent> event)
{
    (void)event;
    // Update body.
    this->_body->root_view()->fill(Color::from_rgb(255, 255, 255));
    this->_body->root_view()->paint();

    if (this->_border != nullptr) {
        this->update_border();
    }
    if (this->_resize != nullptr) {
        this->update_resize();
    }

    // Update geometry hint.
    this->set_geometry_hint(Rect(0, 0, this->width(), this->height()));

    return Surface::resize_event(event);
}

//===============
// Resize View
//===============

ResizeView::ResizeView(View *parent)
    : View(parent)
{
}

Resize* ResizeView::resize_surface() const
{
    return static_cast<Resize*>(const_cast<ResizeView*>(this)->surface());
}

//===============
// Resize
//===============

Resize::Resize(Surface *parent)
    : Surface(parent)
{
    this->_main_view = new ResizeView(this->root_view());
}

XdgToplevel::ResizeEdge Resize::resize_edge(const Point& pos) const
{
    if (this->_top_left_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::TopLeft;
    } else if (this->_top_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::Top;
    } else if (this->_top_right_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::TopRight;
    } else if (this->_left_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::Left;
    } else if (this->_right_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::Right;
    } else if (this->_bottom_left_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::BottomLeft;
    } else if (this->_bottom_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::Bottom;
    } else if (this->_bottom_right_rect().contains(pos)) {
        return XdgToplevel::ResizeEdge::BottomRight;
    } else {
        return XdgToplevel::ResizeEdge::Bottom;
    }

    return XdgToplevel::ResizeEdge::None;
}

Surface* Resize::body()
{
    return this->_body;
}

void Resize::set_body(Surface *surface)
{
    this->_body = surface;
}

uint32_t Resize::resize_corner_size() const
{
    return 6;
}

//==========================
// Resize: Private Methods
//==========================

Rect Resize::_top_left_rect() const
{
    return Rect(
        0,
        0,
        this->resize_corner_size(),
        this->resize_corner_size()
    );
}

Rect Resize::_top_rect() const
{
    return Rect(
        this->resize_corner_size(),
        0,
        this->width() - (this->resize_corner_size() * 2),
        this->resize_corner_size()
    );
}

Rect Resize::_top_right_rect() const
{
    return Rect(
        this->width() - this->resize_corner_size(),
        0,
        this->resize_corner_size(),
        this->resize_corner_size()
    );
}

Rect Resize::_left_rect() const
{
    return Rect(
        0,
        this->resize_corner_size(),
        this->resize_corner_size(),
        this->height() - (this->resize_corner_size() * 2)
    );
}

Rect Resize::_right_rect() const
{
    return Rect(
        this->width() - this->resize_corner_size(),
        this->resize_corner_size(),
        this->resize_corner_size(),
        this->height() - (this->resize_corner_size() * 2)
    );
}

Rect Resize::_bottom_left_rect() const
{
    return Rect(
        0,
        this->height() - this->resize_corner_size(),
        this->resize_corner_size(),
        this->resize_corner_size()
    );
}

Rect Resize::_bottom_rect() const
{
    return Rect(
        0, 0, 0, 0
    );
}

Rect Resize::_bottom_right_rect() const
{
    return Rect(
        this->width() - this->resize_corner_size(),
        this->height() - this->resize_corner_size(),
        this->resize_corner_size(),
        this->resize_corner_size()
    );
}

//=================
// Resize: Events
//=================

void Resize::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    if (event->button() == PointerButton::Left) {
        if (this->_body != nullptr) {
            DesktopSurface *body = static_cast<DesktopSurface*>(this->_body);
            auto edge = this->resize_edge(Point(event->x(), event->y()));
            body->toplevel_resize(edge);
        }
    }

    return Surface::pointer_press_event(event);
}

} // namespace bl
