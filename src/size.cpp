#include <blusher/size.h>

namespace bl {

Size::Size(double width, double height)
{
    this->_width = width;
    this->_height = height;
}

double Size::width() const
{
    return this->_width;
}

double Size::height() const
{
    return this->_height;
}

void Size::set_width(double width)
{
    if (this->_width != width) {
        this->_width = width;
    }
}

void Size::set_height(double height)
{
    if (this->_height != height) {
        this->_height = height;
    }
}

} // namespace bl
