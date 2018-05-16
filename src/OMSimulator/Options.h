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

#ifndef _OMS_OPTIONS_H_
#define _OMS_OPTIONS_H_

#include <regex>
#include <string>

class ProgramOptions
{
public:
  ProgramOptions(int argc, char** argv);
  void printUsage();

private:
  bool isOption(const std::string& name);
  bool isOption(const std::string& name1, const std::string& name2);
  bool isOptionAndValue(const std::string& name, std::string& value, std::regex re);
  bool isOptionAndValue(const std::string& name1, const std::string& name2, std::string& value, std::regex re);

public:
  bool validOptions;
  bool describe;
  bool help;
  bool version;
  double startTime;
  bool useStartTime;
  double stopTime;
  bool useStopTime;
  double tolerance;
  bool useTolerance;
  double communicationInterval;
  bool useCommunicationInterval;
  std::string filename;
  std::string resultFile;
  std::string tempDir;
  std::string workingDir;
  std::string logfile;
  double timeout;
  int logLevel;
  std::string masterAlgorithm;

private:
  int argi;
  int argc;
  char** argv;
};

#endif
