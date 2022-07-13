#include <blusher/output.h>

namespace bl {

Output::Output(struct wl_output *wl_output)
{
    this->_wl_output = wl_output;
    this->_name = ""_S;
    this->_scale = 1;
    this->_width = 0;
    this->_height = 0;
}

//==================
// Private Methods
//==================

void Output::set_name(const pr::String& name)
{
    this->_name = name;
}

void Output::set_scale(uint32_t scale)
{
    this->_scale = scale;
}

//==================
// Public Methods
//==================

const pr::String Output::name() const
{
    return this->_name;
}

uint32_t Output::scale() const
{
    return this->_scale;
}

uint32_t Output::width() const
{
    return this->_width;
}

uint32_t Output::height() const
{
    return this->_height;
}

} // namespace bl
