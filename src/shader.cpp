#include <blusher/gl/shader.h>

// C
#include <stdlib.h>
#include <stdio.h>

namespace bl {
namespace gl {

Shader::Shader(Shader::Type type)
{
    this->_type = type;
    this->_id = 0;
    this->_is_compiled = false;

    // Create shader.
    auto gl_type = GL_VERTEX_SHADER;
    switch (type) {
    case Shader::Type::Vertex:
        gl_type = GL_VERTEX_SHADER;
        break;
    case Shader::Type::Fragment:
        gl_type = GL_FRAGMENT_SHADER;
        break;
    default:
        break;
    }

    this->_id = glCreateShader(gl_type);
    if (this->_id == 0) {
        fprintf(stderr, "[WARN] Shader::Shader() - Shader is 0!\n");
    }
}

Shader::Type Shader::type() const
{
    return this->_type;
}

void Shader::compile(const char *source)
{
    GLint compiled;

    // Load the shader source.
    glShaderSource(this->_id, 1, &source, NULL);

    // Compile the shader.
    glCompileShader(this->_id);

    // Check the compile status.
    glGetShaderiv(this->_id, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint info_len = 0;

        glGetShaderiv(this->_id, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char *info_log = (char*)malloc(sizeof(char));
            glGetShaderInfoLog(this->_id, info_len, NULL, info_log);
            fprintf(stderr, "Error compiling shader: %s\n", info_log);
            free(info_log);
        }

        // glDeleteShader(shader);
        this->_is_compiled = false;
        return;
    }

    this->_is_compiled = true;
}

bool Shader::is_compiled() const
{
    return this->_is_compiled;
}

GLuint Shader::id() const
{
    return this->_id;
}

} // namespace gl
} // namespace bl
