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

#include "Logging.h"
#include "OMSimulator/OMSimulator.h"

#include <json.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

oms::Flags::Flags()
{
  setDefaults();

  for (size_t i = 0; i < flags.size(); ++i)
  {
    lookup[flags[i]->name] = i;
    if (!flags[i]->abbr.empty())
      lookup[flags[i]->abbr] = i;
  }
}

oms::Flags::~Flags()
{
}

oms::Flags &oms::Flags::GetInstance()
{
  // the only instance
  static Flags flags;
  return flags;
}

void LoadSettings(nlohmann::json &json)
{
#if defined(_WIN32) || defined(_WIN64)
  const char *homeDir = getenv("USERPROFILE");
#else
  const char *homeDir = getenv("HOME");
#endif
  if (homeDir)
  {
    std::filesystem::path filePath = std::filesystem::path(homeDir) / ".omsimulator" / "settings.json";
    if (std::filesystem::exists(filePath))
    {
      std::ifstream fileStream(filePath);
      if (fileStream.is_open())
      {
        json = nlohmann::json::parse(fileStream);
        fileStream.close();
      }
    }
  }
}

void SaveSettings(nlohmann::json &json)
{
#if defined(_WIN32) || defined(_WIN64)
  const char *homeDir = getenv("USERPROFILE");
#else
  const char *homeDir = getenv("HOME");
#endif
  if (homeDir)
  {
    std::filesystem::path path = std::filesystem::path(homeDir) / ".omsimulator";
    std::filesystem::path filePath = path / "settings.json";
    std::filesystem::create_directories(path);
    std::ofstream fileStream(filePath);
    if (fileStream.is_open())
    {
      fileStream << json.dump(2);
      fileStream.close();
    }
    else
    {
      logWarning("Failed to open settings.json for writing");
    }
  }
  else
  {
    logWarning("HOME environment variable is not set, settings.json couldn't be saved");
  }
}

void oms::Flags::setDefaults()
{
  nlohmann::json json;
  LoadSettings(json);

  files.clear();
  for (auto &flag : flags)
  {
    flag->explicitlySet = false;

    if (flag->settings && json.find(flag->name) != json.end())
      flag->value = json[flag->name];
    else
      flag->value = flag->defaultValue;
  }
}

oms_status_enu_t oms::Flags::SetFlag(size_t flag_id, const std::string &value)
{
  _Flag *flag = GetInstance().flags[flag_id];

  std::string regex_str = "^" + flag->regex + "$";
  if (!regex_match(value, std::regex(regex_str)))
    return logError("Invalid value: \"" + value + "\" for flag " + flag->name);

  if (flag->explicitlySet && flag->name != "")
    logWarning("Flag " + flag->name + " is set multiple times");

  flag->value = value;
  flag->explicitlySet = true;

  if (flag->settings)
  {
    nlohmann::json json;
    LoadSettings(json);
    json[flag->name] = value;
    SaveSettings(json);
  }

  if (flag->action)
    return flag->action(value);

  return oms_status_ok;
}

oms_status_enu_t oms::Flags::SetCommandLineOption(const std::string &cmd)
{
  GetInstance().files.clear();

  // logInfo("Command line: " + cmd);

  // split command line arguments
  std::vector<std::string> args;
  std::istringstream iss(cmd);
  std::string token;
  while (iss >> std::quoted(token))
  {
    args.push_back(token);
  }

  // process command line arguments
  for (const auto &arg : args)
  {
    // logInfo("\"" + arg + "\"");
    size_t end = 0;
    std::string value;

    for (; end < arg.size() && arg[end] != '='; ++end)
      ;

    if (arg[0] != '-')
    {
      end = 0;
      value = arg;
    }

    // find flag
    auto l = GetInstance().lookup.find(arg.substr(0, end));
    if (l == GetInstance().lookup.end())
      return logError("Unknown flag: \"" + arg + "\"");

    if (arg[end] == '=')
      value = arg.substr(end + 1);

    if (oms_status_ok != SetFlag(l->second, value))
      return logError("Invalid value: \"" + arg + "\"");

    if (GetInstance().flags[l->second]->interrupt)
      return oms_status_ok;
  }

  // run Lua scripts, FMUs, SSP files
  for (const auto &file : GetInstance().files)
  {
    oms_status_enu_t status = oms_RunFile(file.c_str());
    if (oms_status_ok != status)
      return status;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Flags::ClearAllOptions(const std::string &value)
{
  GetInstance().setDefaults();
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Filename(const std::string &value)
{
  GetInstance().files.push_back(value);
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Help(const std::string &value)
{
  std::stringstream ss;
  ss << "Usage: OMSimulator [Options] [Lua script] [FMU] [SSP file]" << std::endl;
  ss << "Options:" << std::endl;
  for (size_t i = 0; i < GetInstance().flags.size(); ++i)
  {
    if (GetInstance().flags[i]->name.empty())
      continue;

    std::string label = "  " + GetInstance().flags[i]->name;
    std::string default_value;
    if (!GetInstance().flags[i]->regex.empty())
    {
      default_value = GetInstance().flags[i]->defaultValue;
      if (GetInstance().flags[i]->regex == GetInstance().re_bool)
        label += "=<bool>";
      else if (GetInstance().flags[i]->regex == GetInstance().re_double)
        label += "=<double>";
      else if (GetInstance().flags[i]->regex == GetInstance().re_number)
        label += "=<int>";
      else
      {
        label += "=<arg>";
        default_value = "\"" + GetInstance().flags[i]->defaultValue + "\"";
      }
    }
    if (!GetInstance().flags[i]->abbr.empty())
      label += " [" + GetInstance().flags[i]->abbr + "]";

    ss << std::left << std::setw(35) << label
       << std::left << std::setw(17) << default_value
       << "  " << GetInstance().flags[i]->desc << std::endl;
  }

  logInfo(ss.str());
  return oms_status_ok;
}

oms_status_enu_t oms::Flags::Version(const std::string &value)
{
  std::cout << oms_getVersion() << std::endl;
  return oms_status_ok;
}

std::vector<std::string> split(const std::string &s, char delim)
{
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  while (getline(ss, item, delim))
    result.push_back(item);

  return result;
}

double oms::Flags::InitialStepSize()
{
  std::vector<std::string> options = split(GetInstance().FlagStepSize.value, ',');

  for (const auto &option : options)
    if (atof(option.c_str()) <= 0.0)
      assert(false && "The step size value must be a greater than zero");

  if (options.size() == 1)
    return atof(options[0].c_str());
  else if (options.size() == 3)
    return atof(options[0].c_str());

  assert(false && "Invalid step size options");
  return 0.0;  // unreachable; to avoid compiler warning
}

double oms::Flags::MinimumStepSize()
{
  std::vector<std::string> options = split(GetInstance().FlagStepSize.value, ',');

  for (const auto &option : options)
    if (atof(option.c_str()) <= 0.0)
      assert(false && "The step size value must be a greater than zero");

  if (options.size() == 1)
    return atof(options[0].c_str());
  else if (options.size() == 3)
    return atof(options[1].c_str());

  assert(false && "Invalid step size options");
  return 0.0;  // unreachable; to avoid compiler warning
}

double oms::Flags::MaximumStepSize()
{
  std::vector<std::string> options = split(GetInstance().FlagStepSize.value, ',');

  for (const auto &option : options)
    if (atof(option.c_str()) <= 0.0)
      assert(false && "The step size value must be a greater than zero");

  if (options.size() == 1)
    return atof(options[0].c_str());
  else if (options.size() == 3)
    return atof(options[2].c_str());

  assert(false && "Invalid step size options");
  return 0.0;  // unreachable; to avoid compiler warning
}

oms_alg_solver_enu_t oms::Flags::AlgLoopSolver()
{
  if (GetInstance().FlagAlgLoopSolver.value == "fixedpoint")
    return oms_alg_solver_fixedpoint;
  else if (GetInstance().FlagAlgLoopSolver.value == "kinsol")
    return oms_alg_solver_kinsol;

  assert(false && "Invalid algebraic loop solver");
  return oms_alg_solver_kinsol;  // unreachable; to avoid compiler warning
}

oms_solver_enu_t oms::Flags::MasterAlgorithm()
{
  if (GetInstance().FlagMasterAlgorithm.value == "ma")
    return oms_solver_wc_ma;

  assert(false && "Invalid master algorithm");
  return oms_solver_wc_ma;  // unreachable; to avoid compiler warning
}

oms_solver_enu_t oms::Flags::Solver()
{
  if (GetInstance().FlagSolver.value == "euler")
    return oms_solver_sc_explicit_euler;
  else if (GetInstance().FlagSolver.value == "cvode")
    return oms_solver_sc_cvode;

  assert(false && "Invalid solver");
  return oms_solver_sc_explicit_euler;  // unreachable; to avoid compiler warning
}
