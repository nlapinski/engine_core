#pragma once

#include "render_base.h"

namespace nrender
{


  
  class OpenGL_Context : public RenderContext
  {
  public:

    bool init(nwindow::IWindow* window) override;

    void pre_render() override;

    void post_render() override;

    void end() override;

private:
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

    float cam_pitch = 0.0f;
    float cam_yaw = 0.0f;
    float rot_scale = 0.01f;

    int prev_mouse_x = 0;
    int prev_mouse_y = 0;


  };
}

