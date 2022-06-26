#ifndef _BL_POINTER_EVENT_H
#define _BL_POINTER_EVENT_H

#include <blusher/input.h>

namespace bl {

class PointerEvent
{
public:
    PointerEvent(Button button, double x, double y);
    ~PointerEvent();

    Button button() const;

    double x() const;

    double y() const;

private:
    Button _button;
    double _x;
    double _y;
};

} // namespace bl

#endif /* _BL_POINTER_EVENT_H */
