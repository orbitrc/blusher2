#include <blusher/pointer-event.h>

#include <stdio.h>

namespace bl {

PointerEvent::PointerEvent(Event::Type type, PointerButton button, double x, double y)
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

PointerButton PointerEvent::button() const
{
    return this->_button;
}

double PointerEvent::x() const
{
    return this->_x;
}

void PointerEvent::set_x(double x)
{
    this->_x = x;
}

double PointerEvent::y() const
{
    return this->_y;
}

void PointerEvent::set_y(double y)
{
    this->_y = y;
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
