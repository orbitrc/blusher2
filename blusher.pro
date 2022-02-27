QT += gui

TEMPLATE = lib
DEFINES += BLUSHERQT_LIBRARY

CONFIG += c++14

INCLUDEPATH += ./include

TARGET = blusher

VERSION = 2.0.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/application.cpp \
    src/surface.cpp \
    src/window.cpp \
    src/title-bar.cpp \
    src/signal.cpp \
    src/color.cpp \
    src/resource.cpp \
    src/pointer-event.cpp \
    src/application-impl.cpp \
    src/signal-impl.cpp \
    src/surface-impl.cpp

HEADERS += \
    include/blusher/application.h \
    include/blusher/surface.h \
    include/blusher/window.h \
    include/blusher/title-bar.h \
    include/blusher/signal.h \
    include/blusher/color.h \
    include/blusher/resource.h \
    include/blusher/pointer-event.h \
    src/application-impl.h \
    src/signal-impl.h \
    src/surface-impl.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
