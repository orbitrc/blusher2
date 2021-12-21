use std::process;

use blusher::{Surface};
use blusher::{Application, Window, PlainSurface, Color};
use blusher::{PointerEvent};

struct MySurface {
    surface: PlainSurface,
}

impl MySurface {
    fn new(parent: Option<&dyn Surface>) -> MySurface {
        let mut my_surface = MySurface {
            surface: PlainSurface::new(parent),
        };

        Application::instance().register_surface(
            my_surface.surface_ptr(),
            &mut my_surface as *mut dyn Surface
        );

        my_surface.surface.set_color(Color::from_rgb(200, 100, 0));

        my_surface
    }
}

impl Surface for MySurface {
    fn show(&self) {
        self.surface.show();
    }

    fn set_geometry(&mut self, x: f64, y: f64, width: f64, height: f64) {
        self.surface.set_geometry(x, y, width, height);
    }

    fn pointer_press_event(&self, event: &PointerEvent) {
        println!("MySurface pointer event!");
        PlainSurface::pointer_press_event(&self.surface, event);
    }

    fn surface_ptr(&self) -> *mut std::os::raw::c_void {
        self.surface.surface_ptr()
    }
}

fn main() {
    let app = Application::new();
    let window = Window::new();
    app.add_window(&window);

    window.show();

    let mut surface = PlainSurface::new(Some(window.body()));
    surface.set_geometry(0.0, 0.0, 100.0, 100.0);
    surface.show();
    let mut surface2 = MySurface::new(Some(&surface));
    surface2.set_geometry(10.0, 10.0, 50.0, 50.0);
    // surface2.set_color(Color::from_rgb(255, 0, 0));
    surface2.show();

    let ret = app.exec();
    if ret != 0 {
        process::exit(ret);
    }
}
