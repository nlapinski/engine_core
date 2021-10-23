#include "pch.h"
#include "opengl_context.h"

#include "../../bgfx-imgui/imgui_impl_bgfx.h"
#include "../../bgfx/bgfx.h"
#include "../../bgfx/platform.h"
#include "../../bx/math.h"
#include "../../file-ops.h"
#include <imgui.h>
#include "../../sdl-imgui/imgui_impl_sdl.h"



namespace nrender
{

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cube_vertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cube_tri_list[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

static bgfx::ShaderHandle createShader(
    const std::string& shader, const char* name)
{
    const bgfx::Memory* mem = bgfx::copy(shader.data(), shader.size());
    const bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, name);
    return handle;
}


  static void on_key_callback(SDL_Window* window, int key, int scancode, int action, int mods)
  {
   //auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
    //pWindow->on_key(key, scancode, action, mods);
  }

  static void on_scroll_callback(SDL_Window* window, double xoffset, double yoffset)
  {
   // auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
  //  pWindow->on_scroll(yoffset);
  }

  static void on_window_size_callback(SDL_Window* window, int width, int height)
  {
   //auto pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
    //pWindow->on_resize(width, height);
  }

  static void on_window_close_callback(SDL_Window* window)
  {
  //  nwindow::IWindow* pWindow = static_cast<nwindow::IWindow*>(glfwGetWindowUserPointer(window));
  //  pWindow->on_close();
  }

  bool OpenGL_Context::init(nwindow::IWindow* window)
  {
    RenderContext::init(window);

    int width = 1280;
    int height = 720;

    printf("INITALIZED SDL \n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }



  // Decide GL+GLSL versions
  #if __APPLE__
      // GL 3.2 Core + GLSL 150
      const char* glsl_version = "#version 150";
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  #else
      // GL 3.0 + GLSL 130
      const char* glsl_version = "#version 130";
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  #endif

    // and prepare OpenGL stuff
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);







    // check opengl version sdl uses
//    SDL_Log("opengl version: %s", (char*)glGetString(GL_VERSION));



    SDL_Window* SDLwindow = SDL_CreateWindow(
        window->Title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->Width,
        window->Height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);


    window->set_native_window(SDLwindow);


    if (SDLwindow == nullptr) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

#if !BX_PLATFORM_EMSCRIPTEN
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(SDLwindow, &wmi)) {
        printf(
            "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
            SDL_GetError());
        return 1;
    }
    bgfx::renderFrame(); // single threaded mode
#endif // !BX_PLATFORM_EMSCRIPTEN


    bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_OSX
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_LINUX
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?
       // BX_PLATFORM_EMSCRIPTEN
    bgfx::Init bgfx_init;
    //bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    //printf("%c -> ",bgfx::RendererType::Count);
    bgfx_init.type=bgfx::RendererType::OpenGL;
     //bgfx_init.type=bgfx::RendererType::Direct3D12;
    //bgfx_renderer_type_t()
    //bgfx_init.type=BGFX_RENDERER_TYPE_DIRECT3D12;
    bgfx_init.resolution.width = width;
    bgfx_init.resolution.height = height;
    bgfx_init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X16 | BGFX_RESET_MAXANISOTROPY;

;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);


    bgfx::VertexLayout pos_col_vert_layout;
    pos_col_vert_layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
        pos_col_vert_layout);
    ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

    std::string vshader;
    if (!fileops::read_file("shader/v_simple.bin", vshader)) {
        return 1;
    }
    //printf("%c ", vshader);


    std::string fshader;
    if (!fileops::read_file("shader/f_simple.bin", fshader)) {
        return 1;
    }
    //printf("%c ", fshader);


    bgfx::ShaderHandle vsh = createShader(vshader, "vshader");
    bgfx::ShaderHandle fsh = createShader(fshader, "fshader");
    program = bgfx::createProgram(vsh, fsh, true);

bgfx::setDebug(BGFX_DEBUG_STATS);
   //   printf(" CUBE -> %d", cube_tri_list[8]);

    return true;
  }

  void OpenGL_Context::pre_render()
  {
  int width = 1280;
  int height = 720;


//bgfx::setViewFrameBuffer(1, fb1);
//bgfx::setTexture(1, fb0);
//bgfx::submit(0);
//bgfx::setViewFrameBuffer(0);

      // simple input code for orbit camera
    int mouse_x, mouse_y;
    const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    if(ImGui::IsWindowFocused() == true){

    if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
        int delta_x = mouse_x - prev_mouse_x;
        int delta_y = mouse_y - prev_mouse_y;
        cam_yaw += float(-delta_x) * rot_scale;
        cam_pitch += float(-delta_y) * rot_scale;
    }
}
    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;

    //printf("%d -> mouse\n",prev_mouse_x);
    //printf("%d -> mouse\n",prev_mouse_y);

    float cam_rotation[16];
    bx::mtxRotateXYZ(cam_rotation, cam_pitch, cam_yaw, 0.0f);

    float cam_translation[16];
    bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

    float cam_transform[16];
    bx::mtxMul(cam_transform, cam_translation, cam_rotation);

    float view[16];
    bx::mtxInverse(view, cam_transform);

    float proj[16];
    bx::mtxProj(
        proj, 60.0f, float(width) / float(height), 0.1f,
        100.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);

    float model[16];
    bx::mtxIdentity(model);
    bgfx::setTransform(model);

    bgfx::setVertexBuffer(0, vbh);
    bgfx::setIndexBuffer(ibh);

    bgfx::submit(0, program);

   //   printf(" CUBE -> %d", cube_tri_list[8]);

  }

  void OpenGL_Context::post_render()
  {



 //   glfwPollEvents();
//    glfwSwapBuffers((GLFWwindow*) mWindow->get_native_window());
     bgfx::frame();

  }

  void OpenGL_Context::end()
  {

      bgfx::destroy(vbh);
      bgfx::destroy(ibh);
      bgfx::destroy(program);
 
      bgfx::shutdown();
  }
}