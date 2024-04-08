# nbody-simulation
N-Body simulation using Axolote Engine

## Dependencies
* CMake
* OpenGL
* GLM
* GLFW3
* Doxygen (Optional)
* Axolote Engine. See [https://github.com/JotaEspig/axolote-engine](https://github.com/JotaEspig/axolote-engine) 

## Compiling
```bash
git clone https://github.com/JotaEspig/nbody-simulation.git
cd nbody-simulation
cmake .
make -j4
```
You need to install [Axolote Engine](https://github.com/JotaEspig/axolote-engine) to run it.

## Running
After compiling
```bash
./bin/nbody-simulation <config json file>
```

There are some config files already made inside config folder. But you can create your custom config file using:
```bash
python3 scripts/config_generator.py
```
