#include <blusher/window.h>

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
    this->_width = 200;
    this->_height = 200;

    // Load resources.
    register_resources();

    // Init body.
    this->_body = static_cast<Surface*>(this);

    this->_body->set_geometry(0, 0, this->_width, this->_height);

    this->_title_bar = new TitleBar(this->_body);

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

uint32_t Window::width() const
{
    return this->_width;
}

uint32_t Window::height() const
{
    return this->_height;
}

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

//====================
// Private Methods
//====================

void Window::update_decoration()
{
    this->_decoration->set_geometry(
        0 - BLUSHER_SHADOW_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_SHADOW_WIDTH,
        this->_width + (BLUSHER_SHADOW_WIDTH * 2),
        this->_height + (BLUSHER_SHADOW_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_decoration->set_color(Color::from_rgba(0, 0, 0, 100));
}

void Window::update_resize()
{
    this->_resize->set_geometry(
        0 - BLUSHER_RESIZE_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_RESIZE_WIDTH,
        this->_width + (BLUSHER_RESIZE_WIDTH * 2),
        this->_height + (BLUSHER_RESIZE_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_resize->set_color(Color::from_rgba(255, 0, 0, 100));
}

void Window::update_border()
{
    this->_border->set_geometry(
        0 - BLUSHER_BORDER_WIDTH,
        0 - BLUSHER_TITLE_BAR_HEIGHT - BLUSHER_BORDER_WIDTH,
        this->_width + (BLUSHER_BORDER_WIDTH * 2),
        this->_height + (BLUSHER_BORDER_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_border->set_color(Color::from_rgb(0, 0, 0));
}

void Window::update_title_bar()
{
    this->_title_bar->set_geometry(
        0,
        -BLUSHER_TITLE_BAR_HEIGHT,
        this->_width,
        BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_title_bar->set_color(Color::from_rgb(100, 100, 100));
}

//===============
// Resize
//===============

Resize::Resize(Surface *parent)
    : Surface(parent)
{
}

void Resize::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    if (event->button() == PointerEvent::Button::Left) {
        for (Surface *it = this; it->parent() != nullptr; it = it->parent()) {
            Surface *win = it->parent();
            if (win->parent() == nullptr) {
                win->resize_if_window();
            }
        }
    }

    return Surface::pointer_press_event(event);
}

} // namespace bl
