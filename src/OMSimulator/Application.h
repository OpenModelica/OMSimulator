#pragma once

#include "Layer.h"
#include "FPSCounter.h"

#include <memory>
#include <vector>
#include <string>

class Application
{
public:
  Application(const std::string &name, uint32_t width, uint32_t height);
  ~Application();

  void ShowMenubar() { show_menubar = true; }
  void PushLayer(const std::shared_ptr<Layer> &layer);

  void Run();

  void Close() { running = false; }

private:
  void Init();
  void Shutdown();

private:
  std::string name;
  uint32_t width;
  uint32_t height;

  std::vector<std::shared_ptr<Layer>> layer_stack;
  FPSCounter fps;

  bool show_menubar = false;
  bool running = false;
};
