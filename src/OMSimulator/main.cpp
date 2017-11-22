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

static int do_simulation(void* pModel, std::chrono::duration<double> timeout)
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
        logFatal(phase);
      }
    }
  });

  oms_initialize(pModel);
  phase = "Timeout occurred during simulation";
  oms_simulate(pModel);

  done=1;

  cv.notify_one();
  t.join();

  oms_terminate(pModel);
  oms_unload(pModel);

  return 0;
}

int main(int argc, char *argv[])
{
  ProgramOptions options;

  if (!options.load_flags(argc, argv))
    return 1;

  if (options.help)
    return 0;

  if (options.version)
  {
    std::cout << oms_getVersion() << std::endl;
    return 0;
  }

  std::string filename = options.filename;
  std::string type = "";
  if (filename.length() > 4)
    type = filename.substr(filename.length() - 3);
  else
  {
    std::cout << "Not able to process file '" << filename.c_str() << "'" << std::endl;
    std::cout << "Use OMSimulator --help for more information." << std::endl;
    return 1;
  }

  oms_setLogFile(options.logfile.c_str());

  if (options.workingDir != "")
    oms_setWorkingDirectory(options.workingDir.c_str());

  if (options.tempDir != "")
    oms_setTempDirectory(options.tempDir.c_str());

  if (type == "fmu" || type == "xml")
  {
    void* pModel = NULL;
    if (type == "fmu")
    {
      pModel = oms_newModel();
      oms_instantiateFMU(pModel, filename.c_str(), "fmu");
    }
    else
      pModel = oms_loadModel(filename.c_str());

    if (options.resultFile != "")
      oms_setResultFile(pModel, options.resultFile.c_str());
    if (options.useStartTime)
      oms_setStartTime(pModel, options.startTime);
    if (options.useStopTime)
      oms_setStopTime(pModel, options.stopTime);
    if (options.useTolerance)
      oms_setTolerance(pModel, options.tolerance);
    if (options.useCommunicationInterval)
      oms_setCommunicationInterval(pModel, options.communicationInterval);

    if (options.describe)
    {
      // OMSimulator --describe example.xml
      oms_describe(pModel);
    }
    else
    {
      return do_simulation(pModel, std::chrono::duration<double>(options.timeout));
    }

    oms_unload(pModel);
  }
  else if (type == "lua")
  {
    if (options.resultFile != "")
      std::cout << "Ignoring option '--resultFile'" << std::endl;
    if (options.useStartTime)
      std::cout << "Ignoring option '--startTime'" << std::endl;
    if (options.useStopTime)
      std::cout << "Ignoring option '--stopTime'" << std::endl;
    if (options.useTolerance)
      std::cout << "Ignoring option '--tolerance'" << std::endl;
    if (options.describe)
      std::cout << "Ignoring option '--describe'" << std::endl;
    if (options.useCommunicationInterval)
      std::cout << "Ignoring option '--interval='" << std::endl;

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
    std::cout << "Not able to process file '" << filename.c_str() << "'" << std::endl;
    std::cout << "Use OMSimulator --help for more information." << std::endl;
    return 1;
  }

  return 0;
}
