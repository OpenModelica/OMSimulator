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

#include <boost/program_options.hpp>

ProgramOptions::ProgramOptions()
{
  describe = false;
  help = false;
  version = false;

  resultFile = "";
  tempDir = "";
  workingDir = "";
  logfile = "";
  startTime = 0.0;
  useStartTime = false;
  stopTime = 1.0;
  useStopTime = false;
  timeout = 0;
  tolerance = 1e-6;
  useTolerance = false;
  communicationInterval = 1e-1;
  useCommunicationInterval = false;
}

bool ProgramOptions::load_flags(int argc, char** argv)
{
  boost::program_options::options_description visible_options("Options");
  boost::program_options::options_description hidden_options;
  boost::program_options::options_description all_options;
  boost::program_options::positional_options_description pos_options;

  visible_options.add_options()
  ("describe,d", "Displays brief summary of given model")
  ("help,h", "Displays the help text")
  ("interval,i", boost::program_options::value<double>(&communicationInterval), "Specifies the communication interval size.")
  ("logFile,l", boost::program_options::value<std::string>(&logfile), "Specifies the logfile (stdout is used if no log file is specified).")
  ("resultFile,r", boost::program_options::value<std::string>(&resultFile), "Specifies the name of the output result file")
  ("startTime,s", boost::program_options::value<double>(&startTime), "Specifies the start time.")
  ("stopTime,t", boost::program_options::value<double>(&stopTime), "Specifies the stop time.")
  ("timeout", boost::program_options::value<double>(&timeout), "Specifies the maximum allowed time in seconds for running a simulation (0 disables).")
  ("tempDir", boost::program_options::value<std::string>(&tempDir), "Specifies the temp directory.")
  ("tolerance", boost::program_options::value<double>(&tolerance), "Specifies the relative tolerance.")
  ("version,v", "Displays version information.")
  ("workingDir", boost::program_options::value<std::string>(&workingDir), "Specifies the working directory.");

  hidden_options.add_options()
  ("file", boost::program_options::value<std::string>(&filename)->required(), "model");

  pos_options.add("file", -1);

  all_options.add(visible_options);
  all_options.add(hidden_options);

  int oms_style = boost::program_options::command_line_style::allow_short
                | boost::program_options::command_line_style::short_allow_adjacent
                | boost::program_options::command_line_style::short_allow_next
                | boost::program_options::command_line_style::allow_long
                | boost::program_options::command_line_style::long_allow_adjacent
                | boost::program_options::command_line_style::long_allow_next
                | boost::program_options::command_line_style::allow_sticky
                | boost::program_options::command_line_style::allow_guessing
                | boost::program_options::command_line_style::allow_dash_for_short;

  boost::program_options::variables_map vm;
  boost::program_options::command_line_parser clp = boost::program_options::command_line_parser(argc, argv);
  clp.options(all_options);
  clp.positional(pos_options);

  clp.style(oms_style);

  try
  {
    boost::program_options::store(clp.run(), vm);
    boost::program_options::notify(vm);
  }
  catch (const boost::program_options::required_option& e)
  {
    /** if help is asked don't print error about missing required input files */
    if (vm.count("help"))
    {
      printUsage(visible_options);
      help = true;
      return true;
    }

    /** if version is asked don't print error about missing required input files */
    if (vm.count("version"))
    {
      version = true;
      return true;
    }

    std::cout << "The input file is required." << std::endl;
    printUsage(visible_options);

    return false;
  }

  catch (const boost::program_options::error& e)
  {
    std::cout << e.what() << std::endl;
    printUsage(visible_options);

    return false;
  }

  if (vm.count("help"))
  {
    printUsage(visible_options);
    help = true;
  }

  if (vm.count("version"))
    version = true;

  if (vm.count("describe"))
    describe = true;

  if (vm.count("startTime"))
    useStartTime = true;

  if (vm.count("stopTime"))
    useStopTime = true;

  if (vm.count("tolerance"))
    useTolerance = true;

  if (vm.count("interval"))
    useCommunicationInterval = true;

  return true;
}

void ProgramOptions::printUsage(boost::program_options::options_description& options)
{
  std::cout << "Usage: OMSimulator [Options] filename" << std::endl;
  std::cout << options;
}
