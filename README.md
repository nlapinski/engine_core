# engine_core

requires glm, sdl2, imgui (docking-layout custom branch), imgui-node-editor, bgfx (cmake branch), tinycc (for runtime C compilation)

The third-party cmakelist should pull everything from various repos, build that then build the main program

I have only tested this on mingwin64 on windows. It might work on linux?
There are a few .sh files that could be modified for various cmake commands on other os

Basic setup in a mingw shell:
```
git clone https://github.com/nlapinski/engine_core.git
git submodule update --init --recursive
cd third-party
./install.sh
cd ..
./configure-ninja
./compile-shaders-win-glsl.bat
./run.sh
```
![nodebox](nodebox.png)

I'm still fixing the install cmakes for tinycc - you might need to copy includes/lib to the buld folder from the tinycc build folder in third party