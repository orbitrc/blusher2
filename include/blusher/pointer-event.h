#ifndef _BL_POINTER_EVENT_H
#define _BL_POINTER_EVENT_H

// C
#include <stdint.h>

// Blusher
#include <blusher/event.h>
#include <blusher/input.h>

namespace bl {

class PointerEvent : public Event
{
public:
    PointerEvent(Event::Type type, PointerButton button, double x, double y);
    ~PointerEvent();

    PointerButton button() const;

    double x() const;
    void set_x(double x);

    double y() const;
    void set_y(double y);

    uint64_t time() const;
    void set_time(uint64_t time);

private:
    PointerButton _button;
    double _x;
    double _y;
    uint64_t _time;
};

} // namespace bl

#endif /* _BL_POINTER_EVENT_H */
