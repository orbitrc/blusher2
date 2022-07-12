#include <stdio.h>

#include <primer/string.h>

#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>
#include <blusher/view.h>
#include <blusher/point.h>
#include <blusher/label.h>
#include <blusher/controls/button.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    // bl::Surface surface;
    bl::Window window;

    // surface.set_color(bl::Color::from_rgba(100, 100, 100, 100));
    window.show();
    // surface.show();

    bl::Label label(window.root_view());
    // window.root_view()->update();
    label.set_text("Hello!"_S);
    label.paint();

    bl::controls::Button button(window.root_view());
    button.set_position(bl::Point(10, 10));
    button.update();

    window.root_view()->paint();
    window.root_view()->update();

    return app.exec();
}
