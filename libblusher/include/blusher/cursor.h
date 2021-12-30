#ifndef _BLUSHER_CURSOR_H
#define _BLUSHER_CURSOR_H

typedef enum bl_cursor_shape {
    /// \brief Default arrow cursor. Same as "arrow".
    BL_CURSOR_SHAPE_ARROW = 0,
    /// \brief Arrow with context menu. Same as "context_menu".
    BL_CURSOR_SHAPE_CONTEXT_MENU = 1,
    /// \brief Arrow with plus sign. Same as "copy".
    BL_CURSOR_SHAPE_COPY = 2,
    /// \brief Arrow with crossed circle. Same as "forbidden".
    BL_CURSOR_SHAPE_FORBIDDEN = 3,
    /// \brief Arrow with question mark. Same as "help".
    BL_CURSOR_SHAPE_HELP = 4,
    /// \brief Closed hand shape. Same as "closedhand".
    BL_CURSOR_SHAPE_CLOSED_HAND = 5,
    /// \brief Open hand shape. Same as "openhand".
    BL_CURSOR_SHAPE_OPEN_HAND = 6,
    /// \brief Hand with pointing finger. Same as "pointer".
    BL_CURSOR_SHAPE_POINTER= 7,
    /// \brief Red circle with slash. Same as "crossed_circle".
    BL_CURSOR_SHAPE_CROSSED_CIRCLE = 8,
    /// \brief Top, right, bottom, left arrow. Same as "size_all".
    BL_CURSOR_SHAPE_SIZE_ALL = 9,
    /// \brief Right, left resizing arrow. Same as "size_hor".
    BL_CURSOR_SHAPE_SIZE_HOR = 10,
    /// \brief Top, bottom resizing arrow. Same as "size_ver".
    BL_CURSOR_SHAPE_SIZE_VER = 11,
    /// \brief Top-right, bottom-left resizing arrow. Same as "size_bdiag".
    BL_CURSOR_SHAPE_SIZE_BDIAG = 12,
    /// \brief Top-left, bottom-right resizing arrow. Same as "size_fdiag".
    BL_CURSOR_SHAPE_SIZE_FDIAG = 13,
} bl_cursor_shape;

#endif /* _BLUSHER_CURSOR_H */
