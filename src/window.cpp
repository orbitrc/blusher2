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

    // Init decoration.
    this->_decoration = new View(this->root_view());
    this->_decoration->set_debug_id("Decoration"_S);

    // Init resize.
    this->_resize = new Resize(this->_decoration);
    this->_resize->set_window(this);
    this->_resize->set_position(Point(20, 20));
    this->_resize->set_debug_id("Resize"_S);

    // Init border.
    this->_border = new View(this->_resize);
    this->_border->set_debug_id("Border"_S);

    // Init body.
    this->_body = new View(this->_border);
    this->_body->set_position(Point(
        BLUSHER_BORDER_WIDTH,
        BLUSHER_BORDER_WIDTH + BLUSHER_TITLE_BAR_HEIGHT));
    this->_body->set_size(Size(300, 300));
    this->_body->fill(Color::from_rgb(255, 255, 255));
    this->_body->set_debug_id("WindowBody"_S);
    this->_body->paint();

    // Init title bar.
    this->_title_bar = new TitleBar(this->_border);
    this->_title_bar->set_body(this);
    this->_title_bar->set_window(this);

    this->set_geometry(0, 0, 300, 300);
    this->set_geometry_hint(Rect(
        BLUSHER_SHADOW_WIDTH - BLUSHER_BORDER_WIDTH,
        BLUSHER_SHADOW_WIDTH - BLUSHER_BORDER_WIDTH,
        this->width(),
        this->height()
    ));

//    this->_border->place_below(this->_body);

    /*
    this->_decoration = new Surface(this->_body);
    this->_decoration->set_debug_id("WindowShadow"_S);
    this->_decoration->place_below(this->_resize);
    */

    // Init title bar.
    this->update_title_bar();

    // Init resize.
    this->update_resize();

    // Init border.
    this->update_border();

    // Init decoration.
    this->update_decoration();

    this->request_update();
}

//===================
// Public Methods
//===================

void Window::show()
{
    DesktopSurface::show();
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

uint32_t Window::width() const
{
    return DesktopSurface::width()
        - (BLUSHER_SHADOW_WIDTH * 2)
        + (BLUSHER_BORDER_WIDTH * 2);
}

uint32_t Window::height() const
{
    return DesktopSurface::height()
        - (BLUSHER_SHADOW_WIDTH * 2)
        + (BLUSHER_BORDER_WIDTH * 2);
}

View* Window::body_view()
{
    return this->_body;
}

//====================
// Private Methods
//====================

void Window::update_decoration()
{
    /*
    this->_decoration->set_geometry(
        0 - BLUSHER_SHADOW_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_SHADOW_WIDTH,
        this->width() + (BLUSHER_SHADOW_WIDTH * 2),
        this->height() + (BLUSHER_SHADOW_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    */
    this->_decoration->set_width(DesktopSurface::width());
    this->_decoration->set_height(DesktopSurface::height());
    this->_decoration->fill(Color::from_rgba(0, 0, 0, 100));
    this->_decoration->paint();
}

void Window::update_resize()
{
    /*
    this->_resize->set_geometry(
        0 - BLUSHER_RESIZE_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_RESIZE_WIDTH,
        this->width() + (BLUSHER_RESIZE_WIDTH * 2),
        this->height() + (BLUSHER_RESIZE_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    */
    this->_resize->set_position(Point(
        BLUSHER_SHADOW_WIDTH - BLUSHER_RESIZE_WIDTH,
        BLUSHER_SHADOW_WIDTH - BLUSHER_RESIZE_WIDTH));
    float width = DesktopSurface::width()
        - (BLUSHER_SHADOW_WIDTH * 2)
        + (BLUSHER_RESIZE_WIDTH * 2);
    float height = DesktopSurface::height()
        - (BLUSHER_SHADOW_WIDTH * 2)
        + (BLUSHER_RESIZE_WIDTH * 2);
    this->_resize->set_size(Size(width, height));
    this->_resize->fill(Color::from_rgba(255, 0, 0, 100));
    this->_resize->paint();
}

void Window::update_border()
{
    /*
    this->_border->set_geometry(
        0 - BLUSHER_BORDER_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_BORDER_WIDTH,
        this->width() + (BLUSHER_BORDER_WIDTH * 2),
        this->height() + (BLUSHER_BORDER_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    */
    // Border is child of the resize.
    this->_border->set_position(Point(
        BLUSHER_RESIZE_WIDTH - BLUSHER_BORDER_WIDTH,
        BLUSHER_RESIZE_WIDTH - BLUSHER_BORDER_WIDTH));
    float width = this->_resize->width()
        - (BLUSHER_RESIZE_WIDTH * 2)
        + (BLUSHER_BORDER_WIDTH * 2);
    float height = this->_resize->height()
        - (BLUSHER_RESIZE_WIDTH * 2)
        + (BLUSHER_BORDER_WIDTH * 2);
    this->_border->set_size(Size(width, height));
    this->_border->fill(Color::from_rgb(0, 0, 0));
    this->_border->paint();
}

void Window::update_title_bar()
{
    this->_title_bar->set_position(Point(1, 1));
    this->_title_bar->set_size(Size(
        this->width() - (BLUSHER_BORDER_WIDTH * 2),
        BLUSHER_TITLE_BAR_HEIGHT));
    this->_title_bar->fill(Color::from_rgb(100, 100, 100));
    this->_title_bar->paint();
}

Size Window::_body_size() const
{
    float width = this->width() - (BLUSHER_BORDER_WIDTH * 2);
    float height = this->height()
        - (BLUSHER_BORDER_WIDTH * 2)
        - BLUSHER_TITLE_BAR_HEIGHT;

    return Size(width, height);
}

//===============
// Events
//===============

void Window::resize_event(std::shared_ptr<ResizeEvent> event)
{
    (void)event;

    if (this->_resize != nullptr) {
        this->update_resize();
    }
    if (this->_border != nullptr) {
        this->update_border();
    }
    if (this->_decoration != nullptr) {
        this->update_decoration();
    }
    if (this->_title_bar != nullptr) {
        this->update_title_bar();
    }
    // Update body.
    this->_body->set_size(this->_body_size());

    // Update geometry hint.
    this->set_geometry_hint(Rect(
        BLUSHER_SHADOW_WIDTH - BLUSHER_BORDER_WIDTH,
        BLUSHER_SHADOW_WIDTH - BLUSHER_BORDER_WIDTH,
        this->width(), this->height()));

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
    return nullptr; // static_cast<Resize*>(const_cast<ResizeView*>(this)->surface());
}

//===============
// Resize
//===============

Resize::Resize(View *parent)
    : View(parent)
{
    // this->_main_view = new ResizeView(this->root_view());
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

Window* Resize::window()
{
    return this->_window;
}

void Resize::set_window(Window *window)
{
    this->_window = window;
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
        if (this->_window != nullptr) {
            DesktopSurface *window = static_cast<DesktopSurface*>(this->_window);
            auto edge = this->resize_edge(Point(event->x(), event->y()));
            fprintf(stderr, " - Resize pointer_press_event. edge: %d\n", (int)edge);
            window->toplevel_resize(edge);
        }
    }

    return View::pointer_press_event(event);
}

} // namespace bl
