#ifndef _BL_WL_INTERFACE_H
#define _BL_WL_INTERFACE_H

// C++
#include <type_traits>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/wl-compositor.h>

namespace bl {

enum class WlInterfaceType {
    Output,
    Compositor,
    Subcompositor,
    XdgWmBase,
};

template <WlInterfaceType type>
class WlInterface
{
public:
    using BindType = typename std::conditional<
        type == WlInterfaceType::Output,
        WlOutput,
        typename std::conditional<
            type == WlInterfaceType::Compositor,
            WlCompositor,
            bool
        >::type
    >::type;

public:
    WlInterface();

    const struct wl_interface* wl_interface()
    {
        return this->_wl_interface;
    }

private:
    const struct wl_interface *_wl_interface;
    WlInterfaceType _type;
};

} // namespace bl

#endif // _BL_WL_INTERFACE_H
