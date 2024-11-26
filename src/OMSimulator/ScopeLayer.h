#pragma once

#include "Application.h"
#include "Layer.h"

#include "OMSimulator/OMSimulator.h"

#include "imgui.h"
#include "tinyfiledialogs.h"

#include <unordered_set>

class ScopeLayer : public Layer
{
public:
  ScopeLayer(Application &app) : app(app) {}

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
        {
          char *cref;
          if (oms_status_ok == oms_importFile(path, &cref))
            models.insert(cref);
        }
      }
      ImGui::EndMenu();
    }
  }

  void OnUIRender() override
  {
    if (ImGui::Begin("Scope"))
    {
      for (const auto &model : models)
      {
        ImGui::Text("%s", model.c_str());
      }
      ImGui::End();
    }
  }

private:
  Application &app;

  std::unordered_set<std::string> models;
};
