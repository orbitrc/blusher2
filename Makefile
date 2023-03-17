QMAKE ?= qmake

WAYLAND_PROTOCOLS_STABLE_DIR=/usr/share/wayland-protocols/stable

BLUSHER_RESOURCES_A=resources/libresources.a

default:
	mkdir -p build
	cd build && $(QMAKE) ../blusher.pro -spec linux-g++ && make qmake_all
	cd build && make -j8

wayland-protocols: wayland-protocols/stable/xdg-shell.h wayland-protocols/stable/xdg-shell.c
	ls

wayland-protocols/stable/xdg-shell.h:
	wayland-scanner client-header $(WAYLAND_PROTOCOLS_STABLE_DIR)/xdg-shell/xdg-shell.xml wayland-protocols/stable/xdg-shell.h

wayland-protocols/stable/xdg-shell.c:
	wayland-scanner public-code $(WAYLAND_PROTOCOLS_STABLE_DIR)/xdg-shell/xdg-shell.xml wayland-protocols/stable/xdg-shell.c

resources:
	bin/blusher rcc resources/resources.brc

clean:
	rm -rf build
	rm resources/*.h
	rm resources/*.cpp
	rm resources/*.o
	rm resources/*.a

.PHONY: resources
