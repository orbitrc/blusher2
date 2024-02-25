#include <stdio.h>

#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>
#include <blusher/view.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::Window window;

    bl::View view(window.body_view());
    view.set_content_policy(bl::View::ContentPolicy::BiggerContent);
    view.set_position({0, 0});
    view.set_size({100, 100});
    view.set_content_geometry({0, -50, 150, 150});
    view.fill(bl::Color::from_rgb(255, 0, 0));
    view.set_debug_id("BiggerView"_S);

    bl::View child(&view);
    child.set_position({70, 0});
    child.set_size({50, 60});
    child.fill(bl::Color::from_rgb(0, 255, 0));
    child.set_debug_id("ChildView"_S);

    window.show();

    return app.exec();
}
