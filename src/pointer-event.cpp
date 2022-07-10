#include <blusher/pointer-event.h>

#include <stdio.h>

namespace bl {

PointerEvent::PointerEvent(Event::Type type, Button button, double x, double y)
    : Event(type)
{
    this->_button = button;
    this->_x = x;
    this->_y = y;
    this->_time = 0;
}

PointerEvent::~PointerEvent()
{
//    fprintf(stderr, "PointerEvent::~PointerEvent()\n");
}

//=================
// Public Methods
//=================

Button PointerEvent::button() const
{
    return this->_button;
}

double PointerEvent::x() const
{
    return this->_x;
}

double PointerEvent::y() const
{
    return this->_y;
}

uint64_t PointerEvent::time() const
{
    return this->_time;
}

void PointerEvent::set_time(uint64_t time)
{
    this->_time = time;
}

} // namespace bl
