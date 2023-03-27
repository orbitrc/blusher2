#include <blusher/wayland/wl-compositor.h>

// C
#include <assert.h>

namespace bl {

WlCompositor *wl_compositor_singleton = nullptr;

WlCompositor::WlCompositor()
{
    this->_wl_compositor = nullptr;
}

WlCompositor::WlCompositor(struct wl_compositor *compositor)
{
    assert(wl_compositor_singleton == nullptr);

    this->_wl_compositor = compositor;

    wl_compositor_singleton = this;
}

WlCompositor::~WlCompositor()
{
}

struct wl_compositor* WlCompositor::c_ptr()
{
    return this->_wl_compositor;
}

std::shared_ptr<WlSurface> WlCompositor::create_surface()
{
    struct wl_surface *wl_surface =
        wl_compositor_create_surface(this->_wl_compositor);
    assert(wl_surface != NULL);

    auto surface = std::make_shared<WlSurface>(wl_surface);

    return surface;
}

WlCompositor* WlCompositor::instance()
{
    return wl_compositor_singleton;
}

WlCompositor& WlCompositor::operator=(const WlCompositor& other)
{
    this->_wl_compositor = other._wl_compositor;

    if (wl_compositor_singleton != nullptr) {
        wl_compositor_singleton = this;
    }

    return *this;
}

} // namespace bl
