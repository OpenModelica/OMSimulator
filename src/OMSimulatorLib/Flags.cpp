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
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "Flags.h"

#include "Component.h"
#include "ComRef.h"
#include "Logging.h"
#include "Model.h"
#include "OMSimulator.h"
#include "Scope.h"
#include "System.h"
#include <iomanip>
#include <iostream>
#include <RegEx.h>
#include <sstream>

oms::Flags::Flags()
{
  setDefaults();

  for (unsigned int i=0; i<flags.size(); ++i)
  {
    lookup[flags[i].name] = i;
    if (!flags[i].abbr.empty())
      lookup[flags[i].abbr] = i;
  }
}

oms::Flags::~Flags()
{
}

void oms::Flags::setDefaults()
{
  addParametersToCSV = false;
  algLoopSolver = oms_alg_solver_fixedpoint;
  defaultModeIsCS = false;
  deleteTempFiles = true;
  emitEvents = true;
  exportParametersInline = true;
  ignoreInitialUnknowns = false;
  inputExtrapolation = false;
  intervals = 100;
  masterAlgorithm = oms_solver_wc_ma;
  maxEventIteration = 100;
  maxLoopIteration = 10;
  numProcs = 1;
  progressBar = false;
  realTime = false;
  resultFile = "<default>";
  skipCSVHeader = false;
  solver = oms_solver_sc_cvode;
  solverStats = false;
  startTime = 0.0;
  stopTime = 1.0;
  stripRoot = false;
  suppressPath = false;
  timeout = 0.0;
  tolerance = 1e-4;
  wallTime = false;
}

oms::Flags& oms::Flags::GetInstance()
{
  // the only instance
  static Flags flags;
  return flags;
}

bool isOption(const std::string& cmd, const std::string& name)
{
  return (0 == cmd.compare(name));
}

bool isOptionAndValue(const std::string& cmd, const std::string& name, std::string& value, oms_regex re)
{
  if (0 == cmd.compare(0, name.length()+1, name + "="))
  {
    value = cmd.substr(name.length()+1);
    return regex_match(value, re);
  }

  return false;
}

oms_status_enu_t oms::Flags::SetCommandLineOption(const std::string& cmd)
{
  GetInstance().files.clear();

  // split command line arguments
  std::vector<std::string> args;
  size_t start=0;
  for (size_t i=0; i<cmd.size()+1; ++i)
  {
    if (cmd[i] == ' ' || cmd[i] == '\0')
    {
      if (start+1 < i)
        args.push_back(cmd.substr(start, i-start));
      start = i+1;
    }
  }

  // process command line arguments
  for (const auto& arg : args)
  {
    //logInfo("\"" + arg + "\"");
    size_t end=0;
    std::string value;

    for (; end<arg.size() && arg[end] != '='; ++end);

    if (arg[0] != '-')
    {
      end = 0;
      value = arg;
    }

    // find flag
    auto l = GetInstance().lookup.find(arg.substr(0, end));
    if (l == GetInstance().lookup.end())
      return logError("Unknown flag: \"" + arg + "\"");

    // check argument
    std::string regex_str = "^" + GetInstance().flags[l->second].regex + "$";
    if (arg[end] == '=')
      value = arg.substr(end+1);
    //logInfo("\"" + value + "\"");

    if (regex_match(value, oms_regex(regex_str)))
    {
      oms_status_enu_t status = GetInstance().flags[l->second].fcn(value);
      if (GetInstance().flags[l->second].interrupt || oms_status_ok != status)
        return status;
    }
    else
      return logError("Invalid value: \"" + arg + "\"");
  }

  // run Lua scripts, FMUs, SSP files
  for (const auto& file : GetInstance().files)
    if (oms_status_ok != oms_RunFile(file.c_str()))
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::Flags::AddParametersToCSV(const std::string& value)
{
  GetInstance().addParametersToCSV = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::AlgLoopSolver(const std::string& value)
{
  if (value == "fixedpoint")
    GetInstance().algLoopSolver = oms_alg_solver_fixedpoint;
  else if (value == "kinsol")
    GetInstance().algLoopSolver = oms_alg_solver_kinsol;
  else
    return logError("Invalid solver method");
  return oms_status_ok;
  }

oms_status_enu_t oms::Flags::ClearAllOptions(const std::string& value)
{
  GetInstance().setDefaults();
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::DeleteTempFiles(const std::string& value)
{
  GetInstance().deleteTempFiles = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::EmitEvents(const std::string& value)
{
  GetInstance().emitEvents = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::ExportParametersInline(const std::string& value)
{
  GetInstance().exportParametersInline = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::FetchAllVars(const std::string& value)
{
  std::string cref = value;
  bool enableOption = true;
  if (value[0] == '-' || value[0] == '+')
  {
    enableOption = (value[0] == '+');
    cref = cref.substr(1);
  }

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  if (component->getType() != oms_component_fmu)
    return oms_status_error;

  component->fetchAllVars(enableOption);
  if (enableOption)
    logDebug("--fetchAllVars is enabled for " + cref);
  else
    logDebug("--fetchAllVars is disabled for " + cref);
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Filename(const std::string& value)
{
  GetInstance().files.push_back(value);
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Help(const std::string& value)
{
  std::stringstream ss;
  ss << "Usage: OMSimulator [Options] [Lua script] [FMU] [SSP file]" << std::endl;
  ss << "Options:" << std::endl;
  for (unsigned int i=0; i<GetInstance().flags.size(); ++i)
  {
    if (GetInstance().flags[i].name.empty())
      continue;

    std::string label = "  " + GetInstance().flags[i].name;
    if (!GetInstance().flags[i].regex.empty())
    {
      if (GetInstance().flags[i].regex == GetInstance().re_bool)
        label += "=<bool>";
      else if (GetInstance().flags[i].regex == GetInstance().re_double)
        label += "=<double>";
      else if (GetInstance().flags[i].regex == GetInstance().re_number)
        label += "=<int>";
      else
        label += "=<arg>";
    }
    if (!GetInstance().flags[i].abbr.empty())
      label += " [" + GetInstance().flags[i].abbr + "]";

    ss << std::left << std::setw(32) << label << "  " << GetInstance().flags[i].desc << std::endl;
  }

  logInfo(ss.str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::IgnoreInitialUnknowns(const std::string& value)
{
  GetInstance().ignoreInitialUnknowns = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::InputExtrapolation(const std::string& value)
{
  GetInstance().inputExtrapolation = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Intervals(const std::string& value)
{
  GetInstance().intervals = atoi(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::LogFile(const std::string& value)
{
  oms_setLogFile(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::LogLevel(const std::string& value)
{
  oms_setLoggingLevel(atoi(value.c_str()));
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::MaxEventIteration(const std::string& value)
{
  GetInstance().maxEventIteration = atoi(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::MaxLoopIteration(const std::string& value)
{
  GetInstance().maxLoopIteration = atoi(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Mode(const std::string& value)
{
  GetInstance().defaultModeIsCS = (value == "cs");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::NumProcs(const std::string& value)
{
  GetInstance().numProcs = atoi(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::ProgressBar(const std::string& value)
{
  GetInstance().progressBar = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::RealTime(const std::string& value)
{
  GetInstance().realTime = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::ResultFile(const std::string& value)
{
  GetInstance().resultFile = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::SetInputDerivatives(const std::string& value)
{
  logWarning("--setInputDerivatives is deprecated; use --inputExtrapolation instead");
  return InputExtrapolation(value);
}

oms_status_enu_t oms::Flags::SkipCSVHeader(const std::string& value)
{
  GetInstance().skipCSVHeader = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Solver(const std::string& value)
{
  if (value == "euler")
    GetInstance().solver = oms_solver_sc_explicit_euler;
  else if (value == "cvode")
    GetInstance().solver = oms_solver_sc_cvode;
  else
    return logError("Invalid solver method");

  return oms_status_ok;
}

oms_status_enu_t oms::Flags::SolverStats(const std::string& value)
{
  GetInstance().solverStats = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::StartTime(const std::string& value)
{
  GetInstance().startTime = atof(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::StopTime(const std::string& value)
{
  GetInstance().stopTime = atof(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::StripRoot(const std::string& value)
{
  GetInstance().stripRoot = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::SuppressPath(const std::string& value)
{
  GetInstance().suppressPath = (value == "true");
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::TempDir(const std::string& value)
{
  oms_setTempDirectory(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Timeout(const std::string& value)
{
  GetInstance().timeout = atoi(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Tolerance(const std::string& value)
{
  GetInstance().tolerance = atof(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Version(const std::string& value)
{
  std::cout << oms_getVersion() << std::endl;
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::WorkingDir(const std::string& value)
{
  oms_setWorkingDirectory(value.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::WallTime(const std::string& value)
{
  GetInstance().wallTime = (value == "true");
  return oms_status_ok;
}
