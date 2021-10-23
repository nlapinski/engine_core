# engine_core

requires glm, sdl2, imgui (docking-layout custom branch), imgui-node-editor, bgfx (cmake branch)

I have only tested this on mingwin64 on windows. It wont work without alterations on other os.

git clone https://github.com/nlapinski/engine_core.git
git submodule update --init --recursive

cd third-party
./install.sh

cd ..

./configure-ninja
./compile-shaders-win-glsl.bat
./run.sh
