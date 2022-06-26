QT += gui

TEMPLATE = lib

CONFIG += c++17

INCLUDEPATH += ../../include

LIBS += -lwayland-client -lwayland-egl -lEGL -lprimer -lblusher-svg \
    ../resources/libresources.a

TARGET = blusher

PKGCONFIG += cairo pango

VERSION = 0.1.0

SOURCES += \
    src/sample-view.cpp \
    src/main.cpp

HEADERS += \
    src/sample-view.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
