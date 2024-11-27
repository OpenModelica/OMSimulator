#pragma once

class Layer
{
public:
  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnDetach() {}

  virtual void OnMenuBar() {}
  virtual void OnUpdate(float ts) {}
  virtual void OnUIRender() {}
};
