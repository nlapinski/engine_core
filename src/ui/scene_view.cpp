#include "pch.h"
#include "scene_view.h"
#include <imgui.h>

#include <SDL_keycode.h>
#include <SDL_timer.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <imgui_node_editor.h>
#include <ax/Math2D.h>
#include <ax/Builders.h>
#include <ax/Widgets.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>




namespace nui
{




  static inline ImRect ImGui_GetItemRect()
  {
      return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
  }

  static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
  {
      auto result = rect;
      result.Min.x -= x;
      result.Min.y -= y;
      result.Max.x += x;
      result.Max.y += y;
      return result;
  }

  namespace ed = ax::NodeEditor;
  namespace util = ax::NodeEditor::Utilities;

  using namespace ax;

  using ax::Widgets::IconType;

  static ed::EditorContext* m_Editor = nullptr;


  enum class PinType
  {
      Flow,
      Bool,
      Int,
      Float,
      String,
      Object,
      Function,
      Delegate,
  };

  enum class PinKind
  {
      Output,
      Input
  };

  enum class NodeType
  {
      Blueprint,
      Simple,
      Tree,
      Comment,
      Houdini
  };

  struct Node;

  struct Pin
  {
      ed::PinId   ID;
      //::Node*     Node;
      Node* Node;
      std::string Name;
      PinType     Type;
      PinKind     Kind;

      Pin(int id, const char* name, PinType type):
          ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
      {
      }
  };

  struct Node
  {
      ed::NodeId ID;
      std::string Name;
      std::vector<Pin> Inputs;
      std::vector<Pin> Outputs;
      ImColor Color;
      NodeType Type;
      ImVec2 Size;

      std::string State;
      std::string SavedState;
      std::string Code;

      Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
          ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0),Code("code")
      {
      }
  };

  struct Link
  {
      ed::LinkId ID;

      ed::PinId StartPinID;
      ed::PinId EndPinID;

      ImColor Color;

      Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
          ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
      {
      }
  };


  static const int            s_PinIconSize = 24;
  static std::vector<Node>    s_Nodes;
  static std::vector<Link>    s_Links;
  static ImTextureID          s_HeaderBackground = nullptr;
  //static ImTextureID          s_SampleImage = nullptr;
  static ImTextureID          s_SaveIcon = nullptr;
  static ImTextureID          s_RestoreIcon = nullptr;

  struct NodeIdLess
  {
      bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
      {
          return lhs.AsPointer() < rhs.AsPointer();
      }
  };

  static const float          s_TouchTime = 1.0f;
  static std::map<ed::NodeId, float, NodeIdLess> s_NodeTouchTime;

  static int s_NextId = 1;
  static int GetNextId()
  {
      return s_NextId++;
  }

  static ed::LinkId GetNextLinkId()
  {
      return ed::LinkId(GetNextId());
  }

  static void TouchNode(ed::NodeId id)
  {
      s_NodeTouchTime[id] = s_TouchTime;
  }

  static float GetTouchProgress(ed::NodeId id)
  {
      auto it = s_NodeTouchTime.find(id);
      if (it != s_NodeTouchTime.end() && it->second > 0.0f)
          return (s_TouchTime - it->second) / s_TouchTime;
      else
          return 0.0f;
  }

  static void UpdateTouch()
  {
      const auto deltaTime = ImGui::GetIO().DeltaTime;
      for (auto& entry : s_NodeTouchTime)
      {
          if (entry.second > 0.0f)
              entry.second -= deltaTime;
      }
  }

  static Node* FindNode(ed::NodeId id)
  {
      for (auto& node : s_Nodes)
          if (node.ID == id)
              return &node;

      return nullptr;
  }

  static Link* FindLink(ed::LinkId id)
  {
      for (auto& link : s_Links)
          if (link.ID == id)
              return &link;

      return nullptr;
  }

  static Pin* FindPin(ed::PinId id)
  {
      if (!id)
          return nullptr;

      for (auto& node : s_Nodes)
      {
          for (auto& pin : node.Inputs)
              if (pin.ID == id)
                  return &pin;

          for (auto& pin : node.Outputs)
              if (pin.ID == id)
                  return &pin;
      }

      return nullptr;
  }

  static bool IsPinLinked(ed::PinId id)
  {
      if (!id)
          return false;

      for (auto& link : s_Links)
          if (link.StartPinID == id || link.EndPinID == id)
              return true;

      return false;
  }

  static bool CanCreateLink(Pin* a, Pin* b)
  {
      if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
          return false;

      return true;
  }


  static void BuildNode(Node* node)
  {
      for (auto& input : node->Inputs)
      {
          input.Node = node;
          input.Kind = PinKind::Input;
      }

      for (auto& output : node->Outputs)
      {
          output.Node = node;
          output.Kind = PinKind::Output;
      }
  }




  static Node* SpawnComment()
  {
      s_Nodes.emplace_back(GetNextId(), "Test Comment");
      s_Nodes.back().Type = NodeType::Comment;
      s_Nodes.back().Size = ImVec2(300, 200);

      return &s_Nodes.back();
  }

  static Node* SpawnHoudiniTransformNode()
  {
      s_Nodes.emplace_back(GetNextId(), "Transform");

      s_Nodes.back().Type = NodeType::Houdini;
     s_Nodes.back().Code =                 "#include <tcclib.h>\n"
                                "int m(const int in_value){\n"
                                "   printf(\"this is a test: %d\" , in_value);\n"
                                "}";



      s_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
      s_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

      BuildNode(&s_Nodes.back());

      return &s_Nodes.back();
  }

  static Node* SpawnHoudiniGroupNode()
  {
      s_Nodes.emplace_back(GetNextId(), "Group");
      s_Nodes.back().Type = NodeType::Houdini;
     s_Nodes.back().Code =                 "#include <tcclib.h>\n"
                                "int m(const int in_value){\n"
                                "   printf(\"this is a test: %d\" , in_value);\n"
                                "}";


      s_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
      s_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
      s_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

      BuildNode(&s_Nodes.back());

      return &s_Nodes.back();
  }


  void BuildNodes()
  {
      for (auto& node : s_Nodes)
          BuildNode(&node);
  }

  void graph_init(){
      ed::Config config;

      //config.NavigateButtonIndex=2;

      config.SettingsFile = "Blueprints.json";

      config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
      {
          auto node = FindNode(nodeId);
          if (!node)
              return 0;

          if (data != nullptr)
              memcpy(data, node->State.data(), node->State.size());
          return node->State.size();
      };

      config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
      {
          auto node = FindNode(nodeId);
          if (!node)
              return false;

          node->State.assign(data, size);

          TouchNode(nodeId);

          return true;
      };

      m_Editor = ed::CreateEditor(&config);

      ed::SetCurrentEditor(m_Editor);
      ed::EnableShortcuts(true);
      //ed::EditorContext::EnableShortcuts(true);

      Node* node;

      node = SpawnHoudiniTransformNode(); ed::SetNodePosition(node->ID, ImVec2(500, -40));
      node = SpawnHoudiniGroupNode();     ed::SetNodePosition(node->ID, ImVec2(500, 2));

      node = SpawnHoudiniTransformNode(); ed::SetNodePosition(node->ID, ImVec2(400, -70));
      node = SpawnHoudiniGroupNode();     ed::SetNodePosition(node->ID, ImVec2(300, 42));



      ed::NavigateToContent();

      BuildNodes();

      auto& io = ImGui::GetIO();

  }

  void graph_end(){
      if (m_Editor)
      {
          ed::DestroyEditor(m_Editor);
          m_Editor = nullptr;
      }
  }



  ImColor GetIconColor(PinType type)
  {
      switch (type)
      {
          default:
          case PinType::Flow:     return ImColor(255, 255, 255);
          case PinType::Bool:     return ImColor(220,  48,  48);
          case PinType::Int:      return ImColor( 68, 201, 156);
          case PinType::Float:    return ImColor(147, 226,  74);
          case PinType::String:   return ImColor(124,  21, 153);
          case PinType::Object:   return ImColor( 51, 150, 215);
          case PinType::Function: return ImColor(218,   0, 183);
          case PinType::Delegate: return ImColor(255,  48,  48);
      }
  };

  void DrawPinIcon(const Pin& pin, bool connected, int alpha)
  {
      IconType iconType;
      ImColor  color = GetIconColor(pin.Type);
      color.Value.w = alpha / 255.0f;
      switch (pin.Type)
      {
          case PinType::Flow:     iconType = IconType::Flow;   break;
          case PinType::Bool:     iconType = IconType::Circle; break;
          case PinType::Int:      iconType = IconType::Circle; break;
          case PinType::Float:    iconType = IconType::Circle; break;
          case PinType::String:   iconType = IconType::Circle; break;
          case PinType::Object:   iconType = IconType::Circle; break;
          case PinType::Function: iconType = IconType::Circle; break;
          case PinType::Delegate: iconType = IconType::Square; break;
          default:
              return;
      }

      ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
  };



  Node* draw_frame(int w, int h){
       UpdateTouch();

      auto& io = ImGui::GetIO();
      ed::SetCurrentEditor(m_Editor);

      static ed::NodeId contextNodeId      = 0;
      static ed::LinkId contextLinkId      = 0;
      static ed::PinId  contextPinId       = 0;
      static bool createNewNode  = false;
      static Pin* newNodeLinkPin = nullptr;
      static Pin* newLinkPin     = nullptr;

      

      std::vector<ed::NodeId> selectedNodes;
      std::vector<ed::LinkId> selectedLinks;
      selectedNodes.resize(ed::GetSelectedObjectCount());
      selectedLinks.resize(ed::GetSelectedObjectCount());

      int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
      int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

      selectedNodes.resize(nodeCount);
      selectedLinks.resize(linkCount);

      Node* current;

      std::string status="no node selected";
          for (auto& node : s_Nodes)
          {

              //grabs selected node
              bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node.ID) != selectedNodes.end();
              if(isSelected){
                  auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) + ")";
                  //printf("node -> %s \n",id.c_str());
                  //printf("node -> %s \n",node.Name.c_str());
                  //printf("node -> %s \n",node.code.c_str());
                  current=&node;
                  status = "current id->"+id+"current type->"+node.Name;
                  
              }



          }

          ImGui::TextUnformatted(status.c_str());



      ed::Begin("Node editor");
      {
          auto cursorTopLeft = ImGui::GetCursorScreenPos();

          util::BlueprintNodeBuilder builder;





          for (auto& node : s_Nodes)
          {
              if (node.Type != NodeType::Houdini)
                  continue;

              const float rounding = 10.0f;
              const float padding  = 12.0f;
              const float padding_inside  = 4.0f;

              ed::PushStyleColor(ed::StyleColor_NodeBg,        ImColor(229, 229, 229, 200));
              ed::PushStyleColor(ed::StyleColor_NodeBorder,    ImColor(125, 125, 125, 200));
              ed::PushStyleColor(ed::StyleColor_PinRect,       ImColor(229, 229, 229, 60));
              ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

              const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

              ed::PushStyleVar(ed::StyleVar_NodePadding,  ImVec4(0, 0, 0, 0));
              ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
              ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f,  1.0f));
              ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
              ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
              ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
              ed::PushStyleVar(ed::StyleVar_PinRadius, 8.0f);
              ed::BeginNode(node.ID);

              ImGui::BeginVertical(node.ID.AsPointer());
              if (!node.Inputs.empty())
              {
                  ImGui::BeginHorizontal("inputs");
                  ImGui::Spring(1, 0);

                  ImRect inputsRect;
                  int inputAlpha = 200;
                  for (auto& pin : node.Inputs)
                  {
                      ImGui::Dummy(ImVec2(padding*1.25, padding*1.25));
                      inputsRect = ImGui_GetItemRect();
                      ImGui::Spring(1, 0);
                      inputsRect.Min.y -= padding;
                      inputsRect.Max.y -= padding;


                      ed::PushStyleVar(ed::StyleVar_PinCorners, 15);
                      ed::BeginPin(pin.ID, ed::PinKind::Input);
                      ed::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
                      ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                      ed::EndPin();
                      //ed::PopStyleVar(3);
                      ed::PopStyleVar(1);

                      auto drawList = ImGui::GetWindowDrawList();
                      drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
                          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);
                      drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);

                      if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                          inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                  }

                  //ImGui::Spring(1, 0);
                  ImGui::EndHorizontal();
              }

              ImGui::SetWindowFontScale(1.25);

              ImGui::BeginHorizontal("content_frame");
              ImGui::Spring(1, padding_inside);

              ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
              ImGui::Dummy(ImVec2(160, 0));
              ImGui::Spring(1);
              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));


              ImGui::TextUnformatted(node.Name.c_str());
              ImGui::PopStyleColor();
              ImGui::Spring(1);
              ImGui::EndVertical();
              auto contentRect = ImGui_GetItemRect();

              ImGui::Spring(1, padding_inside);
              ImGui::EndHorizontal();

              if (!node.Outputs.empty())
              {
                  ImGui::BeginHorizontal("outputs");
                  ImGui::Spring(1, 0);

                  ImRect outputsRect;
                  int outputAlpha = 200;
                  for (auto& pin : node.Outputs)
                  {
                      ImGui::Dummy(ImVec2(padding*1.25, padding*1.25));
                      outputsRect = ImGui_GetItemRect();
                      ImGui::Spring(1, 0);
                      outputsRect.Min.y += padding;
                      outputsRect.Max.y += padding;

                      ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
                      ed::BeginPin(pin.ID, ed::PinKind::Output);
                      ed::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
                      ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                      ed::EndPin();
                      ed::PopStyleVar();

                      auto drawList = ImGui::GetWindowDrawList();
                      drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
                          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);
                      drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);


                      if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                          outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                  }

                  ImGui::EndHorizontal();
              }

              ImGui::EndVertical();

              ed::EndNode();
              ed::PopStyleVar(7);
              ed::PopStyleColor(4);

              auto drawList = ed::GetNodeBackgroundDrawList(node.ID);
          }

     

          for (auto& link : s_Links)
              ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

          if (!createNewNode)
          {
              if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
              {
                  auto showLabel = [](const char* label, ImColor color)
                  {
                      ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                      auto size = ImGui::CalcTextSize(label);

                      auto padding = ImGui::GetStyle().FramePadding;
                      auto spacing = ImGui::GetStyle().ItemSpacing;

                      ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                      auto rectMin = ImGui::GetCursorScreenPos() - padding;
                      auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                      auto drawList = ImGui::GetWindowDrawList();
                      drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                      ImGui::TextUnformatted(label);
                  };

                  ed::PinId startPinId = 0, endPinId = 0;
                  if (ed::QueryNewLink(&startPinId, &endPinId))
                  {
                      auto startPin = FindPin(startPinId);
                      auto endPin   = FindPin(endPinId);

                      newLinkPin = startPin ? startPin : endPin;

                      if (startPin->Kind == PinKind::Input)
                      {
                          std::swap(startPin, endPin);
                          std::swap(startPinId, endPinId);
                      }

                      if (startPin && endPin)
                      {
                          if (endPin == startPin)
                          {
                              ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                          }
                          else if (endPin->Kind == startPin->Kind)
                          {
                              showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                              ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                          }
                          //else if (endPin->Node == startPin->Node)
                          //{
                          //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                          //    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                          //}
                          else if (endPin->Type != startPin->Type)
                          {
                              showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                              ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                          }
                          else
                          {
                              showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                              if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                              {
                                  s_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
                                  s_Links.back().Color = GetIconColor(startPin->Type);
                              }
                          }
                      }
                  }

                  ed::PinId pinId = 0;
                  if (ed::QueryNewNode(&pinId))
                  {
                      newLinkPin = FindPin(pinId);
                      if (newLinkPin)
                          showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                      if (ed::AcceptNewItem())
                      {
                          createNewNode  = true;
                          newNodeLinkPin = FindPin(pinId);
                          newLinkPin = nullptr;
                          ed::Suspend();
                          ImGui::OpenPopup("Create New Node");
                          ed::Resume();
                      }
                  }
              }
              else
                  newLinkPin = nullptr;

              ed::EndCreate();

              if (ed::BeginDelete())
              {
                  ed::LinkId linkId = 0;
                  while (ed::QueryDeletedLink(&linkId))
                  {
                      if (ed::AcceptDeletedItem())
                      {
                          auto id = std::find_if(s_Links.begin(), s_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
                          if (id != s_Links.end())
                              s_Links.erase(id);
                      }
                  }

                  ed::NodeId nodeId = 0;
                  while (ed::QueryDeletedNode(&nodeId))
                  {
                      if (ed::AcceptDeletedItem())
                      {
                          auto id = std::find_if(s_Nodes.begin(), s_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
                          if (id != s_Nodes.end())
                              s_Nodes.erase(id);
                      }
                  }
              }
              ed::EndDelete();
          }

          ImGui::SetCursorScreenPos(cursorTopLeft);
      }

  # if 1
      auto openPopupPosition = ImGui::GetMousePos();
      ed::Suspend();
      if (ed::ShowNodeContextMenu(&contextNodeId))
          ImGui::OpenPopup("Node Context Menu");
      else if (ed::ShowPinContextMenu(&contextPinId))
          ImGui::OpenPopup("Pin Context Menu");
      else if (ed::ShowLinkContextMenu(&contextLinkId))
          ImGui::OpenPopup("Link Context Menu");
      else if (ed::ShowBackgroundContextMenu())
      {
          ImGui::OpenPopup("Create New Node");
          newNodeLinkPin = nullptr;
      }
      ed::Resume();

      ed::Suspend();
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
      if (ImGui::BeginPopup("Node Context Menu"))
      {
          auto node = FindNode(contextNodeId);

          ImGui::TextUnformatted("Node Context Menu");
          ImGui::Separator();
          if (node)
          {
              ImGui::Text("ID: %p", node->ID.AsPointer());
              ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
              ImGui::Text("Inputs: %d", (int)node->Inputs.size());
              ImGui::Text("Outputs: %d", (int)node->Outputs.size());
          }
          else
              ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
          ImGui::Separator();
          if (ImGui::MenuItem("Delete"))
              ed::DeleteNode(contextNodeId);
          ImGui::EndPopup();
      }

      if (ImGui::BeginPopup("Pin Context Menu"))
      {
          auto pin = FindPin(contextPinId);

          ImGui::TextUnformatted("Pin Context Menu");
          ImGui::Separator();
          if (pin)
          {
              ImGui::Text("ID: %p", pin->ID.AsPointer());
              if (pin->Node)
                  ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
              else
                  ImGui::Text("Node: %s", "<none>");
          }
          else
              ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

          ImGui::EndPopup();
      }

      if (ImGui::BeginPopup("Link Context Menu"))
      {
          auto link = FindLink(contextLinkId);

          ImGui::TextUnformatted("Link Context Menu");
          ImGui::Separator();
          if (link)
          {
              ImGui::Text("ID: %p", link->ID.AsPointer());
              ImGui::Text("From: %p", link->StartPinID.AsPointer());
              ImGui::Text("To: %p", link->EndPinID.AsPointer());
          }
          else
              ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
          ImGui::Separator();
          if (ImGui::MenuItem("Delete"))
              ed::DeleteLink(contextLinkId);
          ImGui::EndPopup();
      }

      if (ImGui::BeginPopup("Create New Node"))
      {
          auto newNodePostion = openPopupPosition;
          
          Node* node = nullptr;
     
          if (ImGui::MenuItem("Transform"))
              node = SpawnHoudiniTransformNode();
          if (ImGui::MenuItem("Group"))
              node = SpawnHoudiniGroupNode();

          if (node)
          {
              BuildNodes();

              createNewNode = false;

              ed::SetNodePosition(node->ID, newNodePostion);

              if (auto startPin = newNodeLinkPin)
              {
                  auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                  for (auto& pin : pins)
                  {
                      if (CanCreateLink(startPin, &pin))
                      {
                          auto endPin = &pin;
                          if (startPin->Kind == PinKind::Input)
                              std::swap(startPin, endPin);

                          s_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                          s_Links.back().Color = GetIconColor(startPin->Type);

                          break;
                      }
                  }
              }
          }

          ImGui::EndPopup();
      }
      else{
          createNewNode = false;}
      ImGui::PopStyleVar();
      ed::Resume();
  # endif




  ed::End();

return current;

  }


  void SceneView::resize(int32_t width, int32_t height)
  {
    Width = width;
    Height = height;


  }



  void SceneView::show()
  {




  }


  void SceneView::render()
  {


    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    Width = viewportPanelSize.x;
    Height = viewportPanelSize.y;

    auto& io = ImGui::GetIO();

    static std::string blank="no node selected";
    ImGui::Begin("Graph");
    //std::string *code;
    //std::string val("value"); // initialize a string
    //std::string* current_code = &val; // p points to s
    current_node = draw_frame(Width,Height);
    if(current_node != NULL){
      current_code =&(current_node->Code);
    }
    else{
      current_code = &blank;
    }
   // printf(*(current_node->Name));
    //printf((current_code)->c_str());
   // current_code=code;


    ImGui::End();

  }


  void SceneView::NodeEditorInitialize()
  {
    graph_init();
  }

  void SceneView::NodeEditorShow()
  {

     

  }

  void SceneView::NodeEditorShutdown()
  {

    if (m_Editor)
    {
        ed::DestroyEditor(m_Editor);
        m_Editor = nullptr;
    }
  }
}
