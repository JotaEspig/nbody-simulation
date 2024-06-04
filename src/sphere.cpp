#include "axolote/glad/glad.h"
#include "axolote/model.hpp"

#include "sphere.hpp"

#define UNUSED(x) (void)(x)

Sphere::Sphere() {
    axolote::Model model{"./resources/models/sphere/sphere.obj"};

    for (auto &v : model.meshes[0].vertices) {
        _vertices.push_back(v.position);
    }
    _indices = model.meshes[0].indices;

    vao.bind();
    vertices_vbo = axolote::gl::VBO{_vertices};
    indices_ebo = axolote::gl::EBO(_indices);

    vao.link_attrib(vertices_vbo, 0, 3, GL_FLOAT, 0, (void *)0);
    vao.unbind();
    vertices_vbo.unbind();
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
