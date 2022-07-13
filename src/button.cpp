#include <blusher/controls/button.h>

// C
#include <stdio.h>

// Primer
#include <primer/string.h>

// Blusher
#include <blusher/size.h>
#include <blusher/color.h>

namespace bl {
namespace controls {

Button::Button(View *parent)
    : View(parent)
{
    // Initialize.
    this->_image_view = nullptr;
    this->_label = nullptr;

    // Set default size.
    this->set_size(Size(100, 30));

    // Set background image view.
    this->_image_view = new View(this);
    this->_image_view->set_size(this->geometry().size());
    this->_image_view->fill(Color::from_rgb(255, 0, 0));
    this->_image_view->paint();

    // Set text label.
    this->_label = new Label(this);
    this->_label->set_text("Button"_S);
    this->_label->paint();

    this->paint();
}

Button::~Button()
{
}

//===========
// Events
//===========

void Button::pointer_press_event(std::shared_ptr<PointerEvent> event)
{
    return View::pointer_press_event(event);
}

void Button::pointer_release_event(std::shared_ptr<PointerEvent> event)
{
    return View::pointer_release_event(event);
}

void Button::pointer_click_event(std::shared_ptr<PointerEvent> event)
{
    fprintf(stderr, "[DEBUG] Button clicked!\n");

    return View::pointer_click_event(event);
}

} // namespace controls
} // namespace bl
