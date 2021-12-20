mod libblusher;

use std::ffi::c_void;
use std::ptr::null_mut;

//================
// Application
//================
pub struct Application {
    application: *mut c_void,
}

impl Application {
    pub fn new() -> Application {
        unsafe {
            let application = Application {
                application: libblusher::bl_application_new(),
            };

            application
        }
    }

    pub fn add_window(&self, window: &Window) {
        unsafe {
            libblusher::bl_application_add_window(self.application, window.window);
        }
    }

    pub fn exec(&self) -> i32 {
        unsafe {
            libblusher::bl_application_exec(self.application)
        }
    }
}

impl Drop for Application {
    fn drop(&mut self) {
        unsafe {
            libblusher::bl_application_free(self.application);
        }
    }
}

//=============
// Window
//=============
pub struct Window {
    window: *mut c_void,
    body: PlainSurface,
}

impl Window {
    pub fn new() -> Window {
        unsafe {
            let bl_window = libblusher::bl_window_new();
            let body = PlainSurface::new(None);
            libblusher::bl_window_set_body(bl_window, body.bl_surface);

            let window = Window {
                window: libblusher::bl_window_new(),
                body: body,
            };

            window
        }
    }

    pub fn body(&self) -> &PlainSurface {
        &self.body
    }

    pub fn show(&self) {
        unsafe {
            libblusher::bl_window_show(self.window);
        }
    }
}

//=================
// Trait Surface
//=================
pub trait Surface {
    fn show(&self);

    fn set_geometry(&mut self, x: f64, y: f64, width: f64, height: f64);

    fn surface_ptr(&self) -> *mut c_void;
}

//=================
// PlainSurface
//=================
pub struct PlainSurface {
    bl_surface: *mut c_void,
}

impl PlainSurface {
    pub fn new(parent: Option<&dyn Surface>) -> PlainSurface {
        let bl_surface = match parent {
            Some(surface) => {
                surface.surface_ptr()
            }
            None => {
                null_mut()
            }
        };
        let surface = PlainSurface {
            bl_surface: unsafe { libblusher::bl_surface_new(bl_surface) },
        };

        surface
    }
}

impl Surface for PlainSurface {
    fn show(&self) {
        unsafe {
            libblusher::bl_surface_paint(self.bl_surface);
            libblusher::bl_surface_show(self.bl_surface);
        }
    }

    fn set_geometry(&mut self, x: f64, y: f64, width: f64, height: f64) {
        unsafe {
            libblusher::bl_surface_set_geometry(self.bl_surface, x, y, width, height);
        }
    }

    fn surface_ptr(&self) -> *mut c_void {
        self.bl_surface
    }
}

impl Drop for PlainSurface {
    fn drop(&mut self) {
        unsafe {
            libblusher::bl_surface_free(self.surface_ptr());
        }
    }
}
