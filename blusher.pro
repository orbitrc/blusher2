QT += core

TEMPLATE = lib
DEFINES += BLUSHERQT_LIBRARY

CONFIG += c++17
CONFIG += link_pkgconfig

INCLUDEPATH += ./include

LIBS += -lwayland-client -lwayland-egl -lEGL -lGL -lGLEW \
    -lprimer \
    -lblusher-svg \
    ../resources/libresources.a

TARGET = blusher

PKGCONFIG += cairo pango

VERSION = 2.0.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/application.cpp \
    src/event-dispatcher.cpp \
    src/surface.cpp \
    src/desktop-surface.cpp \
    src/window.cpp \
    src/title-bar.cpp \
    src/view.cpp \
    src/signal.cpp \
    src/point.cpp \
    src/size.cpp \
    src/rect.cpp \
    src/color.cpp \
    src/image.cpp \
    src/svg.cpp \
    src/cursor.cpp \
    src/label.cpp \
    src/resource.cpp \
    src/event.cpp \
    src/pointer-event.cpp \
    src/resize-event.cpp \
    src/update-event.cpp \
    src/output.cpp \
    src/pointer-state.cpp \
    src/application-impl.cpp \
    src/signal-impl.cpp \
    src/surface-impl.cpp \
    src/view-impl.cpp \
    src/utils.cpp \
    src/egl-utils.cpp \
    src/wl-display.cpp \
    src/wl-registry.cpp \
    src/wl-interface.cpp \
    src/wl-compositor.cpp \
    src/wl-subcompositor.cpp \
    src/wl-surface.cpp \
    src/wl-subsurface.cpp \
    src/wl-output.cpp \
    src/wl-seat.cpp \
    src/wl-pointer.cpp \
    src/wl-keyboard.cpp \
    src/xdg-wm-base.cpp \
    src/xdg-surface.cpp \
    src/xdg-toplevel.cpp \
    src/context.cpp \
    src/shader.cpp \
    src/program.cpp \
    src/button.cpp \
    src/xkb/xkb_context.cpp \
    src/xkb/xkb_keymap.cpp \
    src/xkb/xkb_state.cpp \
    wayland-protocols/stable/xdg-shell.c

HEADERS += \
    include/blusher/application.h \
    include/blusher/event-dispatcher.h \
    include/blusher/surface.h \
    include/blusher/desktop-surface.h \
    include/blusher/window.h \
    include/blusher/title-bar.h \
    include/blusher/view.h \
    include/blusher/signal.h \
    include/blusher/point.h \
    include/blusher/size.h \
    include/blusher/rect.h \
    include/blusher/color.h \
    include/blusher/image.h \
    include/blusher/svg.h \
    include/blusher/cursor.h \
    include/blusher/label.h \
    include/blusher/resource.h \
    include/blusher/input.h \
    include/blusher/text.h \
    include/blusher/event.h \
    include/blusher/pointer-event.h \
    include/blusher/resize-event.h \
    include/blusher/update-event.h \
    include/blusher/output.h \
    include/blusher/pointer-state.h \
    src/application-impl.h \
    src/signal-impl.h \
    src/surface-impl.h \
    src/view-impl.h \
    include/blusher/wayland/wl-display.h \
    include/blusher/wayland/wl-registry.h \
    include/blusher/wayland/wl-interface.h \
    include/blusher/wayland/wl-compositor.h \
    include/blusher/wayland/wl-subcompositor.h \
    include/blusher/wayland/wl-surface.h \
    include/blusher/wayland/wl-subsurface.h \
    include/blusher/wayland/wl-output.h \
    include/blusher/wayland/wl-seat.h \
    include/blusher/wayland/wl-pointer.h \
    include/blusher/wayland/wl-keyboard.h \
    include/blusher/wayland/xdg-wm-base.h \
    include/blusher/wayland/xdg-surface.h \
    include/blusher/wayland/xdg-toplevel.h \
    include/blusher/utils.h \
    include/blusher/gl/context.h \
    include/blusher/gl/shader.h \
    include/blusher/gl/program.h \
    include/blusher/controls/button.h \
    include/blusher/xkb/context.h \
    include/blusher/xkb/keymap.h \
    include/blusher/xkb/state.h \
    src/egl-utils.h

# Examples
DISTFILES += examples/wl-show-surface/main.cpp \
    examples/window/main.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
