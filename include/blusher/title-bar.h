#ifndef _BL_TITLE_BAR_H
#define _BL_TITLE_BAR_H

#include <blusher/view.h>

#include "surface.h"

namespace bl {

//===============
// Button
//===============
class TitleBarButton : public View
{
public:
    TitleBarButton(View *parent);

protected:
    void pointer_enter_event(std::shared_ptr<PointerEvent> event) override;
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;
};

//================
// Title Bar
//================

class TitleBar : public Surface
{
public:
    TitleBar(Surface *parent);

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;
};

} // namespace bl

#endif /* _BL_TITLE_BAR_H */
