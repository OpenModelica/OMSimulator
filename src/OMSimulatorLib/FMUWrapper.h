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

#ifndef _OMS_FMU_H_
#define _OMS_FMU_H_

#include "Variable.h"
#include "DirectedGraph.h"
#include "Clocks.h"
#include "ResultWriter.h"

#include <fmilib.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */

class CompositeModel;

class FMUWrapper
{
public:
  FMUWrapper(CompositeModel& model, std::string fmuPath, std::string instanceName);
  ~FMUWrapper();

  double getReal(const std::string& var);
  double getReal(const Variable& var);
  int getInteger(const std::string& var);
  int getInteger(const Variable& var);
  bool getBoolean(const std::string& var);
  bool getBoolean(const Variable& var);
  bool setRealInput(const std::string& var, double value);
  bool setRealInput(const Variable& var, double value);
  bool setIntegerInput(const std::string& var, int value);
  bool setIntegerInput(const Variable& var, int value);
  bool setBooleanInput(const std::string& var, bool value);
  bool setBooleanInput(const Variable& var, bool value);
  bool setRealParameter(const std::string& var, double value);
  bool setIntegerParameter(const std::string& var, int value);
  bool setBooleanParameter(const std::string& var, bool value);

  void enterInitialization(double startTime);
  void exitInitialization();
  void terminate();
  void reset();
  void doStep(double stopTime);

  const DirectedGraph& getOutputsGraph() const {return outputsGraph;}
  const DirectedGraph& getInitialUnknownsGraph() const {return initialUnknownsGraph;}
  Variable* getVariable(const std::string& varName);
  Variable* getVariable(const fmi2_value_reference_t& state_vr);

  const std::string& getFMUPath() const {return fmuPath;}
  const std::string& getFMUInstanceName() const {return instanceName;}
  std::string getFMUKind() const;
  bool isFMUKindME() const;
  std::string getGUID() const;
  std::string getGenerationTool() const;

  void SetSolverMethod(const std::string& solverMethod);
  std::string GetSolverMethodString() const;

  std::vector<Variable>& getAllVariables() {return allVariables;}
  std::vector<unsigned int>& getAllInputs() {return allInputs;}
  std::vector<unsigned int>& getAllOutputs() {return allOutputs;}

  void registerSignalsForResultFile(ResultWriter *resultFile);
  void updateSignalsForResultFile(ResultWriter *resultFile);

  void setVariableFilter(const char* variableFilter) {this->variableFilter = variableFilter;}
private:
  enum Solver_t { NO_SOLVER, EXPLICIT_EULER, CVODE };

  struct SolverDataEuler_t
  {
    // empty
  };

  struct SolverDataCVODE_t
  {
    void *mem;
    N_Vector y;
    N_Vector abstol;
  };

  union SolverData_t
  {
    SolverDataEuler_t euler;
    SolverDataCVODE_t cvode;
  };

private:
  void do_event_iteration();
  void getDependencyGraph_outputs();
  void getDependencyGraph_initialUnknowns();

  friend int cvode_rhs(realtype t, N_Vector y, N_Vector ydot, void *user_data);

private:
  CompositeModel& model;
  Clocks clocks;

  std::string fmuPath;
  std::string tempDir;
  std::string instanceName;
  jm_callbacks callbacks;
  fmi2_fmu_kind_enu_t fmuKind;
  fmi2_callback_functions_t callBackFunctions;
  fmi_import_context_t* context;
  fmi2_import_t* fmu;
  fmi2_event_info_t eventInfo;

  std::vector<Variable> allVariables;
  std::vector<unsigned int> realVariables;
  std::vector<unsigned int> intVariables;
  std::vector<unsigned int> boolVariables;
  std::vector<unsigned int> strVariables;
  std::vector<unsigned int> enumVariables;
  std::vector<unsigned int> allInputs;
  std::vector<unsigned int> allOutputs;
  std::vector<unsigned int> allParameters;
  std::vector<unsigned int> initialUnknowns;

  std::unordered_map<unsigned int /*result file var ID*/, unsigned int /*allVariables ID*/> resultFileMapping;

  DirectedGraph outputsGraph;
  DirectedGraph initialUnknownsGraph;

  // ME & CS
  fmi2_real_t tcur;
  fmi2_real_t relativeTolerance;
  std::string variableFilter;

  // ME
  fmi2_boolean_t callEventUpdate;
  fmi2_boolean_t terminateSimulation;
  size_t n_states;
  size_t n_event_indicators;
  double* states;
  double* states_der;
  double* states_nominal;
  double* event_indicators;
  double* event_indicators_prev;
  Solver_t solverMethod;
  SolverData_t solverData;
};

#endif
