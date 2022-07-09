#include <blusher/pointer-event.h>

#include <stdio.h>

namespace bl {

PointerEvent::PointerEvent(Event::Type type, Button button, double x, double y)
    : Event(type)
{
    this->_button = button;
    this->_x = x;
    this->_y = y;
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

} // namespace bl
