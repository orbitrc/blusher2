#ifndef _BL_CURSOR_H
#define _BL_CURSOR_H

namespace bl {

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

private:
};

} // namespace bl

#endif // _BL_CURSOR_H
