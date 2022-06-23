#include <assert.h>

#include <blusher/point.h>
#include <blusher/rect.h>

int main(int argc, char *argv[])
{
    bl::Rect rect(10, 10, 20, 20);
    bl::Point point(1, 1);
    bl::Point point2(11, 11);
    bl::Point point3(40, 21);

    assert(!rect.contains(point));
    assert(rect.contains(point2));
    assert(!rect.contains(point3));

    return 0;
}
