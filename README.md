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
git checkout binary-stars
cmake .
make -j4
```
You need to install [Axolote Engine](https://github.com/JotaEspig/axolote-engine) to run it.
```bash
chmod +x install_axolote.sh
./install_axolote.sh
```

## Running
After compiling
```bash
./bin/nbody-simulation config/<configuration_file>
```
or

## Theory
A binary star system is a system of 2 stars that are gravitationally bound to
and in orbit around each other.

In `balanced.json` we have a configuration where the 2 stars have the same mass
and the same orbital speed. In this case they are balanced and their trail makes
a perfect circle orbit.

The masses are arbitrary and the orbital speed was calculated using this
thinking process:
<div align="center">
    <img alt="binary star drawing" src="https://iwant2study.org/lookangejss/02_newtonianmechanics_7gravity/ejss_model_gravity02_1/TwoBodyOrbits/2015-02-11_1404.png">
</div>

To calculate the orbital speed, we have to consider the point $O$ as a
"massive object" where our stars will be orbiting.
$$v = \sqrt{ \frac{GM_o}{r} }$$
So, to know $M_o$ we can try to find the mass for the point $O$ where it
generates the same amount of force than the opposite star:
$$\frac{GM_cM}{r^2} = \frac{GMM}{(2r)^2}$$
where $M$ is the stars masses. Simplifying:
$$M_c = \frac{M}{4}$$
So, the orbital speed of the stars $A$ and $B$ should be: $v = \sqrt{ \frac{GM}{4r} }$
