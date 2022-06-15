#include <blusher/wl-interface.h>

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

} // namespace bl
