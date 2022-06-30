#ifndef _BL_EVENT_DISPATCHER_H
#define _BL_EVENT_DISPATCHER_H

namespace bl {

class Application;

class EventDispatcher
{
public:
    class EventQueue
    {
    public:
    private:
    };

public:
    EventDispatcher(Application *application);

private:
    Application *_application;
};

} // namespace bl

#endif // _BL_EVENT_DISPATCHER_H
