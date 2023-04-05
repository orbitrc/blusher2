#include <blusher/event.h>

namespace bl {

Event::Event(Event::Type type)
{
    this->_type = type;

    this->_propagation = false;
}

Event::Type Event::type() const
{
    return this->_type;
}

void Event::set_type(Event::Type type)
{
    if (this->_type != type) {
        this->_type = type;
    }
}

bool Event::propagation() const
{
    return this->_propagation;
}

void Event::set_propagation(bool val)
{
    this->_propagation = val;
}

} // namespace bl
