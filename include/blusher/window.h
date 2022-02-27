#ifndef _BL_WINDOW_H
#define _BL_WINDOW_H

#include <stdint.h>

#include <blusher/surface.h>

namespace bl {

class TitleBar;

class Window : public Surface
{
public:
    Window();

    uint32_t width() const;
    uint32_t height() const;

    void show();

    void move();

private:
    void update_decoration();
    void update_resize();
    void update_border();
    void update_title_bar();

private:
    uint32_t _width;
    uint32_t _height;

    Surface *_decoration;
    Surface *_resize;
    Surface *_border;
    TitleBar *_title_bar;
    Surface *_body;
};

} // namespace bl

#endif /* _BL_WINDOW_H */
