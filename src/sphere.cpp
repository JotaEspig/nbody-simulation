#include "axolote/glad/glad.h"
#include "axolote/model.hpp"

#include "sphere.hpp"

#define UNUSED(x) (void)(x)

Sphere::Sphere() {
    axolote::Model model{"./resources/models/sphere/sphere.obj"};

    for (auto &v : model.meshes[0].vertices) {
        _vertices.push_back(v.position);
        _colors.push_back(v.color);
    }
    _indices = model.meshes[0].indices;

    vao.bind();
    vertices_vbo = axolote::gl::VBO{_vertices};
    glGenBuffers(1, &colors_vbo.id);
    colors_vbo.bind();
    glBufferData(
        GL_ARRAY_BUFFER, _colors.size() * sizeof(glm::vec3), _colors.data(),
        GL_DYNAMIC_DRAW
    );
    indices_ebo = axolote::gl::EBO(_indices);

    vao.link_attrib(vertices_vbo, 0, 3, GL_FLOAT, 0, (void *)0);
    vao.link_attrib(colors_vbo, 1, 3, GL_FLOAT, 0, (void *)0);
    vao.unbind();
    vertices_vbo.unbind();
    colors_vbo.unbind();
}

void Sphere::set_color(const glm::vec3 &color) {
    _colors = std::vector<glm::vec3>(_colors.size(), color);
    colors_vbo.bind();
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, _colors.size() * sizeof(glm::vec3), _colors.data()
    );
    colors_vbo.unbind();
}

void Sphere::bind_shader(const axolote::gl::Shader &shader_program) {
    shader = shader_program;
}

axolote::gl::Shader Sphere::get_shader() const {
    return shader;
}

void Sphere::update(double dt) {
    UNUSED(dt);
}

void Sphere::draw() {
    shader.activate();
    GLsizeiptr size = _indices.size();
    vao.bind();
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
    vao.unbind();
}

void Sphere::draw(const glm::mat4 &mat) {
    UNUSED(mat);
    draw();
}

std::vector<GLuint> Sphere::indices() const {
    return _indices;
}
