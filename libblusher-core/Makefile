default:
	$(CC) -c -fPIC -Iinclude/blusher/core src/log.c -o src/log.o
	ar -rcs liblog.a src/log.o
	cargo build --release

install:
	rm -rf /usr/include/blusher/core
	mkdir -p /usr/include/blusher/core
	cp include/blusher/core/collections.h /usr/include/blusher/core/collections.h
	cp include/blusher/core/log.h /usr/include/blusher/core/log.h
	cp target/release/libblusher_core.so /usr/lib/libblusher-core.so

clean:
	rm src/*.o
	rm liblog.a
	cargo clean
