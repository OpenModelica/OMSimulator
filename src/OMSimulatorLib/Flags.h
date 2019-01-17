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

#ifndef _OMS3_FLAGS_H_
#define _OMS3_FLAGS_H_

#include "Types.h"
#include <string>
#include <vector>
#include <map>

namespace oms3
{
  class Flags
  {
  private:
    Flags();
    ~Flags();
    void setDefaults();

    // stop the compiler generating methods copying the object
    Flags(Flags const&);            ///< not implemented
    Flags& operator=(Flags const&); ///< not implemented

    static Flags& GetInstance();

  public:
    static oms_status_enu_t SetCommandLineOption(const std::string& cmd);

    static bool DefaultModeIsCS() {return GetInstance().defaultModeIsCS;}
    static bool IgnoreInitialUnknowns() {return GetInstance().ignoreInitialUnknowns;}
    static bool InputDerivatives() {return GetInstance().inputDerivatives;}
    static bool ProgressBar() {return GetInstance().progressBar;}
    static bool SuppressPath() {return GetInstance().suppressPath;}
    static double StartTime() {return GetInstance().startTime;}
    static double StopTime() {return GetInstance().stopTime;}
    static double Timeout() {return GetInstance().timeout;}
    static double Tolerance() {return GetInstance().tolerance;}
    static std::string ResultFile() {return GetInstance().resultFile;}
    static std::string Solver() {return GetInstance().solver;}
    static unsigned int Intervals() {return GetInstance().intervals;}

    static void SuppressPath(bool value) {GetInstance().suppressPath = value;}

  private:
    bool ignoreInitialUnknowns;
    bool suppressPath;
    bool progressBar;
    bool inputDerivatives;
    bool defaultModeIsCS;
    unsigned int intervals;
    double startTime;
    double stopTime;
    double timeout;
    double tolerance;
    std::string solver;
    std::string resultFile;

    struct Flag
    {
      const std::string name;
      const std::string abbr;
      const std::string desc;
      const std::string regex;
      oms_status_enu_t (*fnc)(const std::string& value);
      const bool interrupt;
    };

    std::map<std::string, unsigned int> lookup;

    const std::string re_void = "";
    const std::string re_default = ".+";
    const std::string re_bool = "(true|false)";
    const std::string re_mode = "(me|cs)";
    const std::string re_double = "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?";
    const std::string re_number = "[[:digit:]]+";
    const std::string re_filename = ".+(\\.fmu|\\.ssp)";


    const std::vector<Flag> flags = {
      {"", "", "FMU or SSP file", re_filename, Flags::Filename, false},
      {"--clearAllOptions", "", "Reset all flags to efault vlaues", re_void, Flags::ClearAllOptions, false},
      {"--fetchAllVars", "", "", re_default, Flags::FetchAllVars, false},
      {"--help", "-h", "Displays the help text", re_void, Flags::Help, true},
      {"--ignoreInitialUnknowns", "", "Ignore the initial unknowns from the modelDesction.xml", re_bool, Flags::IgnoreInitialUnknowns, false},
      {"--intervals", "-i", "Specifies the number of communication points (arg > 1)", re_number, Flags::Intervals, false},
      {"--logFile", "-l", "Specifies the logfile (stdout is used if no log file is specified)", re_default, Flags::LogFile, false},
      {"--logLevel", "", "0 default, 1 default+debug, 2 default+debug+trace", re_number, Flags::LogLevel, false},
      {"--mode", "-m", "Forces a certain FMI mode iff the FMU provides cs and me [arg: cs (default) or me]", re_mode, Flags::Mode, false},
      {"--progressBar", "", "", re_bool, Flags::ProgressBar, false},
      {"--resultFile", "-r", "Specifies the name of the output result file", re_default, Flags::ResultFile, false},
      {"--setInputDerivatives", "", "", re_bool, Flags::SetInputDerivatives, false},
      {"--solver", "", "Specifies the integration method (internal, euler, cvode)", re_void, Flags::Solver, false},
      {"--startTime", "-s", "Specifies the start time", re_double, Flags::StartTime, false},
      {"--stopTime", "-t", "Specifies the stop time", re_double, Flags::StopTime, false},
      {"--suppressPath", "", "", re_bool, Flags::SuppressPath, false},
      {"--tempDir", "", "Specifies the temp directory", re_default, Flags::TempDir, false},
      {"--timeout", "", "Specifies the maximum allowed time in seconds for running a simulation (0 disables)", re_number, Flags::Timeout, false},
      {"--tolerance", "", "Specifies the relative tolerance", re_double, Flags::Tolerance, false},
      {"--version", "-v", "Displays version information", re_void, Flags::Version, false},
      {"--workingDir", "", "Specifies the working directory", re_default, Flags::WorkingDir, false}
    };

  private:
    static oms_status_enu_t ClearAllOptions(const std::string& value);
    static oms_status_enu_t FetchAllVars(const std::string& value);
    static oms_status_enu_t Filename(const std::string& value);
    static oms_status_enu_t Help(const std::string& value);
    static oms_status_enu_t IgnoreInitialUnknowns(const std::string& value);
    static oms_status_enu_t Intervals(const std::string& value);
    static oms_status_enu_t LogFile(const std::string& value);
    static oms_status_enu_t LogLevel(const std::string& value);
    static oms_status_enu_t Mode(const std::string& value);
    static oms_status_enu_t ProgressBar(const std::string& value);
    static oms_status_enu_t ResultFile(const std::string& value);
    static oms_status_enu_t SetInputDerivatives(const std::string& value);
    static oms_status_enu_t Solver(const std::string& value);
    static oms_status_enu_t StartTime(const std::string& value);
    static oms_status_enu_t StopTime(const std::string& value);
    static oms_status_enu_t SuppressPath(const std::string& value);
    static oms_status_enu_t TempDir(const std::string& value);
    static oms_status_enu_t Timeout(const std::string& value);
    static oms_status_enu_t Tolerance(const std::string& value);
    static oms_status_enu_t Version(const std::string& value);
    static oms_status_enu_t WorkingDir(const std::string& value);
  };
}

#endif
