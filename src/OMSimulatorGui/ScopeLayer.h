#pragma once

#include "Application.h"
#include "Layer.h"

#include "OMSimulator/OMSimulator.h"

#include "imgui.h"
#include "tinyfiledialogs.h"

#include <vector>

struct ModelInfo
{
  std::string name;
  std::string path;
  bool selected;
};

const char* modelStateToString(oms_modelState_enu_t modelState)
{
  switch (modelState)
  {
    case oms_modelState_virgin:             return "virgin";
    case oms_modelState_enterInstantiation: return "enterInstantiation";
    case oms_modelState_instantiated:       return "instantiated";
    case oms_modelState_initialization:     return "initialization";
    case oms_modelState_simulation:         return "simulation";
    case oms_modelState_error:              return "error";
  }
  return "unknown";
}

class ScopeLayer : public Layer
{
public:
  ScopeLayer(Application &app) : app(app) {}

  void LoadModel(const std::string &path)
  {
    char *cref;
    if (oms_status_ok == oms_importFile(path.c_str(), &cref))
      models.push_back(ModelInfo{cref, path, false});
  }

  void OnAttach() override
  {
    app.ShowMenubar();
  }

  void OnMenuBar() override
  {
    if (ImGui::BeginMenu("Scope"))
    {
      if (ImGui::MenuItem("Open Model"))
      {
        char const *patterns[] = {"*.ssp"};
        char *wdir;
        oms_getWorkingDirectory(&wdir);
        std::string wdir_str(wdir);
        wdir_str += "/";
        char const *path = tinyfd_openFileDialog("Open Model File", wdir_str.c_str(), 1, patterns, NULL, 0);
        if (path)
          LoadModel(path);
      }
      ImGui::EndMenu();
    }
  }

  void OnUIRender() override
  {
    if (ImGui::Begin("Scope"))
    {
      if (ImGui::BeginTable("split2", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
      {
        for (auto &model : models)
        {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          if (ImGui::Button(model.name.c_str()))
          {
            #if defined(_WIN32)
                  std::string command = "start code \"" + model.path + "\"";
            #else
                  std::string command = "code \"" + model.path + "\"";
            #endif
                  system(command.c_str());
          }
          ImGui::TableNextColumn();
          oms_modelState_enu_t modelState = oms_modelState_error;
          oms_getModelState(model.name.c_str(), &modelState);
          ImGui::Text(modelStateToString(modelState));
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }

private:
  Application &app;

  std::vector<ModelInfo> models;
};
