#ifndef _BL_GL_SHADER_H
#define _BL_GL_SHADER_H

// C
#include <stdint.h>

// OpenGL
#include <GL/glew.h>

namespace bl {
namespace gl {

class Shader
{
public:
    enum class Type {
        Vertex,
        Fragment,
    };

public:
    Shader(Shader::Type type);

    Shader::Type type() const;

    void compile(const char *source);

    bool is_compiled() const;

    GLuint id() const;

private:
    Shader::Type _type;

    GLuint _id;
    bool _is_compiled;
};

} // namespace gl
} // namespace bl

#endif // _BL_GL_SHADER_H
