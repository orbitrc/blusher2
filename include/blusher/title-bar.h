#ifndef _BL_TITLE_BAR_H
#define _BL_TITLE_BAR_H

#include <blusher/view.h>
#include <blusher/image.h>

#include "surface.h"

namespace bl {

class Window;

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
    void pointer_click_event(std::shared_ptr<PointerEvent> event) override;

private:
    TitleBarButton::Type _type;

    Image *_close_image;
};

//===================
// Title Bar View
//===================

class TitleBarView : public View
{
public:
    TitleBarView(View *parent);

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;
};

//================
// Title Bar
//================

class TitleBar : public View
{
public:
    TitleBar(View *parent);

    Surface* body();
    void set_body(Surface *surface);

    Window* window();
    void set_window(Window *window);

protected:
    void pointer_press_event(std::shared_ptr<PointerEvent> event) override;

private:
    Surface *_body;
    Window *_window;

    TitleBarView *_main_view;

    Image *_close_image;
};

} // namespace bl

#endif /* _BL_TITLE_BAR_H */
