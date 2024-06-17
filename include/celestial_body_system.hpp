/**
 * \file celestial_body_system.hpp
 * \brief Celestial body system class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <memory>
#include <vector>

#include <axolote/engine.hpp>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "celestial_body.hpp"
#include "octree.hpp"
#include "sphere.hpp"

/**
 * \brief Celestial body system class
 * \author João Vitor Espig (JotaEspig)
 **/
class CelestialBodySystem : public axolote::Drawable {
public:
    /** Octree **/
    OcTree octree;
    /** Sphere mesh OpenGL object **/
    Sphere sphere;

    /**
     * \brief Default constructor
     * \author João Vitor Espig (JotaEspig)
     **/
    CelestialBodySystem() = default;

    /**
     * \brief Setup using normal json data
     * \author João Vitor Espig (JotaEspig)
     * \param data - json data
     **/
    void setup_using_json(nlohmann::json &data);
    /**
     * \brief Setup using a baked frame json data
     * \author João Vitor Espig (JotaEspig)
     * \param data - json data
     **/
    void setup_using_baked_frame_json(nlohmann::json &data);
    /**
     * \brief Setup instanced VBO
     * \author João Vitor Espig (JotaEspig)
     **/
    void setup_instanced_vbo();
    /**
     * \brief Add a body to the system
     * \author João Vitor Espig (JotaEspig)
     * \param mass - mass
     * \param pos - position
     * \param vel - velocity
     * \returns shared pointer to the body
     **/
    std::shared_ptr<CelestialBody>
    add_body(double mass, glm::vec3 pos, glm::vec3 vel);
    /**
     * \brief Naive algorithm O(n²)
     * \author João Vitor Espig (JotaEspig)
     * \param dt - delta time
     **/
    void naive_algorithm(double dt);
    /**
     * \brief Barnes-Hut algorithm O(n log n)
     * \author João Vitor Espig (JotaEspig)
     * \param dt - delta time
     **/
    void barnes_hut_algorithm(double dt);
    /**
     * \brief Get celestial bodies
     * \author João Vitor Espig (JotaEspig)
     * \returns vector of celestial bodies
     **/
    std::vector<std::shared_ptr<CelestialBody>> celestial_bodies() const;
    /**
     * \brief Update instanced VBOs
     * \author João Vitor Espig (JotaEspig)
     **/
    void update_vbos();

    /**
     * \brief Bind shader
     * \author João Vitor Espig (JotaEspig)
     * \param shader_program - shader program
     **/
    void bind_shader(const axolote::gl::Shader &shader_program) override;
    /**
     * \brief Get shader
     * \author João Vitor Espig (JotaEspig)
     * \returns shader
     **/
    axolote::gl::Shader get_shader() const override;
    /**
     * \brief Update
     * \author João Vitor Espig (JotaEspig)
     * \param dt - delta time
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
     * \param mat - matrix
     *
     * It just calls draw()
     **/
    void draw(const glm::mat4 &mat) override;

private:
    /** Instanced Matrix VBO **/
    axolote::gl::VBO instanced_matrices_vbo;
    /** Instanced Color VBO **/
    axolote::gl::VBO instanced_colors_vbo;
    /** Vector of celestial bodies on the simulation **/
    std::vector<std::shared_ptr<CelestialBody>> _celestial_bodies;

    /**
     * \brief Build octree
     * \author João Vitor Espig (JotaEspig)
     **/
    void build_octree();
};
