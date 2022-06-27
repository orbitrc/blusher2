#ifndef _SAMPLE_VIEW_H
#define _SAMPLE_VIEW_H

// C++
#include <memory>

// Blusher
#include <blusher/application.h>
#include <blusher/resource.h>
#include <blusher/image.h>
#include <blusher/window.h>
#include <blusher/view.h>
#include <blusher/pointer-event.h>

class SampleView : public bl::View
{
public:
    SampleView(bl::View *parent);

protected:
    void pointer_press_event(std::shared_ptr<bl::PointerEvent> event) override;
    void pointer_move_event(std::shared_ptr<bl::PointerEvent> event) override;

private:
    bl::Image *_sample_image;

    double _pressed_x;
    double _pressed_y;
};

#endif // _SAMPLE_VIEW_H
