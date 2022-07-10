#include <blusher/event-dispatcher.h>

// C
#include <stdio.h>

// Unix
#include <unistd.h>

// Blusher
#include <blusher/application.h>
#include <blusher/pointer-event.h>
#include <blusher/view.h>

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
    // this->_thread = std::thread(&EventDispatcher::loop, this);
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::loop()
{
    while (this->_running) {
        if (this->_event_queue.length() != 0) {
            while (this->_event_queue.length() > 0) {
                // fprintf(stderr, " == Event loop before  - length: %ld\n", this->_event_queue.length());
                auto tuple = this->_event_queue.dequeue();
                // fprintf(stderr, " == Event loop dequeue - length: %ld\n", this->_event_queue.length());
                switch (std::get<1>(tuple)->type()) {
                case Event::Type::PointerEnter:
                {
                    View *view = std::get<0>(tuple);
                    std::shared_ptr<PointerEvent> event =
                        std::static_pointer_cast<PointerEvent>(
                            std::get<1>(tuple)
                        );
                    // fprintf(stderr, " == View: %p\n", view);
                    view->pointer_enter_event(event);
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    View *view = std::get<0>(tuple);
                    auto event = std::static_pointer_cast<PointerEvent>(
                        std::get<1>(tuple));
                    view->pointer_leave_event(event);
                    break;
                }
                case Event::Type::PointerMove:
                {
                    View *view = std::get<0>(tuple);
                    auto event = std::static_pointer_cast<PointerEvent>(
                        std::get<1>(tuple));
                    view->pointer_move_event(event);
                    break;
                }
                }
            }
        }
        usleep(10);
    }
}

void EventDispatcher::start_loop()
{
    this->_running = true;
    this->_thread = std::thread(&EventDispatcher::loop, this);
    this->_thread.detach();
}

void EventDispatcher::stop_loop()
{
    this->_running = false;
}

void EventDispatcher::post_event(View *view, std::shared_ptr<Event> event)
{
    EventQueue::TupleType tuple(view, event);
    this->_event_queue.enqueue(tuple);
}

} // namespace bl
