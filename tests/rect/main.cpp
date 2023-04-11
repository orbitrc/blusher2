#include <assert.h>
#include <stdio.h>

#include <blusher/point.h>
#include <blusher/rect.h>

void rect_intersection()
{
    bl::Rect rect1(0, 0, 10, 10);
    bl::Rect rect2(5, 5, 10, 10);
    assert(rect1.intersection(rect2) == bl::Rect(5, 5, 5, 5));

    bl::Rect rect3(10, 10, 10, 10);
    bl::Rect rect4(-20, -20, 10, 10);
    assert(rect3.intersection(rect4) == bl::Rect());

    bl::Rect rect5(0, 0, 10, 10);
    bl::Rect rect6(5, -5, 20, 20);
    bl::Rect inter = rect6.intersection(rect5);
    printf("(%f, %f) %fx%f\n",
        inter.x(), inter.y(), inter.width(), inter.height());
    assert(rect5.intersection(rect6) == bl::Rect(5, 0, 5, 10));
    assert(rect6.intersection(rect5) == bl::Rect(5, 0, 5, 10));

    bl::Rect rect7(0, 0, 10, 10);
    bl::Rect rect8(10, 0, 10, 10);
    inter = rect7.intersection(rect8);
    printf("(%f, %f) %fx%f\n",
        inter.x(), inter.y(), inter.width(), inter.height());
    assert(rect7.intersection(rect8) == bl::Rect());
}

int main(int argc, char *argv[])
{
    bl::Rect rect(10, 10, 20, 20);
    bl::Point point(1, 1);
    bl::Point point2(11, 11);
    bl::Point point3(40, 21);

    assert(!rect.contains(point));
    assert(rect.contains(point2));
    assert(!rect.contains(point3));

    rect_intersection();

    return 0;
}
