#include <blusher/cursor.h>

// Blusher
#include <blusher/wayland/wl-surface.h>

namespace bl {

//===============
// CursorImpl
//===============

class CursorImpl
{
public:
    CursorImpl(Cursor *cursor)
    {
        this->_cursor = cursor;
    }

    ~CursorImpl()
    {
    }

    struct wl_surface* wl_surface()
    {
        return nullptr;
    }

private:
    Cursor *_cursor;
};

//==============
// Cursor
//==============

Cursor::Cursor(Cursor::Shape shape)
{
    this->_shape = shape;

    this->_hot_spot_x = 0;
    this->_hot_spot_y = 0;

    this->_impl = new CursorImpl(this);
}

Cursor::~Cursor()
{
    delete this->_impl;
}

Cursor::Shape Cursor::shape() const
{
    return this->_shape;
}

uint32_t Cursor::hot_spot_x() const
{
    return this->_hot_spot_x;
}

uint32_t Cursor::hot_spot_y() const
{
    return this->_hot_spot_y;
}

struct wl_surface* Cursor::wl_surface()
{
    return this->_impl->wl_surface();
}

} // namespace bl
