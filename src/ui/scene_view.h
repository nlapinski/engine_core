#pragma once

# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui_internal.h>
# include <imgui_node_editor.h>
//#include "elems/camera.h"
//#include "elems/mesh.h"
//#include "elems/light.h"
//#include "shader/shader_util.h"
//#include "render/opengl_buffer_manager.h"
//#include "elems/input.h"

#include <vector.>


namespace nui
{

  struct Node;

  class SceneView
  {
  public:
    SceneView() : 

     Width(1280), Height(720)
    {

       //std::string *current_code = new string("value");
      //std::string val("value"); // initialize a string
      //std::string* current_code = &val; // p points to s
    }

    ~SceneView()
    {

    }

    //static char current_code[1024 * 64];
    std::string val;
    std::string *current_code;
    //static char current_text[1024 * 64];
    static char* txt;
    void NodeEditorInitialize();
    void NodeEditorShow();
    void show();
    void NodeEditorShutdown();

    void resize(int32_t width, int32_t height);


    void render();


    void reset_view()
    {

    }

  private:

    int Width;
    int Height;
    Node* current_node;

  };
}

