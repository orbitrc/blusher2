#ifndef _BL_WL_INTERFACE_H
#define _BL_WL_INTERFACE_H

// C++
#include <type_traits>

// Wayland
#include <wayland-client.h>

// Blusher
#include <blusher/wayland/wl-output.h>
#include <blusher/wayland/wl-compositor.h>
#include <blusher/wayland/wl-subcompositor.h>
#include <blusher/wayland/wl-seat.h>
#include <blusher/wayland/xdg-wm-base.h>

namespace bl {

enum class WlInterfaceType {
    Output,
    Compositor,
    Subcompositor,
    Seat,
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
            typename std::conditional<
                type == WlInterfaceType::XdgWmBase,
                XdgWmBase,
                typename std::conditional<
                    type == WlInterfaceType::Subcompositor,
                    WlSubcompositor,
                    typename std::conditional<
                        type == WlInterfaceType::Seat,
                        WlSeat,
                        bool
                    >::type
                >::type
            >::type
        >::type
    >::type;

public:
    WlInterface();

    const struct wl_interface* c_ptr()
    {
        return this->_wl_interface;
    }

private:
    const struct wl_interface *_wl_interface;
    WlInterfaceType _type;
};

using WlOutputInterface = WlInterface<WlInterfaceType::Output>;
using WlCompositorInterface = WlInterface<WlInterfaceType::Compositor>;
using WlSubcompositorInterface = WlInterface<WlInterfaceType::Subcompositor>;
using WlSeatInterface = WlInterface<WlInterfaceType::Seat>;
using XdgWmBaseInterface = WlInterface<WlInterfaceType::XdgWmBase>;

} // namespace bl

#endif // _BL_WL_INTERFACE_H
