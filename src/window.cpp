#include <blusher/window.h>

#include <blusher/title-bar.h>

#define BLUSHER_SHADOW_WIDTH 40
#define BLUSHER_RESIZE_WIDTH 5
#define BLUSHER_BORDER_WIDTH 1
#define BLUSHER_TITLE_BAR_HEIGHT 30

namespace bl {

Window::Window()
    : Surface(nullptr)
{
    this->_width = 200;
    this->_height = 200;

    this->_decoration = static_cast<Surface*>(this);
    this->_resize = new Surface(this->_decoration);
    this->_border = new Surface(this->_resize);
    this->_title_bar = new TitleBar(this->_border);

    // Init decoration.
    this->update_decoration();

    // Init resize.
    this->update_resize();

    // Init border.
    this->update_border();

    // Init title bar.
    this->update_title_bar();
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
    this->_decoration->show();
    this->_decoration->paint();

    this->_resize->show();
    this->_resize->paint();

    this->_border->show();
    this->_border->paint();

    this->_title_bar->show();
    this->_title_bar->paint();
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
        0,
        0,
        this->_width + (BLUSHER_SHADOW_WIDTH * 2),
        this->_height + (BLUSHER_SHADOW_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_decoration->set_color(Color::from_rgba(0, 0, 0, 100));
}

void Window::update_resize()
{
    this->_resize->set_geometry(
        BLUSHER_SHADOW_WIDTH - BLUSHER_RESIZE_WIDTH,
        BLUSHER_SHADOW_WIDTH - BLUSHER_RESIZE_WIDTH,
        this->_width + (BLUSHER_RESIZE_WIDTH * 2),
        this->_height + (BLUSHER_RESIZE_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_resize->set_color(Color::from_rgba(255, 0, 0, 100));
}

void Window::update_border()
{
    this->_border->set_geometry(
        BLUSHER_RESIZE_WIDTH - BLUSHER_BORDER_WIDTH,
        BLUSHER_RESIZE_WIDTH - BLUSHER_BORDER_WIDTH,
        this->_width + (BLUSHER_BORDER_WIDTH * 2),
        this->_height + (BLUSHER_BORDER_WIDTH * 2) + BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_border->set_color(Color::from_rgb(0, 0, 0));
}

void Window::update_title_bar()
{
    this->_title_bar->set_geometry(
        BLUSHER_BORDER_WIDTH,
        BLUSHER_BORDER_WIDTH,
        this->_width,
        BLUSHER_TITLE_BAR_HEIGHT
    );
    this->_title_bar->set_color(Color::from_rgb(100, 100, 100));
}

} // namespace bl
