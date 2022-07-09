#ifndef _BL_EVENT_DISPATCHER_H
#define _BL_EVENT_DISPATCHER_H

// C++
#include <tuple>
#include <memory>
#include <thread>

// Primer
#include <primer/vector.h>

// Blusher
#include <blusher/event.h>

namespace bl {

class Application;

class View;

class EventDispatcher
{
public:
    class EventQueue
    {
    public:
        using TupleType = std::tuple<View*, std::shared_ptr<Event>>;

    public:
        EventQueue();

        void enqueue(TupleType tuple);

        TupleType dequeue();

        uint64_t length() const;

    private:
        pr::Vector<TupleType> _vec;
    };

public:
    EventDispatcher(Application *application);
    ~EventDispatcher();

    void loop();

    void start_loop();

    void stop_loop();

private:
    Application *_application;

    EventQueue _event_queue;
    std::thread _thread;
    bool _running;
};

} // namespace bl

#endif // _BL_EVENT_DISPATCHER_H
