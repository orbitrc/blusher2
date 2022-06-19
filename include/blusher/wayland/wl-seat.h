#ifndef _BL_WL_SEAT_H
#define _BL_WL_SEAT_H

#include <wayland-client.h>

namespace bl {

class WlSeat
{
public:
    WlSeat(struct wl_seat *wl_seat);

    ~WlSeat();

    struct wl_seat* wl_seat();

private:
    struct wl_seat *_wl_seat;
};

} // namespace bl

#endif // _BL_WL_SEAT_H
