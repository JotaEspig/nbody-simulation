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
/** Maximum coefficient of restitution for non-merging collisions. Real
 * rocky/icy bodies dissipate a large fraction of impact energy as heat and
 * deformation even well above escape velocity, so this stays well below 1
 * (a perfectly elastic bounce), matching measured restitution values for
 * hypervelocity rock/ice impacts. **/
#define MAX_RESTITUTION 0.5
