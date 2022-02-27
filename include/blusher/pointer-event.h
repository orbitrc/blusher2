#ifndef _BL_POINTER_EVENT_H
#define _BL_POINTER_EVENT_H

namespace bl {

class PointerEvent
{
public:
    enum class Button {
        None,
        Left,
        Right,
        Middle,
    };

public:
    PointerEvent(Button button, double x, double y);
    ~PointerEvent();

    Button button() const;

    double x() const;

    double y() const;

private:
    Button _button;
    double _x;
    double _y;
};

} // namespace bl

#endif /* _BL_POINTER_EVENT_H */
