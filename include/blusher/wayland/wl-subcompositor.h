#ifndef _BL_WL_SUBCOMPOSITOR_H
#define _BL_WL_SUBCOMPOSITOR_H

#include <wayland-client.h>

namespace bl {

class WlSubcompositor
{
public:
    WlSubcompositor(struct wl_subcompositor *subcompositor);

    ~WlSubcompositor();

    struct wl_subcompositor* wl_subcompositor();

private:
    struct wl_subcompositor *_wl_subcompositor;
};

} // namespace bl

#endif // _BL_WL_SUBCOMPOSITOR_H
