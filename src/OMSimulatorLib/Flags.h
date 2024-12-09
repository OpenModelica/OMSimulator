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

#pragma once

#include "OMSimulator/Types.h"

#include <array>
#include <map>
#include <string>
#include <vector>

namespace oms
{
  class Flags
  {
  private:
    Flags();
    ~Flags();
    void setDefaults();

    // stop the compiler generating methods copying the object
    Flags(Flags const &);            ///< not implemented
    Flags &operator=(Flags const &); ///< not implemented

  public:
    static Flags &GetInstance();

    static oms_status_enu_t SetCommandLineOption(const std::string &cmd);

    static bool AddParametersToCSV() { return GetInstance().FlagAddParametersToCSV.value == "true"; }
    static bool DefaultModeIsCS() { return GetInstance().FlagMode.value == "cs"; }
    static bool DeleteTempFiles() { return GetInstance().FlagDeleteTempFiles.value == "true"; }
    static bool DirectionalDerivatives() { return GetInstance().FlagDirectionalDerivatives.value == "true"; }
    static bool DumpAlgLoops() { return GetInstance().FlagDumpAlgLoops.value == "true"; }
    static bool EmitEvents() { return GetInstance().FlagEmitEvents.value == "true"; }
    static bool IgnoreInitialUnknowns() { return GetInstance().FlagIgnoreInitialUnknowns.value == "true"; }
    static bool InputExtrapolation() { return GetInstance().FlagInputExtrapolation.value == "true"; }
    static bool ProgressBar() { return GetInstance().FlagProgressBar.value == "true"; }
    static bool RealTime() { return GetInstance().FlagRealTime.value == "true"; }
    static bool SkipCSVHeader() { return GetInstance().FlagSkipCSVHeader.value == "true"; }
    static bool SolverStats() { return GetInstance().FlagSolverStats.value == "true"; }
    static bool StripRoot() { return GetInstance().FlagStripRoot.value == "true"; }
    static bool SuppressPath() { return GetInstance().FlagSuppressPath.value == "true"; }
    static bool WallTime() { return GetInstance().FlagWallTime.value == "true"; }
    static bool ZeroNominal() { return GetInstance().FlagZeroNominal.value == "true"; }
    static double InitialStepSize() { return atof(GetInstance().FlagInitialStepSize.value.c_str()); }
    static double MaximumStepSize() { return atof(GetInstance().FlagStepSize.value.c_str()); }
    static double MinimumStepSize() { return atof(GetInstance().FlagMinimumStepSize.value.c_str()); }
    static double StartTime() { return atof(GetInstance().FlagStartTime.value.c_str()); }
    static double StopTime() { return atof(GetInstance().FlagStopTime.value.c_str()); }
    static double Timeout() { return atof(GetInstance().FlagTimeout.value.c_str()); }
    static double Tolerance() { return atof(GetInstance().FlagTolerance.value.c_str()); }
    static int CVODEMaxErrTestFails() { return atoi(GetInstance().FlagCVODEMaxErrTestFails.value.c_str()); }
    static int CVODEMaxNLSFailures() { return atoi(GetInstance().FlagCVODEMaxNLSFailures.value.c_str()); }
    static int CVODEMaxNLSIterations() { return atoi(GetInstance().FlagCVODEMaxNLSIterations.value.c_str()); }
    static int CVODEMaxSteps() { return atoi(GetInstance().FlagCVODEMaxSteps.value.c_str()); }
    static oms_alg_solver_enu_t AlgLoopSolver();
    static oms_solver_enu_t MasterAlgorithm();
    static oms_solver_enu_t Solver();
    static std::string ResultFile() { return GetInstance().FlagResultFile.value; }
    static unsigned int Intervals() { return atoi(GetInstance().FlagIntervals.value.c_str()); }
    static unsigned int MaxEventIteration() { return atoi(GetInstance().FlagMaxEventIteration.value.c_str()); }
    static unsigned int MaxLoopIteration() { return atoi(GetInstance().FlagMaxLoopIteration.value.c_str()); }
    static unsigned int NumProcs() { return atoi(GetInstance().FlagNumProcs.value.c_str()); }

  private:
    std::map<std::string, unsigned int> lookup;
    std::vector<std::string> files;

    const std::string re_void = "";
    const std::string re_default = ".+";
    const std::string re_bool = "(true|false)";
    const std::string re_mode = "(me|cs)";
    const std::string re_double = "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?";
    const std::string re_number = "[[:digit:]]+";
    const std::string re_filename = ".+(\\.fmu|\\.ssp|\\.lua)";
    const std::string re_solver = "(euler|cvode)";

  public:
    struct Flag
    {
      const std::string name;
      const std::string abbr;
      std::string value;
      const std::string defaultValue;
      const std::string desc;
      const std::string regex;
      oms_status_enu_t (*action)(const std::string &value);
      const bool interrupt;
      const bool settings;
      bool explicitlySet;
    };

    Flag FlagFilename{"", "", "", "", "FMU or SSP file to be loaded", re_filename, Flags::Filename, false, false, false};
    Flag FlagAddParametersToCSV{"--addParametersToCSV", "", "", "false", "Export parameters to a .csv file", re_bool, nullptr, false, false, false};
    Flag FlagAlgLoopSolver{"--algLoopSolver", "", "", "kinsol", "Specifies the loop solver method (fixedpoint, kinsol) used for algebraic loops spanning multiple components.", re_default, nullptr, false, false, false};
    Flag FlagClearAllOptions{"--clearAllOptions", "", "", "", "Reset all flags to their default values", re_void, Flags::ClearAllOptions, false, false, false};
    Flag FlagCVODEMaxErrTestFails{"--CVODEMaxErrTestFails", "", "", "100", "Maximum number of error test failures for CVODE", re_number, nullptr, false, false, false};
    Flag FlagCVODEMaxNLSFailures{"--CVODEMaxNLSFailures", "", "", "100", "Maximum number of nonlinear convergence failures for CVODE", re_number, nullptr, false, false, false};
    Flag FlagCVODEMaxNLSIterations{"--CVODEMaxNLSIterations", "", "", "5", "Maximum number of nonlinear solver iterations for CVODE", re_number, nullptr, false, false, false};
    Flag FlagCVODEMaxSteps{"--CVODEMaxSteps", "", "", "1000", "Maximum number of steps for CVODE", re_number, nullptr, false, false, false};
    Flag FlagDeleteTempFiles{"--deleteTempFiles", "", "", "true", "Delete temporary files as soon as they are no longer needed", re_bool, nullptr, false, false, false};
    Flag FlagDirectionalDerivatives{"--directionalDerivatives", "", "", "true", "Use directional derivatives to calculate the Jacobian for algebraic loops", re_bool, nullptr, false, false, false};
    Flag FlagDumpAlgLoops{"--dumpAlgLoops", "", "", "false", "Dump information for algebraic loops", re_bool, nullptr, false, false, false};
    Flag FlagEmitEvents{"--emitEvents", "", "", "true", "Emit events during simulation", re_bool, nullptr, false, false, false};
    Flag FlagHelp{"--help", "-h", "", "", "Display the help text", re_void, Flags::Help, true, false, false};
    Flag FlagIgnoreInitialUnknowns{"--ignoreInitialUnknowns", "", "", "false", "Ignore initial unknowns from the modelDescription.xml", re_bool, nullptr, false, false, false};
    Flag FlagInitialStepSize{"--initialStepSize", "", "", "1e-6", "Specify the initial step size", re_double, nullptr, false, false, false};
    Flag FlagInputExtrapolation{"--inputExtrapolation", "", "", "false", "Enable input extrapolation using derivative information", re_bool, nullptr, false, false, false};
    Flag FlagIntervals{"--intervals", "-i", "", "500", "Specify the number of communication points (arg > 1)", re_number, nullptr, false, false, false};
    Flag FlagLogFile{"--logFile", "-l", "", "", "Specify the log file (stdout is used if no log file is specified)", re_default, nullptr, false, false, false};
    Flag FlagLogLevel{"--logLevel", "", "", "0", "Set the log level (0: default, 1: debug, 2: debug+trace)", re_number, nullptr, false, false, false};
    Flag FlagMasterAlgorithm{"--master", "", "", "ma", "Specify the master algorithm (ma)", re_default, nullptr, false, false, false};
    Flag FlagMaxEventIteration{"--maxEventIteration", "", "", "100", "Specify the maximum number of iterations for handling a single event", re_number, nullptr, false, false, false};
    Flag FlagMaxLoopIteration{"--maxLoopIteration", "", "", "10", "Specify the maximum number of iterations for solving algebraic loops between system-level components. Internal algebraic loops of components are not affected.", re_number, nullptr, false, false, false};
    Flag FlagMinimumStepSize{"--minimumStepSize", "", "", "1e-12", "Specify the minimum step size", re_double, nullptr, false, false, false};
    Flag FlagMode{"--mode", "-m", "", "me", "Force a certain FMI mode if the FMU provides both cs and me (cs, me)", re_mode, nullptr, false, false, false};
    Flag FlagNumProcs{"--numProcs", "-n", "", "1", "Specify the maximum number of processors to use (0=auto, 1=default)", re_number, nullptr, false, false, false};
    Flag FlagProgressBar{"--progressBar", "", "", "false", "Show a progress bar for the simulation progress in the terminal", re_bool, nullptr, false, false, false};
    Flag FlagRealTime{"--realTime", "", "", "false", "Enable experimental feature for (soft) real-time co-simulation", re_bool, nullptr, false, false, false};
    Flag FlagResultFile{"--resultFile", "-r", "", "<default>", "Specify the name of the output result file", re_default, nullptr, false, false, false};
    Flag FlagSkipCSVHeader{"--skipCSVHeader", "", "", "true", "Skip exporting the CSV delimiter in the header", re_bool, nullptr, false, false, false};
    Flag FlagSolver{"--solver", "", "", "cvode", "Specify the integration method (euler, cvode)", re_solver, nullptr, false, false, false};
    Flag FlagSolverStats{"--solverStats", "", "", "false", "Add solver stats to the result file, e.g., step size; not supported for all solvers", re_bool, nullptr, false, false, false};
    Flag FlagStartTime{"--startTime", "-s", "", "0", "Specify the start time", re_double, nullptr, false, false, false};
    Flag FlagStepSize{"--stepSize", "", "", "1e-3", "Specify the (maximum) step size", re_double, nullptr, false, false, false};
    Flag FlagStopTime{"--stopTime", "-t", "", "1", "Specify the stop time", re_double, nullptr, false, false, false};
    Flag FlagStripRoot{"--stripRoot", "", "", "false", "Remove the root system prefix from all exported signals", re_bool, nullptr, false, false, false};
    Flag FlagSuppressPath{"--suppressPath", "", "", "false", "Suppress path information in info messages; especially useful for testing", re_bool, nullptr, false, false, false};
    Flag FlagTempDir{"--tempDir", "", "", ".", "Specify the temporary directory", re_default, nullptr, false, true, false};
    Flag FlagTimeout{"--timeout", "", "", "0", "Specify the maximum allowed time in seconds for running a simulation (0 disables)", re_number, nullptr, false, false, false};
    Flag FlagTolerance{"--tolerance", "", "", "1e-4", "Specify the relative tolerance", re_double, nullptr, false, false, false};
    Flag FlagVersion{"--version", "-v", "", "", "Display version information", re_void, Flags::Version, false, false, false};
    Flag FlagWallTime{"--wallTime", "", "", "false", "Add wall time information to the result file", re_bool, nullptr, false, false, false};
    Flag FlagWorkingDir{"--workingDir", "", "", ".", "Specify the working directory", re_default, nullptr, false, true, false};
    Flag FlagZeroNominal{"--zeroNominal", "", "", "false", "Accept FMUs with invalid nominal values and replace the invalid nominal values with 1.0", re_bool, nullptr, false, false, false};

  private:
    std::array<Flag *, 43> flags = {
        &FlagFilename,
        &FlagAddParametersToCSV,
        &FlagAlgLoopSolver,
        &FlagClearAllOptions,
        &FlagCVODEMaxErrTestFails,
        &FlagCVODEMaxNLSFailures,
        &FlagCVODEMaxNLSIterations,
        &FlagCVODEMaxSteps,
        &FlagDeleteTempFiles,
        &FlagDirectionalDerivatives,
        &FlagDumpAlgLoops,
        &FlagEmitEvents,
        &FlagHelp,
        &FlagIgnoreInitialUnknowns,
        &FlagInitialStepSize,
        &FlagInputExtrapolation,
        &FlagIntervals,
        &FlagLogFile,
        &FlagLogLevel,
        &FlagMasterAlgorithm,
        &FlagMaxEventIteration,
        &FlagMaxLoopIteration,
        &FlagMinimumStepSize,
        &FlagMode,
        &FlagNumProcs,
        &FlagProgressBar,
        &FlagRealTime,
        &FlagResultFile,
        &FlagSkipCSVHeader,
        &FlagSolver,
        &FlagSolverStats,
        &FlagStartTime,
        &FlagStepSize,
        &FlagStopTime,
        &FlagStripRoot,
        &FlagSuppressPath,
        &FlagTempDir,
        &FlagTimeout,
        &FlagTolerance,
        &FlagVersion,
        &FlagWallTime,
        &FlagWorkingDir,
        &FlagZeroNominal};

    static oms_status_enu_t SetFlag(size_t flag_id, const std::string &value);

    static oms_status_enu_t ClearAllOptions(const std::string &value);
    static oms_status_enu_t Filename(const std::string &value);
    static oms_status_enu_t Help(const std::string &value);
    static oms_status_enu_t Version(const std::string &value);
  };
}
