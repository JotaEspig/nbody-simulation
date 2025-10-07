pwd=$(pwd)
git clone -b v0.10.0 https://github.com/JotaEspig/axolote-engine.git --depth=1
cd axolote-engine
mkdir build
cd build
cmake ..
cmake --build .
cmake --install . --prefix=$HOME/.local
cd $pwd
