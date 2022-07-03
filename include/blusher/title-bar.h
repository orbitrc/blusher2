#ifndef _BL_TITLE_BAR_H
#define _BL_TITLE_BAR_H

#include <blusher/view.h>
#include <blusher/image.h>

#include "surface.h"

namespace bl {

//===============
// Button
//===============
class TitleBarButton : public View
{
public:
    enum class Type {
        Close,
        Maximize,
        Restore,
        Minimize,
    };

public:
    TitleBarButton(View *parent);

    void set_type(TitleBarButton::Type type);

protected:
    void pointer_enter_event(std::shared_ptr<PointerEvent> event) override;
    void pointer_leave_event(std::shared_ptr<PointerEvent> event) override;
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;

private:
    TitleBarButton::Type _type;

    Image *_close_image;
};

//================
// Title Bar
//================

class TitleBar : public Surface
{
public:
    TitleBar(Surface *parent);

    Surface* body();
    void set_body(Surface *surface);

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;

private:
    Surface *_body;

    Image *_close_image;
};

} // namespace bl

#endif /* _BL_TITLE_BAR_H */
