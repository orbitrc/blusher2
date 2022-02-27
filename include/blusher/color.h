#ifndef _BL_COLOR_H
#define _BL_COLOR_H

#include <stdint.h>

namespace bl {

class Color
{
public:
    Color();

public:
    /// Construct bl::Color from red, green and blue.
    static Color from_rgb(uint32_t r, uint32_t g, uint32_t b);

    /// Construct bl::Color from red, green and blue with alpha.
    static Color from_rgba(uint32_t r, uint32_t g, uint32_t b, uint32_t a);

    uint32_t red() const;
    uint32_t green() const;
    uint32_t blue() const;
    uint32_t alpha() const;

    uint32_t to_argb() const;

private:
    uint32_t _r;
    uint32_t _g;
    uint32_t _b;
    uint32_t _a;
};

} // namespace bl

#endif /* _BL_COLOR_H */
