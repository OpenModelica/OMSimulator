#pragma once

#include "Application.h"
#include "Layer.h"

#include "imgui.h"

class DemoLayer : public Layer
{
public:
  DemoLayer(Application &app) :app(app) {}

  void OnAttach() override
  {
    app.ShowMenubar();
  }

  void OnMenuBar() override
  {
    if (ImGui::BeginMenu("Demo"))
    {
      if (ImGui::MenuItem(show ? "Hide" : "Show"))
        show = !show;
      ImGui::EndMenu();
    }
  }

  void OnUIRender() override
  {
    if (show)
      ImGui::ShowDemoWindow();
  }

private:
  Application &app;

  bool show = false;
};
