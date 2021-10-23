#include "pch.h"
#include "property_panel.h"
#include "imgui_stdlib.h"
#include "imgui_stdlib.cpp"
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


static void run(const char* program,int geo){
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

    int (*const m)(const int in_value) = tcc_get_symbol(s, "m");
    m(geo);
   
    tcc_delete(s);

}

static void ShowPlaceholderObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    ImGui::PushID(uid);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("my sailor is rich");

    if (node_open)
    {
        static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
        for (int i = 0; i < 8; i++)
        {
            ImGui::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowPlaceholderObject("Child", 424242);
            }
            else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                ImGui::TreeNodeEx("Field", flags, "Field_%d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (i >= 5)
                    ImGui::InputFloat("##value", &placeholder_members[i], 1.0f);
                else
                    ImGui::DragFloat("##value", &placeholder_members[i], 0.01f);
                ImGui::NextColumn();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

namespace nui
{

  


  void Property_Panel::render(nui::SceneView* scene_view)
  {
    //  auto mesh = scene_view->get_mesh();
    float f =0;
    ImGui::Begin("Properties");


            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("parms"))
                {
                    ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        // Iterate placeholder objects (all the same data)
        for (int obj_i = 0; obj_i < 4; obj_i++)
        {
            ShowPlaceholderObject("Object", obj_i);
            ImGui::Separator();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("code"))
                {
 
                    // Note: we are using a fixed-sized buffer for simplicity here. See ImGuiInputTextFlags_CallbackResize
                    // and the code in misc/cpp/imgui_stdlib.h for how to setup InputText() for dynamically resizing strings.
                    static char text[1024 * 64] =
                                "#include <tcclib.h>\n"
                                "int m(const int in_value){\n"
                                "   printf(\"this is a test: %d\" , in_value);\n"
                                "}";
                    //std::string cc = *(scene_view->current_code);
                    
                    //
                    //scene_view->current_code = text;
                    //text = *(scene_view->txt);

                   //std::string my_string=scene_view->current_code;
                    static std::string my_string;
                   //my_string->append("0");
                    //std::string s{"foo"};
                   //scene_view->current_code=()
           // bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            //ImGui::InputTextMultiline("##source",  &scene_view->current_code, IM_ARRAYSIZE(&scene_view->current_code), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() *24), flags);
            ImGui::InputTextMultiline("##source", scene_view->current_code, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() *16),flags,NULL,NULL);
    

            //text = scene_view->current_code->c_str();
            //scene_view->current_code=(std::string*)text;
            //scene_view->current_code=(std::string*)text;

         //  printf((const char*)scene_view->current_code);

            if (ImGui::Button("re-compile"))
            {
                run(scene_view->current_code->c_str(),12);
                //... my_code 
            }

            ImGui::TreePop();
             ImGui::Separator();
     

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("extra"))
                {
                    ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                     ImGui::Separator();
                      ImGui::Separator();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::Separator();
            ImGui::TreePop();
  
    //nimgui::draw_vec3_widget("Position", scene_view->get_light()->mPosition, 80.0f);
    //  }

    ImGui::End();

    //mFileDialog.Display();
    //if (mFileDialog.HasSelected())
    //{
    //  auto file_path = mFileDialog.GetSelected().string();
    //  mCurrentFile = file_path.substr(file_path.find_last_of("/\\") + 1);

    //  mMeshLoadCallback(file_path);

    //   mFileDialog.ClearSelected();
    //  }

  }
}
