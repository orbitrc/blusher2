#include <blusher/event-dispatcher.h>

// C
#include <stdio.h>

// C++
#include <chrono>

// Unix
#include <unistd.h>

// Blusher
#include <blusher/application.h>
#include <blusher/pointer-event.h>
#include <blusher/update-event.h>
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
                case Event::Type::PointerPress:
                {
                    View *view = std::get<0>(tuple);
                    auto event = std::static_pointer_cast<PointerEvent>(
                        std::get<1>(tuple));
                    view->pointer_press_event(event);
                    // Event propagation.
                    View *v = view->parent();
                    double x = event->x() + view->x();
                    double y = event->y() + view->y();
                    while (v != nullptr) {
                        event->set_x(x);
                        event->set_y(y);
                        if (!event->propagation()) {
                            break;
                        }
                        v->pointer_press_event(event);
                        x = event->x() + v->x();
                        y = event->y() + v->y();
                        v = v->parent();
                    }
                    break;
                }
                case Event::Type::PointerRelease:
                {
                    View *view = std::get<0>(tuple);
                    auto event = std::static_pointer_cast<PointerEvent>(
                        std::get<1>(tuple));
                    view->pointer_release_event(event);
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
                case Event::Type::Update:
                    View *view = std::get<0>(tuple);
                    auto event = std::static_pointer_cast<UpdateEvent>(
                        std::get<1>(tuple));
                    view->update_event(event);
                    break;
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
    if (event->type() == Event::Type::PointerMove) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        std::static_pointer_cast<PointerEvent>(event)->set_time(ms.count());
    }

    EventQueue::TupleType tuple(view, event);
    this->_event_queue.enqueue(tuple);
}

} // namespace bl
