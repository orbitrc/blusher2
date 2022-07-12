#ifndef _BL_BUTTON_H
#define _BL_BUTTON_H

// C++
#include <memory>

// Blusher
#include <blusher/view.h>
#include <blusher/pointer-event.h>
#include <blusher/label.h>

namespace bl {
namespace controls {

class Button : public View
{
public:
    Button(View *parent);
    ~Button();

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;
    void pointer_release_event(std::shared_ptr<PointerEvent> event) override;
    void pointer_click_event(std::shared_ptr<PointerEvent> event) override;

private:
    View *_image_view;
    Label *_label;
};

} // namespace controls
} // namespace bl

#endif // _BL_BUTTON_H
