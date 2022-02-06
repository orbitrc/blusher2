CONFIG += c++14

SOURCES += main.c \
    src/utils.c \
    src/application.c \
    src/surface.c \
    src/window.c \
    src/title-bar.c \
    src/color.c \
    src/label.c \
    src/image.c \
    src/svg.c \
    src/pointer-event.c \
    src/resource.c \
    src/cpp/application.cpp

HEADERS += include/blusher/utils.h \
    include/blusher/blusher.h \
    include/blusher/application.h \
    include/blusher/surface.h \
    include/blusher/window.h \
    include/blusher/title-bar.h \
    include/blusher/color.h \
    include/blusher/cursor.h \
    include/blusher/label.h \
    include/blusher/image.h \
    include/blusher/svg.h \
    include/blusher/pointer-event.h \
    include/blusher/resource.h \
    blusher-collections/include/blusher-collections.h

INCLUDEPATH += wayland-protocols blusher-collections/include \
    include/blusher \
    /usr/include/pango-1.0 \
    /usr/include/glib-2.0 \
    /usr/lib/glib-2.0/include \
    /usr/include/harfbuzz \
    /usr/include/freetype2 \
    /usr/include/libpng16 \
    /usr/include/libmount \
    /usr/include/blkid \
    /usr/include/fribidi \
    /usr/include/cairo \
    /usr/include/pixman-1

DISTFILES += Makefile
