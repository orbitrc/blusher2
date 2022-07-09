#include <blusher/event-dispatcher.h>

// Unix
#include <unistd.h>

// Blusher
#include <blusher/application.h>
#include <blusher/pointer-event.h>

namespace bl {

//================
// Event Queue
//================
using EventQueue = EventDispatcher::EventQueue;

EventQueue::EventQueue()
{
}

void EventQueue::enqueue(EventQueue::TupleType tuple)
{
    this->_vec.push(tuple);
}

EventQueue::TupleType EventQueue::dequeue()
{
    auto ret = this->_vec.remove(0);

    return ret;
}

uint64_t EventQueue::length() const
{
    return this->_vec.length();
}

//====================
// Event Dispatcher
//====================

EventDispatcher::EventDispatcher(Application *application)
{
    this->_application = application;
    this->_running = false;
    this->_thread = std::thread(&EventDispatcher::loop, this);
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::loop()
{
    while (this->_running) {
        usleep(10);
    }
}

void EventDispatcher::start_loop()
{
    this->_running = true;
    this->_thread.detach();
}

void EventDispatcher::stop_loop()
{
    this->_running = false;
}

} // namespace bl
