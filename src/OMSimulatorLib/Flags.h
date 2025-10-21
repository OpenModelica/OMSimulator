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

#include "OMSimulator/Types.h"
#include <string>
#include <vector>
#include <map>

namespace oms
{
  template <typename T>
  class FlagValue
  {
  public:
    bool given;
    T value;

    FlagValue() = default;
    FlagValue(const FlagValue&) = default;

    FlagValue& operator=(const FlagValue&) = default;

    FlagValue(const T& defaultValue) : given(false), value(defaultValue)
    { }

    void operator=(const T& newValue) {
      given = true;
      value = newValue;
    }

    operator const T& () const {
      return value;
    }

    bool operator==(const T& v) const {
      return value == v;
    }

    bool operator!=(const T& v) const {
      return value != v;
    }
  };

  class Flags
  {
  private:
    Flags();
    ~Flags();
    void setDefaults();

    // stop the compiler generating methods copying the object
    //Flags(Flags const&);            ///< not implemented
    Flags& operator=(Flags const&) = default; ///< not implemented

    static Flags& GetInstance();

  public:
    static oms_status_enu_t SetCommandLineOption(const std::string& cmd);

    static FlagValue<bool> &AddParametersToCSV() {return GetInstance().flagValues.addParametersToCSV;}
    static FlagValue<int> &CVODEMaxErrTestFails() {return GetInstance().flagValues.cvodeMaxErrTestFails;}
    static FlagValue<int> &CVODEMaxNLSFailures() {return GetInstance().flagValues.cvodeMaxNLSFails;}
    static FlagValue<int> &CVODEMaxNLSIterations() {return GetInstance().flagValues.cvodeMaxNLSIterations;}
    static FlagValue<int> &CVODEMaxSteps() { return GetInstance().flagValues.cvodeMaxSteps; }
    static FlagValue<bool> &DefaultModeIsCS() {return GetInstance().flagValues.defaultModeIsCS;}
    static FlagValue<bool> &DeleteTempFiles() {return GetInstance().flagValues.deleteTempFiles;}
    static FlagValue<bool> &DirectionalDerivatives() {return GetInstance().flagValues.directionalDerivatives;}
    static FlagValue<bool> &DumpAlgLoops() {return GetInstance().flagValues.dumpAlgLoops;}
    static FlagValue<bool> &EmitEvents() {return GetInstance().flagValues.emitEvents;}
    static FlagValue<bool> &IgnoreInitialUnknowns() {return GetInstance().flagValues.ignoreInitialUnknowns;}
    static FlagValue<bool> &InputExtrapolation() {return GetInstance().flagValues.inputExtrapolation;}
    static FlagValue<bool> &ProgressBar() {return GetInstance().flagValues.progressBar;}
    static FlagValue<bool> &RealTime() {return GetInstance().flagValues.realTime;}
    static FlagValue<bool> &SkipCSVHeader() {return GetInstance().flagValues.skipCSVHeader;}
    static FlagValue<bool> &SolverStats() {return GetInstance().flagValues.solverStats;}
    static FlagValue<bool> &StripRoot() {return GetInstance().flagValues.stripRoot;}
    static FlagValue<bool> &SuppressPath() {return GetInstance().flagValues.suppressPath;}
    static FlagValue<bool> &WallTime() {return GetInstance().flagValues.wallTime;}
    static FlagValue<bool> &ZeroNominal() {return GetInstance().flagValues.zeroNominal;}
    static FlagValue<double> &InitialStepSize() {return GetInstance().flagValues.initialStepSize;}
    static FlagValue<double> &MaximumStepSize() {return GetInstance().flagValues.maximumStepSize;}
    static FlagValue<double> &MinimumStepSize() {return GetInstance().flagValues.minimumStepSize;}
    static FlagValue<double> &StartTime() {return GetInstance().flagValues.startTime;}
    static FlagValue<double> &StopTime() {return GetInstance().flagValues.stopTime;}
    static FlagValue<double> &Tolerance() {return GetInstance().flagValues.tolerance;}
    static FlagValue<oms_alg_solver_enu_t> &AlgLoopSolver() {return GetInstance().flagValues.algLoopSolver;}
    static FlagValue<oms_solver_enu_t> &MasterAlgorithm() {return GetInstance().flagValues.masterAlgorithm;}
    static FlagValue<oms_solver_enu_t> &Solver() {return GetInstance().flagValues.solver;}
    static FlagValue<std::string> &ResultFile() {return GetInstance().flagValues.resultFile;}
    static FlagValue<unsigned int> &Intervals() {return GetInstance().flagValues.intervals;}
    static FlagValue<unsigned int> &MaxEventIteration() {return GetInstance().flagValues.maxEventIteration;}
    static FlagValue<unsigned int> &MaxLoopIteration() {return GetInstance().flagValues.maxLoopIteration;}
    static FlagValue<unsigned int> &NumProcs() {return GetInstance().flagValues.numProcs;}
    static FlagValue<unsigned int> &Timeout() {return GetInstance().flagValues.timeout;}

    struct FlagValues {
      FlagValues();

      FlagValue<bool> addParametersToCSV;
      FlagValue<int> cvodeMaxErrTestFails;
      FlagValue<int> cvodeMaxNLSFails;
      FlagValue<int> cvodeMaxNLSIterations;
      FlagValue<int> cvodeMaxSteps;
      FlagValue<bool> defaultModeIsCS;
      FlagValue<bool> deleteTempFiles;
      FlagValue<bool> directionalDerivatives;
      FlagValue<bool> dumpAlgLoops;
      FlagValue<bool> emitEvents;
      FlagValue<bool> ignoreInitialUnknowns;
      FlagValue<bool> inputExtrapolation;
      FlagValue<bool> progressBar;
      FlagValue<bool> realTime;
      FlagValue<bool> skipCSVHeader;
      FlagValue<bool> solverStats;
      FlagValue<bool> stripRoot;
      FlagValue<bool> suppressPath;
      FlagValue<bool> wallTime;
      FlagValue<bool> zeroNominal;
      FlagValue<double> initialStepSize;
      FlagValue<double> maximumStepSize;
      FlagValue<double> minimumStepSize;
      FlagValue<double> startTime;
      FlagValue<double> stopTime;
      FlagValue<double> tolerance;
      FlagValue<oms_alg_solver_enu_t> algLoopSolver;
      FlagValue<oms_solver_enu_t> masterAlgorithm;
      FlagValue<oms_solver_enu_t> solver;
      FlagValue<std::string> resultFile;
      FlagValue<unsigned int> intervals;
      FlagValue<unsigned int> maxEventIteration;
      FlagValue<unsigned int> maxLoopIteration;
      FlagValue<unsigned int> numProcs;
      FlagValue<unsigned int> timeout;
    };

  private:
    FlagValues flagValues;

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
    const std::string re_stepSize = "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?(,((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?,((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?)?";

    const std::vector<Flag> flags = {
      {"", "", "FMU or SSP file", re_filename, Flags::Filename, false},
      {"--addParametersToCSV", "", "Export parameters to .csv file (true, [false])", re_default, Flags::AddParametersToCSV, false},
      {"--algLoopSolver", "", "Specifies the alg. loop solver method (fixedpoint, [kinsol]) used for algebraic loops spanning over multiple components.", re_default, Flags::AlgLoopSolver, false},
      {"--clearAllOptions", "", "Reset all flags to default values", re_void, Flags::ClearAllOptions, false},
      {"--CVODEMaxErrTestFails", "", "Maximum number of error test failures for CVODE", re_number, Flags::CVODEMaxErrTestFails, false},
      {"--CVODEMaxNLSFailures", "", "Maximum number of nonlinear convergence failures for CVODE", re_number, Flags::CVODEMaxNLSFailures, false},
      {"--CVODEMaxNLSIterations", "", "Maximum number of nonlinear solver iterations for CVODE", re_number, Flags::CVODEMaxNLSIterations, false},
      {"--CVODEMaxSteps", "", "Maximum number of steps for CVODE", re_number, Flags::CVODEMaxSteps, false},
      {"--deleteTempFiles", "", "Deletes temp files as soon as they are no longer needed ([true], false)", re_bool, Flags::DeleteTempFiles, false},
      {"--directionalDerivatives", "", "Specifies whether directional derivatives should be used to calculate the Jacobian for alg. loops or if a numerical approximation should be used instead ([true], false)", re_bool, Flags::DirectionalDerivatives, false},
      {"--dumpAlgLoops", "", "Dump information for alg loops (true, [false])", re_bool, Flags::DumpAlgLoops, false},
      {"--emitEvents", "", "Specifies whether events should be emitted or not ([true], false)", re_bool, Flags::EmitEvents, false},
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
      {"--skipCSVHeader", "", "Skip exporting the scv delimiter in the header ([true], false), ", re_default, Flags::SkipCSVHeader, false},
      {"--solver", "", "Specifies the integration method (euler, [cvode])", re_default, Flags::Solver, false},
      {"--solverStats", "", "Adds solver stats to the result file, e.g. step size; not supported for all solvers (true, [false])", re_bool, Flags::SolverStats, false},
      {"--startTime", "-s", "Specifies the start time", re_double, Flags::StartTime, false},
      {"--stepSize", "", "Specifies the step size (<step size> or <init step,min step,max step>)", re_stepSize, Flags::StepSize, false},
      {"--stopTime", "-t", "Specifies the stop time", re_double, Flags::StopTime, false},
      {"--stripRoot", "", "Removes the root system prefix from all exported signals (true, [false])", re_bool, Flags::StripRoot, false},
      {"--suppressPath", "", "Supresses path information in info messages; especially useful for testing ([true], false)", re_bool, Flags::SuppressPath, false},
      {"--tempDir", "", "Specifies the temp directory", re_default, Flags::TempDir, false},
      {"--timeout", "", "Specifies the maximum allowed time in seconds for running a simulation (0 disables)", re_number, Flags::Timeout, false},
      {"--tolerance", "", "Specifies the relative tolerance", re_double, Flags::Tolerance, false},
      {"--version", "-v", "Displays version information", re_void, Flags::Version, false},
      {"--wallTime", "", "Add wall time information for to the result file (true, [false])", re_bool, Flags::WallTime, false},
      {"--workingDir", "", "Specifies the working directory", re_default, Flags::WorkingDir, false},
      {"--zeroNominal", "", "Using this flag, FMUs with invalid nominal values will be accepted and the invalid nominal values will be replaced with 1.0", re_bool, Flags::ZeroNominal, false}
    };

    static oms_status_enu_t AddParametersToCSV(const std::string& value);
    static oms_status_enu_t AlgLoopSolver(const std::string& value);
    static oms_status_enu_t ClearAllOptions(const std::string& value);
    static oms_status_enu_t CVODEMaxErrTestFails(const std::string& value);
    static oms_status_enu_t CVODEMaxNLSFailures(const std::string& value);
    static oms_status_enu_t CVODEMaxNLSIterations(const std::string& value);
    static oms_status_enu_t CVODEMaxSteps(const std::string& value);
    static oms_status_enu_t DeleteTempFiles(const std::string& value);
    static oms_status_enu_t DirectionalDerivatives(const std::string& value);
    static oms_status_enu_t DumpAlgLoops(const std::string& value);
    static oms_status_enu_t EmitEvents(const std::string& value);
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
    static oms_status_enu_t SkipCSVHeader(const std::string& value);
    static oms_status_enu_t Solver(const std::string& value);
    static oms_status_enu_t SolverStats(const std::string& value);
    static oms_status_enu_t StartTime(const std::string& value);
    static oms_status_enu_t StepSize(const std::string& value);
    static oms_status_enu_t StopTime(const std::string& value);
    static oms_status_enu_t StripRoot(const std::string& value);
    static oms_status_enu_t SuppressPath(const std::string& value);
    static oms_status_enu_t TempDir(const std::string& value);
    static oms_status_enu_t Timeout(const std::string& value);
    static oms_status_enu_t Tolerance(const std::string& value);
    static oms_status_enu_t Version(const std::string& value);
    static oms_status_enu_t WallTime(const std::string& value);
    static oms_status_enu_t WorkingDir(const std::string& value);
    static oms_status_enu_t ZeroNominal(const std::string& value);
  };
}

#endif
