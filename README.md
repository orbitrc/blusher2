Blusher 2.0
===========

A Desktop GUI Toolkit for Linux with Wayland written in Rust.

Introduction
------------

Blusher 2.0 is a new desktop GUI toolkit built on Wayland. An old version is
written in Qt/QML but this project not using Qt.

Blusher 2.0 using Cairo as a 2D graphics engine and Pango for text rendering.

libblusher
----------
Wayland client library is written in C. Blusher depends on Wayland client library, so core logic is written in C. libblusher is a C bridge of Blusher.

First you need to install the `libblusher-collections.so` shared library.

```sh
$ cd libblusher-collections
$ make
$ sudo make install
```

You can build it with command
```sh
$ make
```

and you can run the demo with simply run
```sh
$ make run
```

License
-------
Blusher is developed under MIT License. For the detail, see the LICENSE file.

