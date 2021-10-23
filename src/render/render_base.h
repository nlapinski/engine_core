#pragma once

//#include "elems/vertex_holder.h"

#include "window/window.h"

namespace nrender
{



  class RenderContext
  {

  public:

    RenderContext() : mWindow(nullptr) {}

    virtual bool init(nwindow::IWindow* window)
    {
      mWindow = window;
      return true;
    }

    virtual void pre_render() = 0;

    virtual void post_render() = 0;

    virtual void end() = 0;

  protected:
    nwindow::IWindow* mWindow;
  };
}
