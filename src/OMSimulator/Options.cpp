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

#include <iostream>
#include <string>

ProgramOptions::ProgramOptions(int argc, char** argv)
{
  this->argi = 1;
  this->argc = argc;
  this->argv = argv;

  help = false;
  version = false;
  intervals = 1e-1;
  startTime = 0.0;
  stopTime = 1.0;
  timeout = 0;
  tolerance = 1e-6;
  useIntervals = false;
  useStartTime = false;
  useStopTime = false;
  useTolerance = false;
  logLevel = 0;
  cs = true;

  oms_regex re_integer("(\\+|-)?[[:digit:]]+");
  oms_regex re_number("[[:digit:]]+");
  oms_regex re_double("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");
  oms_regex re_default(".+");
  oms_regex re_solver("internal|euler|cvode");
  oms_regex re_mode("me|cs");

  validOptions = true;
  for (; argi<argc; ++argi)
  {
    std::string arg(argv[argi]);
    std::string value;

    if (isOption("--help", "-h"))
    {
      help = true;
    }
    else if (isOptionAndValue("--intervals", "-i", value, re_number))
    {
      intervals = atoi(value.c_str());
      useIntervals = true;

      if (intervals < 2)
      {
        std::cout << "Invalid argument: " << arg << std::endl;
        validOptions = false;
      }
    }
    else if (isOptionAndValue("--logfile", "-l", value, re_default))
    {
      logfile = value;
    }
    else if (isOptionAndValue("--logLevel", value, re_number))
    {
      logLevel = atoi(value.c_str());
    }
    else if (isOptionAndValue("--resultFile", "-r", value, re_default))
    {
      resultFile = value;
    }
    else if (isOptionAndValue("--startTime", "-s", value, re_double))
    {
      startTime = atof(value.c_str());
      useStartTime = true;
    }
    else if (isOptionAndValue("--solver", value, re_solver))
    {
      solver = value;
    }
    else if (isOptionAndValue("--mode", "-m", value, re_mode))
    {
      if ("me" == value)
        cs = false;
      else
        cs = true;
    }
    else if (isOptionAndValue("--stopTime", "-t", value, re_double))
    {
      stopTime = atof(value.c_str());
      useStopTime = true;
    }
    else if (isOptionAndValue("--timeout", value, re_double))
    {
      timeout = atof(value.c_str());
    }
    else if (isOptionAndValue("--tempDir", value, re_default))
    {
      tempDir = value;
    }
    else if (isOptionAndValue("--tolerance", value, re_double))
    {
      tolerance = atof(value.c_str());
      useTolerance = true;
    }
    else if (isOption("--version", "-v"))
    {
      version = true;
    }
    else if (isOptionAndValue("--workingDir", value, re_default))
    {
      workingDir = value;
    }
    else if (arg.front() != '-' && filename.empty())
    {
      filename = arg;
    }
    else
    {
      std::cout << "Unknown option or missing argument: " << arg << std::endl;
      validOptions = false;
    }
  }

  if(!validOptions)
    std::cout << "Use 'OMSimulator --help' for more information." << std::endl;
}

bool ProgramOptions::isOption(const std::string& name)
{
  std::string arg(argv[argi]);
  return (0 == arg.compare(name));
}

bool ProgramOptions::isOption(const std::string& name1, const std::string& name2)
{
  return isOption(name1) || isOption(name2);
}

bool ProgramOptions::isOptionAndValue(const std::string& name, std::string& value, oms_regex re)
{
  std::string arg(argv[argi]);
  std::string _value;

  if (0 == arg.compare(name))
  {
    if (++argi<argc)
      _value = std::string(argv[argi]);
    if (!regex_match(_value, re))
    {
      argi--;
      return false;
    }
    value = _value;
    return true;
  }
  else if(0 == arg.compare(0, name.length()+1, name + "="))
  {
    _value = arg.substr(name.length()+1);
    if (!regex_match(_value, re))
    {
      return false;
    }
    value = _value;
    return true;
  }

  return false;
}

bool ProgramOptions::isOptionAndValue(const std::string& name1, const std::string& name2, std::string& value, oms_regex re)
{
  return isOptionAndValue(name1, value, re) || isOptionAndValue(name2, value, re);
}

void ProgramOptions::printUsage()
{
  std::cout << "Usage: OMSimulator [Options] filename\n" << std::endl;

  std::cout << "Options:" << std::endl;
  std::cout << "  -h [ --help ]             Displays the help text" << std::endl;
  std::cout << "  -i [ --intervals ] arg    Specifies the number of communication points (arg > 1)." << std::endl;
  std::cout << "  -l [ --logFile ] arg      Specifies the logfile (stdout is used if no log file is specified)." << std::endl;
  std::cout << "  --logLevel arg            0 default, 1 default+debug, 2 default+debug+trace" << std::endl;
  std::cout << "  -m [ --mode ] arg         Forces a certain FMI mode iff the FMU provides cs and me [arg: cs (default) or me]" << std::endl;
  std::cout << "  -r [ --resultFile ] arg   Specifies the name of the output result file" << std::endl;
  std::cout << "  -s [ --startTime ] arg    Specifies the start time." << std::endl;
  std::cout << "  --solver arg              Specifies the integration method (internal, euler, cvode)." << std::endl;
  std::cout << "  -t [ --stopTime ] arg     Specifies the stop time." << std::endl;
  std::cout << "  --tempDir arg             Specifies the temp directory." << std::endl;
  std::cout << "  --timeout arg             Specifies the maximum allowed time in seconds for running a simulation (0 disables)." << std::endl;
  std::cout << "  --tolerance arg           Specifies the relative tolerance." << std::endl;
  std::cout << "  -v [ --version ]          Displays version information." << std::endl;
  std::cout << "  --workingDir arg          Specifies the working directory." << std::endl;
}
