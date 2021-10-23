#pragma once


#include "SDL.h"
#include "SDL_syswm.h"
#include "../bgfx-imgui/imgui_impl_bgfx.h"
#include "../bgfx/bgfx.h"
#include "../bgfx/platform.h"
#include "../bx/math.h"
#include "../file-ops.h"
#include <imgui.h>
#include "../sdl-imgui/imgui_impl_sdl.h"
# include <imgui_node_editor.h>


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

// Std includes
#include <string>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
