#include "sample-view.h"

#include <stdio.h>

#include <primer/string.h>

#include <blusher/point.h>

SampleView::SampleView(bl::View *parent)
    : bl::View(parent)
{
    this->set_width(150);
    this->set_height(150);
    this->set_color(bl::Color::from_rgb(0, 255, 0));
    this->_sample_image = new bl::Image("brc:/io.orbitrc.blusher/sample.png"_S);
    this->draw_image(bl::Point(0, 0), *this->_sample_image);
}

//=============
// Events
//=============

void SampleView::pointer_move_event(std::shared_ptr<bl::PointerEvent> event)
{
    if (event->button() == bl::Button::Left) {
        //
        fprintf(stderr, "pointer move x, y: %f, %f\n", event->x(), event->y());
        double offset_x = event->x() - this->x();
        this->set_x(offset_x);
    }

    return View::pointer_move_event(event);
}
