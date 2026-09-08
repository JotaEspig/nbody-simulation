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
/** Scale constant for the physical collision radius (radius = k *
 * mass^(1/3), assuming constant density). Calibrated so a mass of 50 --
 * the old fixed merge threshold -- gets roughly the same collision radius
 * the old log-based radius gave it, keeping already-tuned massive-body
 * collisions roughly unchanged while shrinking low-mass (dust-like) bodies
 * to a physically-realistic, much smaller cross-section. **/
#define COLLISION_RADIUS_CONST 0.766
/** Minimum physical collision radius, avoids a zero/near-zero radius sum
 * in mutual_escape_velocity() for extremely low-mass bodies. **/
#define COLLISION_RADIUS_MIN 1e-3
/** Minimum mass for the non-merging impulse branch of collide(). Below
 * this, 1/mass would blow up to infinity/NaN, so such bodies always merge
 * instead (well below any real body's mass in the shipped configs, e.g.
 * config/shuriken.json's dust particles are 1e-5). **/
#define MIN_MASS 1e-9
