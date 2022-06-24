#ifndef _BL_CURSOR_H
#define _BL_CURSOR_H

// C
#include <stdint.h>

// Wayland
#include <wayland-client.h>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/image.h>

namespace bl {

class CursorImpl;

class Cursor
{
public:
    enum class Shape {
        /// Default arrow cursor.
        Arrow,
        /// Copy arrow. Plus sign next to the arrow.
        Copy,
        /// Context menu arrow. Menu icon next to the arrow.
        ContextMenu,
        /// Cross cursor.
        Cross,
        /// Red circle with diagonal in the circle.
        CrossedCircle,
        /// Arrow with CrossedCircle.
        Forbidden,
        /// Question mark in a circle next to the arrow.
        Help,
        /// Curved arrow in a circle next to the arrow.
        Link,
        /// Four arrows for top, left, right and bottom with a dot in center.
        SizeAll,
        /// Two arrows for top-right and bottom-left with a dot in center.
        SizeBdiag,
        /// Two arrows for top-left and bottom-right with a dot in center.
        SizeFdiag,
        /// Two arrows for left and right with a dot in center.
        /// Usually used in resize window horizontally.
        SizeHor,
        /// Two arrows for top and bottom with a dot in center.
        /// Usually used in resize window vertically.
        SizeVer,
        /// Two arrows for left and right with a vertical bar.
        SplitHor,
        /// Tow arrows for top and bottom with a horizontal bar.
        SplitVer,
    };

public:
    Cursor(Cursor::Shape shape);

    ~Cursor();

    Cursor::Shape shape() const;

    uint32_t hot_spot_x() const;

    uint32_t hot_spot_y() const;

    struct wl_surface* wl_surface();

private:
    Cursor::Shape _shape;
    uint32_t _hot_spot_x;
    uint32_t _hot_spot_y;

    pr::Vector<Image> _images;

    CursorImpl *_impl;
};

} // namespace bl

#endif // _BL_CURSOR_H
