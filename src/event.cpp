#include <blusher/event.h>

namespace bl {

Event::Event(Event::Type type)
{
    this->_type = type;
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

} // namespace bl
