#ifndef _BL_WL_SUBCOMPOSITOR_H
#define _BL_WL_SUBCOMPOSITOR_H

#include <memory>

#include <wayland-client.h>

#include <blusher/wayland/wl-surface.h>
#include <blusher/wayland/wl-subsurface.h>

namespace bl {

class WlSubcompositor
{
public:
    WlSubcompositor(struct wl_subcompositor *subcompositor);

    ~WlSubcompositor();

    struct wl_subcompositor* c_ptr();

    std::shared_ptr<WlSubsurface> get_subsurface(const WlSurface& surface,
            const WlSurface& parent);

private:
    struct wl_subcompositor *_wl_subcompositor;
};

} // namespace bl

#endif // _BL_WL_SUBCOMPOSITOR_H
