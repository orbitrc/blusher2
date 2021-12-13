mod libblusher;

use std::ffi::c_void;

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
