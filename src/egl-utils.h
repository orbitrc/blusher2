#ifndef _BLUSHER_EGL_UTILS_H
#define _BLUSHER_EGL_UTILS_H

#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* egl_error_to_string(EGLint err);

#ifdef __cplusplus
}
#endif

#endif // _BLUSHER_EGL_UTILS_H
