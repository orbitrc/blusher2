QMAKE ?= qmake

default:
	mkdir -p build
	cd build && $(QMAKE) ../blusher.pro -spec linux-g++ && make qmake_all
	cd build && make -j8

clean:
	rm -rf build
	rm resources/*.h
	rm resources/*.cpp
	rm resources/*.o
	rm resources/*.a
