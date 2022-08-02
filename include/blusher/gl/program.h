#ifndef _BL_GL_PROGRAM_H
#define _BL_GL_PROGRAM_H

// C
#include <stdint.h>

// C++
#include <memory>

// OpenGL
#include <GL/glew.h>

// Blusher
#include <blusher/gl/shader.h>

namespace bl {
namespace gl {

class Program
{
public:
    Program();

    GLuint id() const;

    void attach_shader(std::shared_ptr<gl::Shader> shader);

    std::shared_ptr<gl::Shader> vertex_shader();

    std::shared_ptr<gl::Shader> fragment_shader();

    void link();

private:
    GLuint _id;

    std::shared_ptr<gl::Shader> _vertex_shader;
    std::shared_ptr<gl::Shader> _fragment_shader;
};

} // namespace gl
} // namespace bl

#endif // _BL_GL_PROGRAM_H
