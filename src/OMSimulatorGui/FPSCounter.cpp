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
