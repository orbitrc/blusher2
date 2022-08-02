#include <blusher/gl/program.h>

// C
#include <stdlib.h>
#include <stdio.h>

// OpenGL
#include <GL/glew.h>

namespace bl {
namespace gl {

Program::Program()
{
    this->_id = 0;
    this->_vertex_shader = nullptr;
    this->_fragment_shader = nullptr;

    // OpenGL
    this->_id = glCreateProgram();
    if (this->_id == 0) {
        fprintf(stderr, "[WARN] Program::Program() - Program is 0!\n");
    }
}

GLuint Program::id() const
{
    return this->_id;
}

void Program::attach_shader(std::shared_ptr<gl::Shader> shader)
{
    if (shader->type() == Shader::Type::Vertex) {
        if (this->_vertex_shader != nullptr) {
            fprintf(stderr, "[WARN] Program::attach_shader() - "
                "Vertex shader already attached!\n");
            return;
        }
    } else if (shader->type() == Shader::Type::Fragment) {
        if (this->_fragment_shader != nullptr) {
            fprintf(stderr, "[WARN] Program::attach_shader() - "
                "Fragment shader already attached!\n");
            return;
        }
    }

    glAttachShader(this->_id, shader->id());

    if (shader->type() == Shader::Type::Vertex) {
        this->_vertex_shader = shader;
    } else if (shader->type() == Shader::Type::Fragment) {
        this->_fragment_shader = shader;
    }
}

std::shared_ptr<Shader> Program::vertex_shader()
{
    return this->_vertex_shader;
}

std::shared_ptr<Shader> Program::fragment_shader()
{
    return this->_fragment_shader;
}

void Program::link()
{
    glLinkProgram(this->_id);

    // Check the link status.
    GLint linked;
    glGetProgramiv(this->_id, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint info_len = 0;
        glGetProgramiv(this->id(), GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1) {
            char *info_log = (char*)malloc(sizeof(char) * info_len);

            glGetProgramInfoLog(this->id(), info_len, NULL, info_log);
            fprintf(stderr, "Error linking program: %s\n", info_log);
            free(info_log);
        }
    }
}

} // namespace gl
} // namespace bl
