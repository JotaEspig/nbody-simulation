/**
 * \file constants.hpp
 * \brief Constants used in the simulation
 * \author João Vitor Espig (JotaEspig)
 **/
#pragma once

/** Gravitational constant **/
#define G 6.67e-11
/** Distance epsilon used to avoid normalizing a zero-length vector in
 * collisions **/
#define COLLISION_EPS 1e-6
/** Maximum octree recursion depth, guards against unbounded split() recursion
 * **/
#define MAX_OCTREE_DEPTH 40
