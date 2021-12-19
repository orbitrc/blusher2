INCLUDEPATH += include/blusher/svg \
    /usr/include/librsvg-2.0 \
    /usr/include/glib-2.0 \
    /usr/lib/glib-2.0/include \
    /usr/include/libmount \
    /usr/include/blkid \
    /usr/include/gdk-pixbuf-2.0 \
    /usr/include/libpng16 \
    /usr/include/cairo \
    /usr/include/pixman-1 \
    /usr/include/freetype2 \
    /usr/include/harfbuzz

SOURCES += src/svg-core.c

HEADERS += include/blusher/svg/svg-core.h
