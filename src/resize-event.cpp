#include <blusher/resize-event.h>

namespace bl {

ResizeEvent::ResizeEvent(const Size& size, const Size& old_size)
    : Event(Event::Type::Resize),
      _size(size)
{
    (void)old_size;
}

} // namespace bl
