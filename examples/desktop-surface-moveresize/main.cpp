#include <blusher/application.h>
#include <blusher/view.h>
#include <blusher/desktop-surface.h>
#include <blusher/color.h>

class MoveArea : public bl::View
{
public:
    MoveArea(bl::View *parent)
        : bl::View(parent)
    {
        this->set_size({100, 30});
        this->fill(bl::Color::from_rgb(100, 100, 100));

        this->_desktop_surface = nullptr;
    }

    void set_desktop_surface(bl::DesktopSurface *surf)
    {
        this->_desktop_surface = surf;
    }

protected:
    void pointer_press_event(std::shared_ptr<bl::PointerEvent> event) override
    {
        if (this->_desktop_surface != nullptr) {
            this->_desktop_surface->toplevel_move();
        }

        bl::View::pointer_press_event(event);
    }

private:
    bl::DesktopSurface *_desktop_surface;
};

class ResizeArea : public bl::View
{
public:
    ResizeArea(bl::View *parent)
        : bl::View(parent)
    {
        this->set_size({30, 30});
        this->fill(bl::Color::from_rgb(0, 255, 0));

        this->_desktop_surface = nullptr;
    }

    void set_desktop_surface(bl::DesktopSurface *surf)
    {
        this->_desktop_surface = surf;
    }

protected:
    void pointer_press_event(std::shared_ptr<bl::PointerEvent> event) override
    {
        if (this->_desktop_surface != nullptr) {
            this->_desktop_surface->toplevel_resize(
                bl::XdgToplevel::ResizeEdge::Bottom);
        }

        bl::View::pointer_press_event(event);
    }

private:
    bl::DesktopSurface *_desktop_surface;
};

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::DesktopSurface desktop_surface(bl::DesktopSurface::Role::Toplevel);

    desktop_surface.set_size(640, 360);

    MoveArea move_area(desktop_surface.root_view());
    move_area.set_position({10, 10});
    move_area.set_debug_id("MoveArea"_S);
    move_area.set_desktop_surface(&desktop_surface);

    ResizeArea resize_area(desktop_surface.root_view());
    resize_area.set_position({100, 100});
    resize_area.set_debug_id("ResizeArea"_S);
    resize_area.set_desktop_surface(&desktop_surface);

    desktop_surface.show();

    return app.exec();
}
