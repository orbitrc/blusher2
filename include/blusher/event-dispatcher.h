#ifndef _BL_EVENT_DISPATCHER_H
#define _BL_EVENT_DISPATCHER_H

// C++
#include <tuple>
#include <memory>

// Primer
#include <primer/vector.h>

namespace bl {

class Application;

class View;

class EventDispatcher
{
public:
    template <typename E>
    class EventQueue
    {
    public:
        using TupleType = std::tuple<View*, std::shared_ptr<E>>;

    public:
        EventQueue<E>();

        void enqueue(TupleType tuple);

        TupleType dequeue();

        uint64_t length() const;

    private:
        pr::Vector<TupleType> _vec;
    };

public:
    EventDispatcher(Application *application);
    ~EventDispatcher();

private:
    Application *_application;
};

} // namespace bl

#endif // _BL_EVENT_DISPATCHER_H
