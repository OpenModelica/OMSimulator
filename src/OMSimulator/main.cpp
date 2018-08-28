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

#include "Options.h"
#include "Logging.h"

#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <OMSimulator.h>

extern "C"
{
  #include <OMSimulatorLua.c>
}

static int do_simulation(char* pModel, std::chrono::duration<double> timeout)
{
  std::mutex m;
  std::condition_variable cv;
  int done=0;
  std::string phase = "Timeout occurred during initialization";

  std::thread t([&m, &cv, &done, &phase, timeout]() // OMSimulator example.xml
  {
    std::unique_lock<std::mutex> l(m);
    if (cv.wait_for(l, timeout) == std::cv_status::timeout) {
      if (!done && timeout > std::chrono::seconds(0)) {
        logError(phase);
        exit(1);
      }
    }
  });

  oms2_initialize(pModel);
  phase = "Timeout occurred during simulation";
  oms2_simulate(pModel);

  done=1;

  cv.notify_one();
  t.join();

  oms2_unloadModel(pModel);

  return 0;
}

int main(int argc, char *argv[])
{
  ProgramOptions options(argc, argv);

  if (!options.validOptions)
    return 1;

  if (options.help)
  {
    options.printUsage();
    return 0;
  }

  if (options.version)
  {
    std::cout << oms2_getVersion() << std::endl;
    return 0;
  }

  oms2_setLoggingLevel(options.logLevel);

  std::string filename = options.filename;
  std::string type = "";
  if (filename.length() > 4)
    type = filename.substr(filename.length() - 3);
  else
  {
    logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");
    return 1;
  }

  oms2_setLogFile(options.logfile.c_str());

  if (options.workingDir != "")
    oms2_setWorkingDirectory(options.workingDir.c_str());

  if (options.tempDir != "")
    oms2_setTempDirectory(options.tempDir.c_str());

  if (type == "fmu" || type == "xml" || type == "ssd")
  {
    char* name = NULL;
    const char* defaultName = "model";
    if (type == "fmu")
    {
      name = (char*)defaultName;
      oms2_newFMIModel(name);
      oms2_addFMU(name, filename.c_str(), "fmu");

      if (options.solver != "")
      {
        oms2_addSolver(name, "solver", options.solver.c_str());
        oms2_addConnection(name, "fmu", "solver");
      }
    }
    else
    {
      oms2_loadModel(filename.c_str(), &name);
      if (options.solver != "")
        logWarning("--solver can only be used when simulating a single FMU");
    }

    if (options.resultFile != "")
      oms2_setResultFile(name, options.resultFile.c_str(), 1);
    if (options.useStartTime)
      oms2_setStartTime(name, options.startTime);
    if (options.useStopTime)
      oms2_setStopTime(name, options.stopTime);
    if (options.useTolerance)
      logWarning("--tolerance not supported yet");//oms2_setTolerance(name, options.tolerance);
    if (options.useCommunicationInterval)
      oms2_setCommunicationInterval(name, options.communicationInterval);

    if (options.describe)
    {
      // OMSimulator --describe example.xml
      //oms2_describe(name);
      logWarning("--describe not supported yet");
    }
    else
    {
      return do_simulation(name, std::chrono::duration<double>(options.timeout));
    }

    oms2_unloadModel(name);
  }
  else if (type == "lua")
  {
    if (options.resultFile != "")
      logWarning("Ignoring option '--resultFile'");
    if (options.useStartTime)
      logWarning("Ignoring option '--startTime'");
    if (options.useStopTime)
      logWarning("Ignoring option '--stopTime'");
    if (options.useTolerance)
      logWarning("Ignoring option '--tolerance'");
    if (options.describe)
      logWarning("Ignoring option '--describe'");
    if (options.useCommunicationInterval)
      logWarning("Ignoring option '--interval'");
    if (options.solver != "")
      logWarning("Ignoring option '--solver'");

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_OMSimulatorLua(L);
    if (luaL_loadfile(L, filename.c_str()))
    {
      std::cout << "FATAL ERROR: luaL_loadfile() failed: " << lua_tostring(L, -1) << std::endl;
      return 1;
    }

    if (lua_pcall(L, 0, 0, 0))
    {
      std::cout << "FATAL ERROR: lua_pcall() failed: " << lua_tostring(L, -1) << std::endl;
      return 1;
    }

    lua_close(L);
  }
  else
  {
    logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");
    return 1;
  }

  return 0;
}
