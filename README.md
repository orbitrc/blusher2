Blusher 2.0
===========

A Desktop GUI Toolkit for Linux with Wayland written in Rust.

Introduction
------------

Blusher 2.0 is a new desktop GUI toolkit built on Wayland. An old version is
written in Qt/QML but this project not using Qt.

Blusher 2.0 using Cairo as a 2D graphics engine and Pango for text rendering.


Dependencies
------------
Blusher has a dependency on `libblusher`. And it depends on `blusher-core` and `blusher-svg`.

Below is the list of all dependencies of those.

- Cairo
- Pango
- librsvg


libblusher
----------
Wayland client library is written in C. Blusher depends on Wayland client library, so core logic is written in C. libblusher is a C bridge of Blusher.

First you need to install `libblusher-core.so` and `libblusher-svg.so` shared libraries.

```sh
$ cd libblusher-core
$ make
$ sudo make install
```

```sh
$ cd libblusher-svg
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

Blusher is not...
-----------------
- a cross-platform GUI toolkit. We will support only Linux.
- for X11 systems. It supports only Wayland.
- for custom themes. It has its own design called Hydrogen, inspired by the KDE Oxygen theme.


License
-------
Blusher is developed under MIT License. For the detail, see the LICENSE file.

