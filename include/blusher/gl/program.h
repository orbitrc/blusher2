#ifndef _BL_GL_PROGRAM_H
#define _BL_GL_PROGRAM_H

// C
#include <stdint.h>

// C++
#include <memory>

// OpenGL
#include <GL/glew.h>

namespace bl {
namespace gl {

class Program
{
public:
    Program();

private:
    GLuint _program;
};

} // namespace gl
} // namespace bl

#endif // _BL_GL_PROGRAM_H
