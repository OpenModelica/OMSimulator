/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "Clocks.h"
#include "Clock.h"
#include "Util.h"
#include "Logging.h"

#include <string>

Clocks globalClocks(GLOBALCLOCK_MAX_INDEX);

const char* GlobalClockNames[GLOBALCLOCK_MAX_INDEX] = {
  /* GLOBALCLOCK_IDLE */           "idle",
  /* GLOBALCLOCK_INSTANTIATION */  "instantiation",
  /* GLOBALCLOCK_INITIALIZATION */ "initialization",
  /* GLOBALCLOCK_SIMULATION */     "simulation",
  /* GLOBALCLOCK_COMMUNICATION */  "communication",
  /* GLOBALCLOCK_RESULTFILE */     "result file"
};

Clocks::Clocks(int numSubClocks)
  : numSubClocks(numSubClocks)
{
  clocks = new Clock[numSubClocks];

  for (int i = 0; i<numSubClocks; ++i)
   clocks[i].reset();

  activeClocks.push(0);
  clocks[0].tic();
}

Clocks::~Clocks()
{
  if (activeClocks.size() > 1 || activeClocks.top() != 0)
    logFatal("Time measurement is corrupted.");

  delete[] clocks;
}

void Clocks::tic(int index)
{
  int activeClock = activeClocks.top();

  if (activeClock == index)
  {
    activeClocks.push(index);
    return;
  }

  clocks[activeClock].toc();
  clocks[index].tic();
  activeClocks.push(index);
}

void Clocks::toc(int index)
{
  int activeClock = activeClocks.top();

  if (activeClock != index)
    logFatal("Time measurement is corrupted.");

  activeClocks.pop();
  activeClock = activeClocks.top();

  if (activeClock != index)
  {
    clocks[index].toc();
    clocks[activeClock].tic();
  }
}

// cpuStats and wallStats have to be of size numSubClocks+1
void Clocks::getStats(double* cpuStats, double* wallStats)
{
  if (cpuStats) cpuStats[numSubClocks] = 0.0;
  if (wallStats) wallStats[numSubClocks] = 0.0;

  for (int i = 0; i<numSubClocks; ++i)
  {
    if (cpuStats)
    {
      cpuStats[i] = clocks[i].getElapsedCPUTime();
      cpuStats[numSubClocks] += cpuStats[i];
    }
    if (wallStats)
    {
      wallStats[i] = clocks[i].getElapsedWallTime();
      wallStats[numSubClocks] += cpuStats[i];
    }
  }
}
