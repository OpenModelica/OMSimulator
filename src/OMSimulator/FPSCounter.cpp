#include "FPSCounter.h"

#include <chrono>
#include <limits>

FPSCounter::FPSCounter()
{
  reset();
}

void FPSCounter::reset()
{
  frameCount = 0;
  fps = 0.0;
  fastestFrameTime = std::numeric_limits<double>::max();
  slowestFrameTime = 0.0;
  currentFrameTime = 0.0;

  lastTime = std::chrono::high_resolution_clock::now();
  lastReset = lastTime;
  startTime = lastTime;
  currentTime = lastTime;
}

void FPSCounter::update()
{
  frameCount++;
  currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsedTime = currentTime - lastTime;
  std::chrono::duration<double> elapsedTimeReset = currentTime - lastReset;
  lastTime = currentTime;

  // Track frame time for the current frame
  currentFrameTime = elapsedTime.count();

  // Update fastest and slowest frame times
  if (currentFrameTime < fastestFrameTime)
  {
    fastestFrameTime = currentFrameTime;
  }
  if (currentFrameTime > slowestFrameTime)
  {
    slowestFrameTime = currentFrameTime;
  }

  // Update FPS every second
  if (elapsedTimeReset.count() >= 1.0)
  {
    fps = frameCount / elapsedTimeReset.count();
    lastReset = currentTime; // Reset lastReset for the next interval
    frameCount = 0;          // Reset frame count

    slowestFrameTime = currentFrameTime;
    fastestFrameTime = currentFrameTime;
  }
}

double FPSCounter::getTotalTime() const
{
  std::chrono::duration<double> elapsedTime = currentTime - startTime;
  return elapsedTime.count();
}
