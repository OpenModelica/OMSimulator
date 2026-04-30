/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#pragma once

#include "Application.h"
#include "Layer.h"

#include "OMSimulator/OMSimulator.h"

#include "imgui.h"
#include "tinyfiledialogs.h"

#include <vector>

#include "Scope.h"
#include "Model.h"

struct ModelInfo
{
  std::string name;
  std::string path;
  bool selected;
};

const char *modelStateToString(oms_modelState_enu_t modelState)
{
  switch (modelState)
  {
  case oms_modelState_virgin:
    return "virgin";
  case oms_modelState_enterInstantiation:
    return "enterInstantiation";
  case oms_modelState_instantiated:
    return "instantiated";
  case oms_modelState_initialization:
    return "initialization";
  case oms_modelState_simulation:
    return "simulation";
  case oms_modelState_error:
    return "error";
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
            oms::Model *oms_model = oms::Scope::GetInstance().getModel(model.name);
            if (oms_model)
            {
              std::string command = "code -n \"" + oms_model->getTempDirectory() + "\"";
              system(command.c_str());
            }
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
