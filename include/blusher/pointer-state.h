#ifndef _BL_POINTER_STATE_H
#define _BL_POINTER_STATE_H

// C
#include <stdint.h>

// Bluhser
#include <blusher/input.h>

namespace bl {

class PointerState
{
public:
    PointerState();

    double x;
    double y;
    PointerButton button;

    bool pressed() const;
    void set_pressed(bool value);

private:
    bool _pressed;
};

} // namespace bl

#endif // _BL_POINTER_STATE_H
