/**
 * \file celestial_body.hpp
 * \brief Celestial body class
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axolote/engine.hpp>

/**
 * \brief Celestial body class
 * \author João Vitor Espig (JotaEspig)
 **/
class CelestialBody {
public:
    /** Celestial body matrix **/
    glm::mat4 mat{1.0f};
    /** Celestial body velocity **/
    glm::vec3 velocity;
    /** Celestial body position **/
    glm::vec3 pos;
    /** Should celestial body be merged (removed) in next frame **/
    bool merged = false;

    /**
     * \brief Celestial body constructor
     * \author João Vitor Espig (JotaEspig)
     * \param mass - celestial body mass
     * \param velocity - celestial body velocity
     * \param pos - celestial body position
     **/
    CelestialBody(double mass, const glm::vec3 &velocity, const glm::vec3 &pos);

    /**
     * \brief Calculates the acceleration vector between bodies
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     * \returns acceleration vector towards the other celestial body
     **/
    glm::vec3 calculate_acceleration_vec(const CelestialBody &other) const;
    /**
     * \brief Calculates the acceleration vec between a body and a massive point
     * \author João Vitor Espig (JotaEspig)
     * \param pos - point position
     * \param mass - point mass
     * \returns acceleration vector towards the point
     **/
    glm::vec3
    calculate_acceleration_vec(const glm::vec3 &pos, double mass) const;
    /**
     * \brief Checks if two celestial bodies are colliding
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     * \returns true if the two celestial bodies are colliding
     **/
    bool is_colliding(const CelestialBody &other) const;
    /**
     * \brief Merges two celestial bodies
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     *
     * the resulting celestial body will have its attributes be a weighted
     *average according to the mass of each body
     **/
    void merge(std::shared_ptr<CelestialBody> other);
    /**
     * \brief mass getter
     * \author João Vitor Espig (JotaEspig)
     * \returns celestial body mass
     **/
    double mass() const;
    /**
     * \brief mass and radius setter
     * \author João Vitor Espig (JotaEspig)
     * \param mass - new mass
     *
     * THe radius is setted according to the mass
     **/
    void set_mass(double mass);
    /**
     * \brief radius getter
     * \author João Vitor Espig (JotaEspig)
     * \returns celestial body radius
     **/
    float radius() const;
    /**
     * \brief color getter
     * \author João Vitor Espig (JotaEspig)
     * \returns celestial body color
     **/
    glm::vec3 color() const;
    /**
     * \brief updates the model matrix
     * \author João Vitor Espig (JotaEspig)
     **/
    void update_matrix();
    /**
     * \brief updates the color and calls update_matrix()
     * \author João Vitor Espig (JotaEspig)
     **/
    void update_values();

protected:
    /** Mass **/
    double _mass;
    /** Radius **/
    float _radius = 1.0f;
    /** Color **/
    glm::vec3 _color;
};
