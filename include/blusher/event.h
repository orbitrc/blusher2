#ifndef _BL_EVENT_H
#define _BL_EVENT_H

namespace bl {

class Event
{
public:
    enum class Type {
        PointerEnter,
        PointerLeave,
        PointerPress,
        PointerRelease,
        PointerMove,
        Resize,
        Update,
    };

public:
    Event(Event::Type type);

    Event::Type type() const;

    void set_type(Event::Type type);

private:
    Event::Type _type;
};

} // namespace bl

#endif // _BL_EVENT_H
