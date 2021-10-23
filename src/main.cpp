#define BGFX_CONFIG_RENDERER_OPENGL 41
#define BGFX_RENDERER_OPENGL_NAME "OpenGL 4.1"

#include "SDL.h"
#include "SDL_syswm.h"
#include "../bgfx-imgui/imgui_impl_bgfx.h"
#include "../bgfx/bgfx.h"
#include "../bgfx/platform.h"
#include "../bx/math.h"


#include "../sdl-imgui/imgui_impl_sdl.h"



const char* program = \
    "#include <tcclib.h>"
    "int foo(const int in_value){"
    "   printf(\"this is a test: %d\n\" , in_value);"
    "}";

int main(int argc, char *argv[])
{

 


	return 0;
}


