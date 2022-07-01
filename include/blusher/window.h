#ifndef _BL_WINDOW_H
#define _BL_WINDOW_H

#include <stdint.h>

#include <blusher/wayland/xdg-toplevel.h>
#include <blusher/surface.h>
#include <blusher/point.h>

namespace bl {

class TitleBar;

class Resize;

class Window : public Surface
{
public:
    Window();

    void show();

    void move();

    uint32_t title_bar_height() const;

private:
    void update_decoration();
    void update_resize();
    void update_border();
    void update_title_bar();

protected:
    void resize_event(std::shared_ptr<ResizeEvent> event) override;

private:
    Surface *_decoration;
    Resize *_resize;
    Surface *_border;
    TitleBar *_title_bar;
    Surface *_body;
};


//=================
// Window Resize
//=================
class Resize : public Surface
{
public:
    Resize(Surface *parent);

    XdgToplevel::ResizeEdge resize_edge(const Point& pos) const;

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;

private:
};

} // namespace bl

#endif /* _BL_WINDOW_H */
