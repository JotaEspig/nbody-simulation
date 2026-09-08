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
     * \brief Treat the collision between two celestial bodies
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     *
     * If the relative approach speed is below the mutual escape velocity,
     * the two bodies are gravitationally bound at contact and are merged
     * (perfectly inelastic, conserving mass and momentum). Otherwise, an
     * impulse is applied that conserves momentum exactly, with a
     * restitution derived from how much the impact speed exceeds the
     * mutual escape velocity (see mutual_escape_velocity()).
     **/
    void collide(std::shared_ptr<CelestialBody> other);
    /**
     * \brief Checks if two celestial bodies should merge
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     * \returns true if the relative approach speed is at or below the
     * mutual escape velocity of the two bodies (i.e. they are
     * gravitationally bound and cannot separate again)
     **/
    bool should_merge(std::shared_ptr<CelestialBody> other) const;
    /**
     * \brief Computes the mutual (two-body) escape velocity
     * \author João Vitor Espig (JotaEspig)
     * \param other - other celestial body
     * \returns sqrt(2 * G * (m1 + m2) / (r1 + r2))
     **/
    double mutual_escape_velocity(const CelestialBody &other) const;
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
     * \returns celestial body radius (visual/rendering scale, log-compressed
     * so that huge mass ratios don't dominate the screen)
     **/
    float radius() const;
    /**
     * \brief physical collision radius getter
     * \author João Vitor Espig (JotaEspig)
     * \returns celestial body's physical radius used for collision
     * detection and escape-velocity calculations, assuming constant
     * density (radius ~ mass^(1/3)). Kept separate from radius() because
     * that one is a log-compressed visual scale unrelated to actual
     * physical size -- reusing it for collisions would make low-mass
     * bodies (e.g. dust) have wildly oversized collision cross-sections.
     **/
    float collision_radius() const;
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
    /** Radius (visual/rendering scale) **/
    float _radius = 1.0f;
    /** Physical collision radius (mass^(1/3) scale) **/
    float _collision_radius = 1.0f;
    /** Color **/
    glm::vec3 _color;
};
