default:
	cargo build --release

install:
	rm -rf /usr/include/blusher/core
	mkdir -p /usr/include/blusher/core
	cp include/blusher/core/collections.h /usr/include/blusher/core/collections.h
	cp target/release/libblusher_core.so /usr/lib/libblusher-core.so

