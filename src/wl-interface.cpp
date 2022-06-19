#include <blusher/wayland/wl-interface.h>

// Wayland Protocol
#include <wayland-protocols/stable/xdg-shell.h>

namespace bl {

template<>
WlInterface<WlInterfaceType::Output>::WlInterface()
{
    this->_wl_interface = &wl_output_interface;
    this->_type = WlInterfaceType::Output;
}

template<>
WlInterface<WlInterfaceType::Compositor>::WlInterface()
{
    this->_wl_interface = &wl_compositor_interface;
    this->_type = WlInterfaceType::Compositor;
}

template<>
WlInterface<WlInterfaceType::XdgWmBase>::WlInterface()
{
    this->_wl_interface = &xdg_wm_base_interface;
    this->_type = WlInterfaceType::XdgWmBase;
}

} // namespace bl
