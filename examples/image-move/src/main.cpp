#include <stdio.h>

#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>
#include <blusher/view.h>
#include <blusher/point.h>
#include <blusher/image.h>

#include "sample-view.h"

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::Window window;

    window.show();

    SampleView v(window.root_view().get());

    v.update();
    v.surface()->root_view()->update();

    return app.exec();
}
