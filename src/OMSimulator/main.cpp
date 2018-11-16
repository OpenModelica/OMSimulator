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

static int do_simulation(std::string model, std::chrono::duration<double> timeout)
{
  std::mutex m;
  std::condition_variable cv;
  int done=0;
  std::string phase = "Timeout occurred during instantiation";

  std::thread t([&m, &cv, &done, &phase, timeout]()
  {
    std::unique_lock<std::mutex> l(m);
    if (cv.wait_for(l, timeout) == std::cv_status::timeout)
    {
      if (!done && timeout > std::chrono::seconds(0))
      {
        logError(phase);
        exit(1);
      }
    }
  });

  oms3_instantiate(model.c_str());
  phase = "Timeout occurred during initialization";
  oms3_initialize(model.c_str());
  phase = "Timeout occurred during simulation";
  oms3_simulate(model.c_str());

  done=1;

  cv.notify_one();
  t.join();

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
    std::cout << oms3_getVersion() << std::endl;
    return 0;
  }

  oms3_setLoggingLevel(options.logLevel);

  std::string filename = options.filename;
  std::string type = "";
  if (filename.length() > 4)
    type = filename.substr(filename.length() - 4);
  else
  {
    logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");
    return 1;
  }

  oms3_setLogFile(options.logfile.c_str());

  if (options.workingDir != "")
    oms3_setWorkingDirectory(options.workingDir.c_str());

  if (options.tempDir != "")
    oms3_setTempDirectory(options.tempDir.c_str());

  if (type == ".fmu")
  {
    std::string modelName("model");
    std::string systemName = modelName + ".root";
    std::string fmuName = systemName + ".fmu";
    oms_fmi_kind_enu_t kind;
    oms_status_enu_t status;

    status = oms3_extractFMIKind(filename.c_str(), &kind);
    if (oms_status_ok != status) return logError("oms3_extractFMIKind failed");

    status = oms3_newModel(modelName.c_str());
    if (oms_status_ok != status) return logError("oms3_newModel failed");

    if (kind == oms_fmi_kind_me_and_cs)
      status = oms3_addSystem(systemName.c_str(), options.cs ? oms_system_wc : oms_system_sc);
    else
      status = oms3_addSystem(systemName.c_str(), (kind == oms_fmi_kind_cs) ? oms_system_wc : oms_system_sc);
    if (oms_status_ok != status) return logError("oms3_addSystem failed");

    status = oms3_addSubModel(fmuName.c_str(), filename.c_str());
    if (oms_status_ok != status) return logError("oms3_addSubModel failed");

    if (options.resultFile != "")
      oms3_setResultFile(modelName.c_str(), options.resultFile.c_str(), 1);
    if (options.useStartTime)
      oms3_setStartTime(modelName.c_str(), options.startTime);
    if (options.useStopTime)
      oms3_setStopTime(modelName.c_str(), options.stopTime);
    if (options.useTolerance)
      oms3_setTolerance(systemName.c_str(), options.tolerance);
    if (options.useIntervals)
      oms3_setFixedStepSize(systemName.c_str(), (options.stopTime - options.startTime) / (options.intervals-1));
    if (options.solver != "")
      oms3_setSolver(systemName.c_str(), options.solver.c_str());

    int rc = do_simulation(modelName, std::chrono::duration<double>(options.timeout));
    oms3_terminate(modelName.c_str());
    oms3_delete(modelName.c_str());
    return rc;
  }
  else if (type == ".ssp")
  {
    char* cref;
    oms3_import(filename.c_str(), &cref);

    if (options.resultFile != "")
      oms3_setResultFile(cref, options.resultFile.c_str(), 1);
    if (options.useStartTime)
      oms3_setStartTime(cref, options.startTime);
    if (options.useStopTime)
      oms3_setStopTime(cref, options.stopTime);
    if (options.useTolerance)
      logWarning("Ignoring option '--tolerance'");
    if (options.useIntervals)
      logWarning("Ignoring option '--interval'");
    if (options.solver != "")
      logWarning("Ignoring option '--solver'");

    int rc = do_simulation(std::string(cref), std::chrono::duration<double>(options.timeout));
    oms3_terminate(cref);
    oms3_delete(cref);
    return rc;
  }
  else if (type == ".lua")
  {
    if (options.resultFile != "")
      logWarning("Ignoring option '--resultFile'");
    if (options.useStartTime)
      logWarning("Ignoring option '--startTime'");
    if (options.useStopTime)
      logWarning("Ignoring option '--stopTime'");
    if (options.useTolerance)
      logWarning("Ignoring option '--tolerance'");
    if (options.useIntervals)
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
