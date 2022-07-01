#include <blusher/event-dispatcher.h>

#include <blusher/application.h>
#include <blusher/pointer-event.h>

namespace bl {

//================
// Event Queue
//================
template <typename E>
using EventQueue = EventDispatcher::EventQueue<E>;

template<>
EventQueue<PointerEvent>::EventQueue()
{
}

template<>
void EventQueue<PointerEvent>::enqueue(EventQueue::TupleType tuple)
{
    this->_vec.push(tuple);
}

template<>
EventQueue<PointerEvent>::TupleType EventQueue<PointerEvent>::dequeue()
{
    auto ret = this->_vec.remove(0);

    return ret;
}

template<>
uint64_t EventQueue<PointerEvent>::length() const
{
    return this->_vec.length();
}

//====================
// Event Dispatcher
//====================

EventDispatcher::EventDispatcher(Application *application)
{
    this->_application = application;
}

EventDispatcher::~EventDispatcher()
{
}

} // namespace bl
