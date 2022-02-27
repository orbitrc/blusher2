#include <blusher/title-bar.h>

#include <stdio.h>

#include <blusher/window.h>

namespace bl {

TitleBar::TitleBar(Surface *parent)
    : Surface(parent)
{
    this->set_geometry(0, 0, parent->width(), 30);
}

//===============
// Events
//===============

void TitleBar::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    (void)event;

    if (event->button() == PointerEvent::Button::Left) {
        fprintf(stderr, "bl::TitleBar::pointer_press_event() - Button::Left\n");
        for (Surface *it = this; it->parent() != nullptr; it = it->parent()) {
            Surface *win = it->parent();
            if (win->parent() == nullptr) {
                fprintf(stderr, "bl::TitleBar::pointer_press_event() - parent is NULL\n");
                win->move_if_window();
            }
        }
    }

    return Surface::pointer_press_event(event);
}

} // namespace bl
