#include <blusher/application.h>
#include <blusher/view.h>
#include <blusher/desktop-surface.h>
#include <blusher/color.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::DesktopSurface desktop_surface(bl::DesktopSurface::Role::Toplevel);

    desktop_surface.set_size(640, 360);

    bl::View view(desktop_surface.root_view());
    view.set_size({50, 50});
    view.fill(bl::Color::from_rgba(255, 0, 0, 150));
    view.paint();
    view.update();

    bl::View view_blue(desktop_surface.root_view());
    view_blue.set_size({50, 50});
    view_blue.set_position({50, 50});
    view_blue.fill(bl::Color::from_rgb(0, 0, 255));

    bl::View view_inner(&view_blue);
    view_inner.set_size({50, 50});
    view_inner.set_position({10, 10});
    view_inner.fill(bl::Color::from_rgb(0, 255, 0));

    desktop_surface.show();

    return app.exec();
}
