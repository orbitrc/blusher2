#include <blusher/wayland/wl-interface.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

template<>
WlInterface<WlInterfaceType::Output>::WlInterface()
{
    this->_wl_interface = &wl_output_interface;
}

template<>
WlInterface<WlInterfaceType::Compositor>::WlInterface()
{
    this->_wl_interface = &wl_compositor_interface;
}

template<>
WlInterface<WlInterfaceType::XdgWmBase>::WlInterface()
{
    this->_wl_interface = &xdg_wm_base_interface;
}

} // namespace bl
