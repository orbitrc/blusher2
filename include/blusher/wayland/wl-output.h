#ifndef _BL_WL_OUTPUT_H
#define _BL_WL_OUTPUT_H

#include <wayland-client.h>

namespace bl {

class WlOutput
{
public:
    WlOutput(struct wl_output *output);

    ~WlOutput();

private:
    struct wl_output *_wl_output;
};

} // namespace bl

#endif // _BL_WL_OUTPUT_H
