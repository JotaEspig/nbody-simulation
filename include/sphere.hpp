/**
 * \file sphere.hpp
 * \brief Sphere class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <memory>
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
    std::shared_ptr<axolote::gl::VAO> vao = axolote::gl::VAO::create();
    /** vertices VBO **/
    std::shared_ptr<axolote::gl::VBO> vertices_vbo = axolote::gl::VBO::create();
    /** indices EBO **/
    std::shared_ptr<axolote::gl::EBO> indices_ebo = axolote::gl::EBO::create();

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
    void bind_shader(std::shared_ptr<axolote::gl::Shader> shader_program
    ) override;
    /**
     * \brief Get the shader
     * \author João Vitor Espig (JotaEspig)
     * \returns Shader binded to the object
     **/
    std::vector<std::shared_ptr<axolote::gl::Shader>> get_shaders() const override;
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
    std::shared_ptr<axolote::gl::Shader> shader;
};
