# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
# Configure (out-of-source recommended, or in-tree like existing build/)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
# Output: build/bin/nbody-simulation

# Debug build (enables ASan + UBSan)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)

# Generate Doxygen docs (if Doxygen installed)
cmake --build build --target docs
```

Install the Axolote Engine dependency first if not present:
```bash
chmod +x install_axolote.sh && ./install_axolote.sh
```

## Running

```bash
# Real-time simulation (default: Barnes-Hut + OpenMP)
./build/bin/nbody-simulation config/galaxy.json

# With gravity grid overlay
./build/bin/nbody-simulation config/galaxy.json --grav-grid

# Pre-compute frames to a .baked file
./build/bin/nbody-simulation config/galaxy.json --bake

# Play back baked frames
./build/bin/nbody-simulation config/galaxy.json --render

# Benchmark algorithms
./build/bin/nbody-simulation config/galaxy.json --benchmark
```

Generate custom config files:
```bash
python3 scripts/config_generator.py
```

## Architecture

The simulation runs in one of four modes selected by CLI flags (`--simulate` default, `--bake`, `--render`, `--benchmark`). `App` (`src/app.cpp`) owns a `CelestialBodySystem` and drives the main loop via the Axolote Engine window class.

**Physics pipeline** lives in `CelestialBodySystem` (`src/celestial_body_system.cpp`):
- Loads bodies from JSON config via `setup_using_json()`
- Each frame calls `simulate(dt)` which dispatches to one of three algorithm implementations selected by enum `SimulationAlgorithm`
- Rendering uses **instanced drawing**: positions and colors for all bodies are packed into two VBOs and drawn in a single call via `Sphere`

**Simulation algorithms** (all in `celestial_body_system.cpp`):
- `Naive` — O(n²), every pair
- `BarnesHut` — O(n log n), single-threaded octree
- `BarnesHutOpenMP` — same octree, OpenMP-parallelized force accumulation

**Octree** (`src/octree.cpp`) implements Barnes-Hut spatial partitioning. The static `OcTree::theta` controls the multipole acceptance criterion (s/d < θ). Nodes subdivide into 8 named children (luf/lub/lbf/lbb/ruf/rub/rbf/rbb).

**`CelestialBody`** (`src/celestial_body.cpp`) holds mass, position, velocity, and handles collision/merge logic. Radius is derived as `log2(mass) / 2`.

**`GravitationalGrid`** (`src/gravitational_grid.cpp`) is an optional visualization layer that writes gravity-field displacement data into a GPU SSBO, rendered by `gravgrid_vertex_shader.glsl`.

**Bake mode** serializes each frame's body positions to JSON (compact keys `m`, `px`, `py`, `pz`) and writes a `.baked` file. Render mode reads this file to replay without re-simulating.

## Key constants and parameters

- `G = 6.67e-11` — defined in `include/constants.hpp`
- `OcTree::theta` — static, Barnes-Hut threshold (default ~0.5)
- `dt_multiplier` in config JSON scales the physics timestep
- Octree initial bounds: cube ±1000 units (hardcoded in `octree.cpp`)

## Config JSON format

```json
{
  "dt_multiplier": 1.0,
  "bodies": [
    { "mass": 1e30, "pos": {"x": 0, "y": 0, "z": 0}, "velocity": {"x": 0, "y": 0, "z": 0} }
  ]
}
```

## Code style

A `.clang-format` file is present — run `clang-format -i` on changed files before committing.
