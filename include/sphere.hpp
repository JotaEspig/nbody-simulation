#pragma once

#include <vector>

#include <axolote/engine.hpp>

class Sphere : public axolote::Drawable {
public:
    axolote::gl::VAO vao;
    axolote::gl::VBO vertices_vbo;
    axolote::gl::EBO indices_ebo;

    Sphere();

    void bind_shader(const axolote::gl::Shader &shader_program) override;
    axolote::gl::Shader get_shader() const override;
    void update(double dt) override;
    void draw() override;
    void draw(const glm::mat4 &mat) override;
    std::vector<GLuint> indices() const;

private:
    std::vector<glm::vec3> _vertices;
    std::vector<GLuint> _indices;
    axolote::gl::Shader shader;
};
