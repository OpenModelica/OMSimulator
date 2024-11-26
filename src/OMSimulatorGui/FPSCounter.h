#pragma once

#include <chrono>

class FPSCounter
{
public:
  FPSCounter();
  void update();

  void reset();

  double getFPS() const
  {
    return fps;
  }

  double getFastestFrameTime() const
  {
    return fastestFrameTime;
  }

  double getSlowestFrameTime() const
  {
    return slowestFrameTime;
  }

  double getCurrentFrameTime() const
  {
    return currentFrameTime;
  }

  double getTotalTime() const;

private:
  uint64_t frameCount;                                                   // Number of frames counted
  double fps;                                                            // Frames per second
  double fastestFrameTime;                                               // Fastest frame time in seconds
  double slowestFrameTime;                                               // Slowest frame time in seconds
  double currentFrameTime;                                               // Current frame time in seconds
  std::chrono::time_point<std::chrono::high_resolution_clock> lastReset; // Last reset time
  std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;  // Last update time
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime; // Initial time
  std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
};
