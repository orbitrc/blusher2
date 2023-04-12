#include <assert.h>
#include <stdio.h>

#include <blusher/point.h>
#include <blusher/rect.h>

void print_rect(const bl::Rect& rect, const char *id)
{
    printf("%s: (%f, %f) %fx%f\n",
        id, rect.x(), rect.y(), rect.width(), rect.height());
}

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

    bl::Rect rect9(0, 0, 300, 300);
    bl::Rect rect10(30, 30, 240, 240);
    assert(rect9.intersection(rect10) == bl::Rect(30, 30, 240, 240));

    bl::Rect rect11(-20, -20, 15, 15);
    bl::Rect rect12(-10, -10, 10, 10);
    assert(rect11.intersection(rect12) == bl::Rect(-10, -10, 5, 5));

    bl::Rect rect13(345.0, 306.0, 46.5, 125.5);
    bl::Rect rect14(293.0, 350.0, 147.0, 34.0);
    assert(rect13.intersection(rect14) == bl::Rect(345.0, 350.0, 46.5, 34.0));

    bl::Rect rect15(50, 40, 210, 130);
    bl::Rect rect16(40, 50, 190, 60);
    assert(rect15.intersection(rect16) == bl::Rect(50, 50, 180, 60));
}

void rect_valid_viewport()
{
    bl::Rect red(0, 0, 300, 300);
    bl::Rect white(30, 30, 240, 240);
    bl::Rect purple(120, 170, 130, 70);
    bl::Rect green(50, 40, 210, 130);
    bl::Rect grey(40, 50, 190, 60);
    bl::Rect blue(40, 90, 20, 20);

    auto red_white = red.intersection(white);
    print_rect(red_white, "red_white");
    auto white_purple = red_white.intersection(purple);
    print_rect(white_purple, "white_purple");

    auto white_green = red_white.intersection(green);
    print_rect(white_green, "white_green");
    auto green_grey = white_green.intersection(grey);
    print_rect(green_grey, "green_grey");
    auto grey_blue = green_grey.intersection(blue);
    print_rect(grey_blue, "grey_blue");
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

    rect_valid_viewport();

    return 0;
}
