#include <blusher/color.h>

namespace bl {

Color::Color()
{
}

Color Color::from_rgb(uint32_t r, uint32_t g, uint32_t b)
{
    Color color;
    color._r = r;
    color._g = g;
    color._b = b;
    color._a = 255;

    return color;
}

Color Color::from_rgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    Color color;
    color._r = r;
    color._g = g;
    color._b = b;
    color._a = a;

    return color;
}

//=================
// Public Methods
//=================

uint32_t Color::red() const
{
    return this->_r;
}

uint32_t Color::green() const
{
    return this->_g;
}

uint32_t Color::blue() const
{
    return this->_b;
}

uint32_t Color::alpha() const
{
    return this->_a;
}

double Color::red_f() const
{
    return this->_r / 255.0;
}

double Color::green_f() const
{
    return this->_g / 255.0;
}

double Color::blue_f() const
{
    return this->_b / 255.0;
}

double Color::alpha_f() const
{
    return this->_a / 255.0;
}

uint32_t Color::to_argb() const
{
    uint32_t ret = 0x00000000;

    ret += (this->_r) << 16;
    ret += (this->_g) << 8;
    ret += (this->_b) << 0;
    ret += (this->_a) << 24;

    return ret;
}

} // namespace bl
