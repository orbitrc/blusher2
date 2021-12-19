OBJ = src/svg-core.o

VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
SONAME = libblusher-svg.so.$(VERSION_MAJOR)

C_INCLUDES := -Iinclude/blusher/svg

PKG_CONFIGS = `pkg-config --cflags --libs librsvg-2.0`

default: $(OBJ)
	$(CC) -shared $(CFLAGS) $(C_INCLUDES) $^ -o libblusher-svg.so.$(VERSION) $(PKG_CONFIGS)
	ln -sf libblusher-svg.so.$(VERSION) libblusher-svg.so.$(VERSION_MAJOR).$(VERSION_MINOR)
	ln -sf libblusher-svg.so.$(VERSION_MAJOR).$(VERSION_MINOR) libblusher-svg.so.$(VERSION_MAJOR)
	ln -sf libblusher-svg.so.$(VERSION_MAJOR) libblusher-svg.so

src/%.o: src/%.c
	$(CC) -c $(C_INCLUDES) -fPIC -o $@ $< $(PKG_CONFIGS)

install:
	rm -rf /usr/include/blusher/svg
	mkdir -p /usr/include/blusher/svg
	cp include/blusher/svg/svg-core.h /usr/include/blusher/svg/
	cp libblusher-svg.so.$(VERSION) /usr/lib
	cp -P libblusher-svg.so.$(VERSION_MAJOR).$(VERSION_MINOR) /usr/lib/
	cp -p libblusher-svg.so.$(VERSION_MAJOR) /usr/lib/
	cp -p libblusher-svg.so /usr/lib

clean:
	rm libblusher-svg.so*
	rm src/*.o
