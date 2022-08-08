#include <blusher/gl/context.h>

// C
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

namespace bl {

namespace gl {

Context::Context(EGLDisplay display)
{
    this->_egl_display = display;

    // Init.
    this->_egl_configs = nullptr;

    EGLBoolean result;
    EGLint major;
    EGLint minor;
    EGLint count;
    EGLint n;
    EGLint size;

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RED_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_ALPHA_SIZE,
        8,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_NONE,
    };

    EGLint context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        4,
        EGL_CONTEXT_MINOR_VERSION,
        6,
        EGL_NONE,
    };

    eglBindAPI(EGL_OPENGL_API);

    result = eglInitialize(this->_egl_display, &major, &minor);
    if (result != EGL_TRUE) {
        fprintf(stderr, "Can't initialize EGL display!\n");
        return;
    }

    eglGetConfigs(this->_egl_display, NULL, 0, &count);

    this->_egl_configs = new EGLConfig[count];

    eglChooseConfig(this->_egl_display,
        config_attribs, this->_egl_configs, count, &n);

    for (int32_t i = 0; i < n; ++i) {
        eglGetConfigAttrib(this->_egl_display,
            this->_egl_configs[i], EGL_BUFFER_SIZE, &size);
        eglGetConfigAttrib(this->_egl_display,
            this->_egl_configs[i], EGL_RED_SIZE, &size);
    }
    // Just choose the first one.
    this->_egl_config = this->_egl_configs[0];

    this->_egl_context = eglCreateContext(this->_egl_display,
        this->_egl_config, EGL_NO_CONTEXT, context_attribs);
    if (this->_egl_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "[WARN] Context::Context() - EGL_NO_CONTEXT\n");
    }

    this->make_current(EGL_NO_SURFACE, false);
}

EGLDisplay Context::egl_display()
{
    return this->_egl_display;
}

EGLConfig Context::egl_config()
{
    return this->_egl_config;
}

EGLContext Context::egl_context()
{
    return this->_egl_context;
}

void Context::make_current(EGLSurface egl_surface, bool no_context)
{
    EGLBoolean result;

    EGLContext egl_context = !no_context
        ? this->_egl_context
        : EGL_NO_CONTEXT;

    result = eglMakeCurrent(this->_egl_display,
        egl_surface, egl_surface,
        egl_context);
    if (result != EGL_TRUE) {
        fprintf(stderr,
            "[WARN] Context::make_current() - eglMakeCurrent failed!\n");
    }
}

} // namespace gl

} // namespace bl
