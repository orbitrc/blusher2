#ifndef _BL_SVG_H
#define _BL_SVG_H

// C
#include <stdint.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/image.h>

typedef struct bl_svg_core bl_svg_core;

namespace bl {

class Svg
{
public:
    Svg(const pr::String& path);

    Image to_image(uint64_t width, uint64_t height) const;

private:
    bl_svg_core *_core;
};

} // namespace bl

#endif // _BL_SVG_H
