/**
 * \file sphere.hpp
 * \brief Sphere class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <vector>

#include <axolote/engine.hpp>

/**
 * \class Sphere
 * \brief Sphere class
 * \author João Vitor Espig (JotaEspig)
 **/
class Sphere : public axolote::Drawable {
public:
    /** VAO **/
    axolote::gl::VAO vao;
    /** vertices VBO **/
    axolote::gl::VBO vertices_vbo;
    /** indices EBO **/
    axolote::gl::EBO indices_ebo;

    /**
     * \brief Default constructor
     * \author João Vitor Espig (JotaEspig)
     **/
    Sphere();

    /**
     * \brief Bind shader
     * \author João Vitor Espig (JotaEspig)
     * \param shader_program Shader to be binded
     **/
    void bind_shader(const axolote::gl::Shader &shader_program) override;
    /**
     * \brief Get the shader
     * \author João Vitor Espig (JotaEspig)
     * \returns Shader binded to the object
     **/
    axolote::gl::Shader get_shader() const override;
    /**
     * \brief Update
     * \author João Vitor Espig (JotaEspig)
     * \param dt Delta time
     **/
    void update(double dt) override;
    /**
     * \brief Draw
     * \author João Vitor Espig (JotaEspig)
     **/
    void draw() override;
    /**
     * \brief Draw
     * \author João Vitor Espig (JotaEspig)
     * \param mat Model matrix
     **/
    void draw(const glm::mat4 &mat) override;
    /**
     * \brief Get the vertices
     * \author João Vitor Espig (JotaEspig)
     * \returns Vertices of the object
     **/
    std::vector<GLuint> indices() const;

private:
    /** Vertices **/
    std::vector<glm::vec3> _vertices;
    /** Indices **/
    std::vector<GLuint> _indices;
    /** Shader **/
    axolote::gl::Shader shader;
};
