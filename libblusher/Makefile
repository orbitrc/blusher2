WAYLAND_PROTOCOLS_UNSTABLE_DIR=/usr/share/wayland-protocols/unstable

WAYLAND_PROTOCOLS_STABLE_DIR=/usr/share/wayland-protocols/stable

XDG_SHELL_HEADER=xdg-shell.h
XDG_SHELL_SOURCE=xdg-shell.c
XDG_SHELL_HEADER_PATH=include/blusher/wayland-protocols/stable/$(XDG_SHELL_HEADER)
XDG_SHELL_SOURCE_PATH=src/wayland-protocols/stable/$(XDG_SHELL_SOURCE)
XDG_SHELL_OBJECT_PATH=src/wayland-protocols/stable/xdg-shell.o

BLUSHER_COLLECTIONS_LIB=libblusher-core.so

BLUSHER_RESOURCES=resources/libresources.a

CFLAGS += -lblusher-core -lblusher-svg

C_INCLUDES := -Iinclude/blusher

PKG_CONFIGS = `pkg-config --cflags --libs cairo` `pkg-config --cflags --libs pango` `pkg-config --cflags --libs pangocairo`

OBJ = src/application.o \
	src/window.o \
	src/title-bar.o \
	src/surface.o \
	src/utils.o \
	src/color.o \
	src/label.o \
	src/image.o \
	src/resource.o \
	src/svg.o \
	src/pointer-event.o \
	src/cpp/application.o \
	$(XDG_SHELL_OBJECT_PATH)

VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
SONAME = libblusher.so.$(VERSION_MAJOR)

default: $(OBJ) $(XDG_SHELL_HEADER_PATH) $(BLUSHER_RESOURCES)
	$(CXX) -shared $(C_INCLUDES) $(CFLAGS) $^ -o libblusher.so.$(VERSION) -lwayland-client -lwayland-egl -lwayland-cursor $(PKG_CONFIGS)
	ln -sf libblusher.so.$(VERSION) libblusher.so.$(VERSION_MAJOR).$(VERSION_MINOR)
	ln -sf libblusher.so.$(VERSION_MAJOR).$(VERSION_MINOR) libblusher.so.$(VERSION_MAJOR)
	ln -sf libblusher.so.$(VERSION_MAJOR) libblusher.so

src/%.o: src/%.c
	$(CC) -c $(C_INCLUDES) $(CFLAGS) $(PKG_CONFIGS) -fPIC -o $@ $<

src/cpp/%.o: src/cpp/%.cpp
	$(CXX) -c $(C_INCLUDES) $(CFLAGS) $(PKG_CONFIGS) -fPIC -o $@ $<

wayland-protocols: $(XDG_SHELL_HEADER_PATH) $(XDG_SHELL_SOURCE_PATH)
	ls

$(XDG_SHELL_HEADER_PATH):
	wayland-scanner client-header $(WAYLAND_PROTOCOLS_STABLE_DIR)/xdg-shell/xdg-shell.xml include/blusher/wayland-protocols/stable/$(XDG_SHELL_HEADER)

$(XDG_SHELL_SOURCE_PATH):
	wayland-scanner public-code $(WAYLAND_PROTOCOLS_STABLE_DIR)/xdg-shell/xdg-shell.xml src/wayland-protocols/stable/$(XDG_SHELL_SOURCE)

$(XDG_SHELL_OBJECT_PATH): $(XDG_SHELL_SOURCE_PATH)
	$(CC) -c -fPIC $(XDG_SHELL_SOURCE_PATH) -o src/wayland-protocols/stable/xdg-shell.o

$(BLUSHER_COLLECTIONS_LIB):
	cd blusher-collections ; cargo build --release

resources: $(BLUSHER_RESOURCES)

$(BLUSHER_RESOURCES):
	../bin/blusher rcc resources/resources.brc

install:
	rm -f /usr/include/blusher/*.h
	cp include/blusher/*.h /usr/include/blusher
	cp libblusher.so.$(VERSION) /usr/lib/
	cp -P libblusher.so.$(VERSION_MAJOR).$(VERSION_MINOR) /usr/lib/
	cp -P libblusher.so.$(VERSION_MAJOR) /usr/lib/
	cp -P libblusher.so /usr/lib/

clean:
	rm libblusher.so*
	rm src/*.o
	rm src/wayland-protocols/stable/*.o
	rm resources/*.h
	rm resources/*.c
	rm resources/*.o
	rm resources/*.a

run:
	$(CC) -I./include main.c -L. -lblusher -lwayland-client -lblusher-core
	LD_LIBRARY_PATH=./: ./a.out
