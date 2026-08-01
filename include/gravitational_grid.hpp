#pragma once

#include "axolote/object3d.hpp"

#include "celestial_body.hpp"
#include <vector>

class GravGrid : public axolote::Object3D {
public:
    GLuint ssbo = 0;
    double multiplier_constant = 1;

    GravGrid(std::vector<std::shared_ptr<CelestialBody>> bodies);

    void update_for_body(std::shared_ptr<CelestialBody> c);
    void draw() override;

    friend class CelestialBodySystem;

private:
    std::vector<GLuint> _indices;
    std::vector<float> _displacements;

    /**
     * @brief Calculate a multiplier to scale the gravitational effect on the
     *grid
     *
     * @param max_mass The mass of the most massive body in the system
     * @return double The calculated multiplier
     *
     * The more massive the most massive body is, the smaller the multiplier
     * should be.
     * The formula is:
     * multiplier = A * exp(-max_mass / K) + 1
     * Where A and K are constants that can be adjusted to fit the needs of
     * the simulation.
     **/
    double calculate_multiplier(double max_mass);
};
