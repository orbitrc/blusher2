#ifndef _BL_RESIZE_EVENT_H
#define _BL_RESIZE_EVENT_H

#include <blusher/size.h>

namespace bl {

class ResizeEvent
{
public:
    ResizeEvent(const Size& size, const Size& old_size);

private:
    Size _size;
};

} // namespace bl

#endif // _BL_RESIZE_EVENT_H
