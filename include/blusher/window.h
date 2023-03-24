#ifndef _BL_WINDOW_H
#define _BL_WINDOW_H

// C
#include <stdint.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/desktop-surface.h>
#include <blusher/point.h>
#include <blusher/rect.h>
#include <blusher/view.h>

namespace bl {

class TitleBar;

class Resize;

class Window : public DesktopSurface
{
public:
    Window();

    void show();

    void move();

    uint32_t title_bar_height() const;

    const pr::String& title() const;

    void set_title(const pr::String& title);

    /// Width of this window. The shadow and resize area are not included.
    uint32_t width() const;

    /// Height of this window. The shadow and resize area are not included.
    uint32_t height() const;

    View* body_view();

private:
    void update_decoration();
    void update_resize();
    void update_border();
    void update_title_bar();

    Size _body_size() const;

protected:
    void resize_event(std::shared_ptr<ResizeEvent> event) override;

private:
    View *_decoration;
    Resize *_resize;
    View *_border;
    TitleBar *_title_bar;
    View *_body;
    pr::String _title;
};

//=======================
// Window Resize View
//=======================
class ResizeView : public View
{
public:
    ResizeView(View *parent);

    Resize* resize_surface() const;
};

//=================
// Window Resize
//=================
class Resize : public View
{
public:
    Resize(View *parent);

    XdgToplevel::ResizeEdge resize_edge(const Point& pos) const;

    Surface* body();
    void set_body(Surface *surface);

    Window* window();
    void set_window(Window *window);

    uint32_t resize_corner_size() const;

private:
    Rect _top_left_rect() const;
    Rect _top_rect() const;
    Rect _top_right_rect() const;
    Rect _left_rect() const;
    Rect _right_rect() const;
    Rect _bottom_left_rect() const;
    Rect _bottom_rect() const;
    Rect _bottom_right_rect() const;

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;

private:
    Surface *_body;
    Window *_window;

    ResizeView *_main_view;
};

} // namespace bl

#endif /* _BL_WINDOW_H */
