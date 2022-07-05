Blusher 2.0
===========

A Desktop GUI Toolkit for Linux with Wayland written in C++.

Introduction
------------

Blusher 2.0 is a new desktop GUI toolkit built on Wayland. An old version is
written in Qt/QML but this project not using QML. It only using Qt for signal/slot
and event driven programming.

Blusher 2.0 using Cairo as a 2D graphics engine and Pango for text rendering.


Dependencies
------------
Blusher has a dependency on `libblusher-svg`.

Below is the list of all dependencies of those.

- Cairo
- Pango
- librsvg


libblusher-svg
--------------
Rendering SVG is not a simple thing. Currently Blusher using `librsvg` for that.
But it could be changed in the future. So we split SVG library as a dependency.
It called `libblusher-svg`.

First you need to install `libblusher-svg.so` shared libraries.

```sh
$ cd libblusher-svg
$ make
$ sudo make install
```

You can build it with command
```sh
$ make wayland-protocols
$ make resources
$ make
```

Blusher is not...
-----------------
- a cross-platform GUI toolkit. We will support only Linux.
- for X11 systems. It supports only Wayland.
- for custom themes. It has its own design called Hydrogen, inspired by the KDE Oxygen theme.


License
-------
Blusher is developed under MIT License. For the detail, see the LICENSE file.

