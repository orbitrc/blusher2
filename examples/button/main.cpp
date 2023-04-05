#include <stdio.h>

#include <blusher/application.h>
#include <blusher/window.h>
#include <blusher/surface.h>
#include <blusher/color.h>
#include <blusher/view.h>
#include <blusher/controls/button.h>

int main(int argc, char *argv[])
{
    bl::Application app(argc, argv);

    bl::Window window;

    bl::Button button(window.body_view());
    button.clicked.connect([]() {
        fprintf(stderr, "==Button clicked!==\n");
    });

    window.show();

    return app.exec();
}
