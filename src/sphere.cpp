#include <vector>

#include "axolote/glad/glad.h"
#include "axolote/model.hpp"

#include "sphere.hpp"

#define UNUSED(x) (void)(x)

Sphere::Sphere() {
    axolote::Model model{"./resources/models/sphere/sphere.obj"};

    for (auto &v : model.meshes[0].vertices) {
        _vertices.push_back(v.pos);
    }
    _indices = model.meshes[0].indices;

    // binds to VAO
    vao->bind();

    vertices_vbo->buffer_data(
        sizeof(glm::vec3) * _vertices.size(), _vertices.data()
    );
    indices_ebo->buffer_data(sizeof(GLuint) * _indices.size(), _indices.data());

    vao->link_attrib(vertices_vbo, 0, 3, GL_FLOAT, 0, (void *)0);

    vao->unbind();
    vertices_vbo->unbind();
    indices_ebo->unbind();
}

void Sphere::bind_shader(std::shared_ptr<axolote::gl::Shader> shader_program) {
    shader = shader_program;
}

std::vector<std::shared_ptr<axolote::gl::Shader>> Sphere::get_shaders() const {
    return {shader};
}

void Sphere::update(double dt) {
    UNUSED(dt);
}

void Sphere::draw() {
    shader->activate();
    GLsizeiptr size = _indices.size();
    vao->bind();
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
    vao->unbind();
}

void Sphere::draw(const glm::mat4 &mat) {
    UNUSED(mat);
    draw();
}

std::vector<GLuint> Sphere::indices() const {
    return _indices;
}
