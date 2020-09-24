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

#ifndef _OMS_FLAGS_H_
#define _OMS_FLAGS_H_

#include "Types.h"
#include <string>
#include <vector>
#include <map>

namespace oms
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

    static oms_alg_solver_enu_t AlgLoopSolver() {return GetInstance().algLoopSolver;}
    static bool AddParametersToCSV() {return GetInstance().addParametersToCSV;}
    static bool DefaultModeIsCS() {return GetInstance().defaultModeIsCS;}
    static bool DeleteTempFiles() {return GetInstance().deleteTempFiles;}
    static bool EmitEvents() {return GetInstance().emitEvents;}
    static bool ExportParametersInline() {return GetInstance().exportParametersInline;}
    static bool IgnoreInitialUnknowns() {return GetInstance().ignoreInitialUnknowns;}
    static bool InputExtrapolation() {return GetInstance().inputExtrapolation;}
    static bool ProgressBar() {return GetInstance().progressBar;}
    static bool RealTime() {return GetInstance().realTime;}
    static bool SkipCSVHeader() {return GetInstance().skipCSVHeader;}
    static bool SolverStats() {return GetInstance().solverStats;}
    static bool StripRoot() {return GetInstance().stripRoot;}
    static bool SuppressPath() {return GetInstance().suppressPath;}
    static bool WallTime() {return GetInstance().wallTime;}
    static double StartTime() {return GetInstance().startTime;}
    static double StopTime() {return GetInstance().stopTime;}
    static double Timeout() {return GetInstance().timeout;}
    static double Tolerance() {return GetInstance().tolerance;}
    static oms_solver_enu_t MasterAlgorithm() {return GetInstance().masterAlgorithm;}
    static oms_solver_enu_t Solver() {return GetInstance().solver;}
    static std::string ResultFile() {return GetInstance().resultFile;}
    static unsigned int Intervals() {return GetInstance().intervals;}
    static unsigned int MaxEventIteration() {return GetInstance().maxEventIteration;}
    static unsigned int MaxLoopIteration() {return GetInstance().maxLoopIteration;}
    static unsigned int NumProcs() {return GetInstance().numProcs;}

  private:
    bool addParametersToCSV;
    oms_alg_solver_enu_t algLoopSolver;
    bool defaultModeIsCS;
    bool deleteTempFiles;
    bool emitEvents;
    bool exportParametersInline;
    bool ignoreInitialUnknowns;
    bool inputExtrapolation;
    bool progressBar;
    bool realTime;
    bool skipCSVHeader;
    bool solverStats;
    bool stripRoot;
    bool suppressPath;
    bool wallTime;
    double startTime;
    double stopTime;
    double timeout;
    double tolerance;
    oms_solver_enu_t masterAlgorithm;
    oms_solver_enu_t solver;
    std::string resultFile;
    unsigned int intervals;
    unsigned int maxEventIteration;
    unsigned int maxLoopIteration;
    unsigned int numProcs;

  private:
    struct Flag
    {
      const std::string name;
      const std::string abbr;
      const std::string desc;
      const std::string regex;
      oms_status_enu_t (*fcn)(const std::string& value);
      const bool interrupt;
    };

    std::map<std::string, unsigned int> lookup;
    std::vector<std::string> files;

    const std::string re_void = "";
    const std::string re_default = ".+";
    const std::string re_bool = "(true|false)";
    const std::string re_mode = "(me|cs)";
    const std::string re_double = "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?";
    const std::string re_number = "[[:digit:]]+";
    const std::string re_filename = ".+(\\.fmu|\\.ssp|\\.lua)";

    const std::vector<Flag> flags = {
      {"", "", "FMU or SSP file", re_filename, Flags::Filename, false},
      {"--addParametersToCSV", "", "Export parameters to .csv file (true, [false])", re_default, Flags::AddParametersToCSV, false},
      {"--algLoopSolver", "", "Specifies the alg. loop solver method ([fixedpoint], kinsol) used for algebraic loops spanning over multiple components.", re_default, Flags::AlgLoopSolver, false},
      {"--clearAllOptions", "", "Reset all flags to default values", re_void, Flags::ClearAllOptions, false},
      {"--deleteTempFiles", "", "Deletes temp files as soon as they are no longer needed ([true], false)", re_bool, Flags::DeleteTempFiles, false},
      {"--emitEvents", "", "Specifies whether events should be emitted or not ([true], false)", re_bool, Flags::EmitEvents, false},
      {"--exportParametersInline", "", "Export ParameterBindings inline with .ssd file, ", re_default, Flags::ExportParametersInline, false},
      {"--fetchAllVars", "", "Workaround for certain FMUs that do not update all internal dependencies automatically", re_default, Flags::FetchAllVars, false},
      {"--help", "-h", "Displays the help text", re_void, Flags::Help, true},
      {"--ignoreInitialUnknowns", "", "Ignore the initial unknowns from the modelDescription.xml (true, [false])", re_bool, Flags::IgnoreInitialUnknowns, false},
      {"--inputExtrapolation", "", "Enables input extrapolation using derivative information (true, [false])", re_bool, Flags::InputExtrapolation, false},
      {"--intervals", "-i", "Specifies the number of communication points (arg > 1)", re_number, Flags::Intervals, false},
      {"--logFile", "-l", "Specifies the logfile (stdout is used if no log file is specified)", re_default, Flags::LogFile, false},
      {"--logLevel", "", "0 default, 1 debug, 2 debug+trace", re_number, Flags::LogLevel, false},
      {"--maxEventIteration", "", "Specifies the max. number of iterations for handling a single event", re_number, Flags::MaxEventIteration, false},
      {"--maxLoopIteration", "", "Specifies the max. number of iterations for solving algebraic loops between system-level components. Internal algebraic loops of components are not affected.", re_number, Flags::MaxLoopIteration, false},
      {"--mode", "-m", "Forces a certain FMI mode iff the FMU provides cs and me (cs, [me])", re_mode, Flags::Mode, false},
      {"--numProcs", "-n", "Specifies the max. number of processors to use (0=auto, 1=default)", re_number, Flags::NumProcs, false},
      {"--progressBar", "", "Shows a progress bar for the simulation progress in the terminal (true, [false])", re_bool, Flags::ProgressBar, false},
      {"--realTime", "", "Experimental feature for (soft) real-time co-simulation (true, [false])", re_bool, Flags::RealTime, false},
      {"--resultFile", "-r", "Specifies the name of the output result file", re_default, Flags::ResultFile, false},
      {"--setInputDerivatives", "", "Deprecated; see '--inputExtrapolation'", re_bool, Flags::SetInputDerivatives, false},
      {"--skipCSVHeader", "", "Skip exporting the scv delimiter in the header (true, [false]), ", re_default, Flags::SkipCSVHeader, false},
      {"--solver", "", "Specifies the integration method (euler, [cvode])", re_default, Flags::Solver, false},
      {"--solverStats", "", "Adds solver stats to the result file, e.g. step size; not supported for all solvers (true, [false])", re_bool, Flags::SolverStats, false},
      {"--startTime", "-s", "Specifies the start time", re_double, Flags::StartTime, false},
      {"--stopTime", "-t", "Specifies the stop time", re_double, Flags::StopTime, false},
      {"--stripRoot", "", "Removes the root system prefix from all exported signals (true, [false])", re_bool, Flags::StripRoot, false},
      {"--suppressPath", "", "Supresses path information in info messages; especially useful for testing (true, [false])", re_bool, Flags::SuppressPath, false},
      {"--tempDir", "", "Specifies the temp directory", re_default, Flags::TempDir, false},
      {"--timeout", "", "Specifies the maximum allowed time in seconds for running a simulation (0 disables)", re_number, Flags::Timeout, false},
      {"--tolerance", "", "Specifies the relative tolerance", re_double, Flags::Tolerance, false},
      {"--version", "-v", "Displays version information", re_void, Flags::Version, false},
      {"--wallTime", "", "Add wall time information for to the result file (true, [false])", re_bool, Flags::WallTime, false},
      {"--workingDir", "", "Specifies the working directory", re_default, Flags::WorkingDir, false}
    };

    static oms_status_enu_t AddParametersToCSV(const std::string& value);
    static oms_status_enu_t AlgLoopSolver(const std::string& value);
    static oms_status_enu_t ClearAllOptions(const std::string& value);
    static oms_status_enu_t DeleteTempFiles(const std::string& value);
    static oms_status_enu_t EmitEvents(const std::string& value);
    static oms_status_enu_t ExportParametersInline(const std::string& value);
    static oms_status_enu_t FetchAllVars(const std::string& value);
    static oms_status_enu_t Filename(const std::string& value);
    static oms_status_enu_t Help(const std::string& value);
    static oms_status_enu_t IgnoreInitialUnknowns(const std::string& value);
    static oms_status_enu_t InputExtrapolation(const std::string& value);
    static oms_status_enu_t Intervals(const std::string& value);
    static oms_status_enu_t LogFile(const std::string& value);
    static oms_status_enu_t LogLevel(const std::string& value);
    static oms_status_enu_t MaxEventIteration(const std::string& value);
    static oms_status_enu_t MaxLoopIteration(const std::string& value);
    static oms_status_enu_t Mode(const std::string& value);
    static oms_status_enu_t NumProcs(const std::string& value);
    static oms_status_enu_t ProgressBar(const std::string& value);
    static oms_status_enu_t RealTime(const std::string& value);
    static oms_status_enu_t ResultFile(const std::string& value);
    static oms_status_enu_t SetInputDerivatives(const std::string& value);
    static oms_status_enu_t SkipCSVHeader(const std::string& value);
    static oms_status_enu_t Solver(const std::string& value);
    static oms_status_enu_t SolverStats(const std::string& value);
    static oms_status_enu_t StartTime(const std::string& value);
    static oms_status_enu_t StopTime(const std::string& value);
    static oms_status_enu_t StripRoot(const std::string& value);
    static oms_status_enu_t SuppressPath(const std::string& value);
    static oms_status_enu_t TempDir(const std::string& value);
    static oms_status_enu_t Timeout(const std::string& value);
    static oms_status_enu_t Tolerance(const std::string& value);
    static oms_status_enu_t Version(const std::string& value);
    static oms_status_enu_t WallTime(const std::string& value);
    static oms_status_enu_t WorkingDir(const std::string& value);
  };
}

#endif
