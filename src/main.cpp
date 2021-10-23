#define BGFX_CONFIG_RENDERER_OPENGL 41
#define BGFX_RENDERER_OPENGL_NAME "OpenGL 4.1"

#include "SDL.h"
#include "SDL_syswm.h"
#include "../bgfx-imgui/imgui_impl_bgfx.h"
#include "../bgfx/bgfx.h"
#include "../bgfx/platform.h"
#include "../bx/math.h"
#include "../file-ops.h"

#include "../sdl-imgui/imgui_impl_sdl.h"

#include "application.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <libtcc.h>

#ifdef __cplusplus
}
#endif

const char* program = \
    "#include <tcclib.h>"
    "int foo(const int in_value){"
    "   printf(\"this is a test: %d\n\" , in_value);"
    "}";

int main(int argc, char *argv[])
{

    TCCState* s = tcc_new();
    if(!s){
        printf("Can’t create a TCC context\n");
        return 1;
    }
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
   
    if (tcc_compile_string(s, program) > 0) {
        printf("Compilation error !\n");
        return 2;
    }

    tcc_relocate(s, TCC_RELOCATE_AUTO);

    int (*const foo)(const int in_value) = tcc_get_symbol(s, "foo");
    foo(32);
   
    tcc_delete(s);

	auto app = std::make_unique<Application>("node_box");
	app->loop();

	printf("END");


	return 0;
}


