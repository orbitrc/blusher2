#include "sample-view.h"

#include <stdio.h>

#include <primer/string.h>

#include <blusher/point.h>

SampleView::SampleView(bl::View *parent)
    : bl::View(parent)
{
    this->_pressed_x = 0;
    this->_pressed_y = 0;

    this->set_width(150);
    this->set_height(150);
    this->set_color(bl::Color::from_rgb(0, 255, 0));
    this->_sample_image = new bl::Image("brc:/io.orbitrc.blusher/sample.png"_S);
    this->draw_image(bl::Point(0, 0), *this->_sample_image);
}

//=============
// Events
//=============

void SampleView::pointer_press_event(std::shared_ptr<bl::PointerEvent> event)
{
    if (event->button() == bl::Button::Left) {
        fprintf(stderr, "pointer press x, y: %f, %f\n", event->x(), event->y());
        this->_pressed_x = event->x();
        this->_pressed_y = event->y();
    }
}

void SampleView::pointer_move_event(std::shared_ptr<bl::PointerEvent> event)
{
    if (event->button() == bl::Button::Left) {
        double offset_x = event->x() - this->_pressed_x;
        double offset_y = event->y() - this->_pressed_y;

        this->set_x(offset_x);
        this->set_y(offset_y);
    }

    return View::pointer_move_event(event);
}
