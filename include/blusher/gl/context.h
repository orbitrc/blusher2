#ifndef _BL_GL_CONTEXT_H
#define _BL_GL_CONTEXT_H

// C
#include <stdint.h>

// EGL
#include <EGL/egl.h>

namespace bl {

namespace gl {

class Context
{
public:
    Context(EGLDisplay display);

    EGLDisplay egl_display();
    EGLConfig egl_config();
    EGLContext egl_context();

private:
    EGLDisplay _egl_display;

    EGLConfig *_egl_configs;
    EGLConfig _egl_config;
    EGLContext _egl_context;
};

} // namespace gl

} // namespace bl

#endif // _BL_GL_CONTEXT_H
