#ifndef _BL_XDG_TOPLEVEL_H
#define _BL_XDG_TOPLEVEL_H

// C
#include <stdint.h>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/wayland/wl-seat.h>

struct wl_array;
struct xdg_toplevel;
struct xdg_toplevel_listener;

namespace bl {

class XdgToplevel
{
public:
    enum class ResizeEdge {
        None = 0,
        Top = 1,
        Bottom = 2,
        Left = 4,
        TopLeft = 5,
        BottomLeft = 6,
        Right = 8,
        TopRight = 9,
        BottomRight = 10,
    };

    enum class State {
        Maximized = 1,
        Fullscreen = 2,
        Resizing = 3,
        Activated = 4,
        TiledLeft = 5,      // Since 2
        TiledRight = 6,     // Since 2
        TiledTop = 7,       // Since 2
        TiledBottom = 8,    // Since 2
    };

    class Listener
    {
    public:
        using ConfigureHandler = void (void *data,
            struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
            struct wl_array *states);
        using CloseHandler = void (void *data,
            struct xdg_toplevel *xdg_toplevel);

    public:
        Listener();

        Listener(ConfigureHandler configure, CloseHandler close);

        ~Listener();

        const struct xdg_toplevel_listener* xdg_toplevel_listener() const;

    private:
        struct xdg_toplevel_listener *_xdg_toplevel_listener;
    };

public:
    XdgToplevel(struct xdg_toplevel *xdg_toplevel);

    ~XdgToplevel();

    struct xdg_toplevel* c_ptr();

    void add_listener(const XdgToplevel::Listener& listener,
            void *data = nullptr);

    void move(const WlSeat& seat, uint32_t serial);

    void resize(const WlSeat& seat, uint32_t serial,
            XdgToplevel::ResizeEdge edge);

    void set_maximized();

    void unset_maximized();

    /// wl_array states to Vector. ConfigureHandler's last argument.
    static pr::Vector<XdgToplevel::State> states_to_vector(
            struct wl_array *states);

private:
    struct xdg_toplevel *_xdg_toplevel;
    XdgToplevel::Listener _listener;
};

} // namespace bl

#endif // _BL_XDG_TOPLEVEL_H
