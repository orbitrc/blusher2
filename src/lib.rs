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

//=============
// Window
//=============
pub struct Window {
    window: *mut c_void,
}

impl Window {
    pub fn new() -> Window {
        unsafe {
            let window = Window {
                window: libblusher::bl_window_new(),
            };

            window
        }
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

    fn surface_ptr(&self) -> *mut c_void;
}

pub struct PlainSurface {
    bl_surface: *mut c_void,
}

//=================
// PlainSurface
//=================
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
            libblusher::bl_surface_show(self.bl_surface);
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
