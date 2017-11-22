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

#ifndef _OMS_MODEL_H_
#define _OMS_MODEL_H_

#include "DirectedGraph.h"
#include "FMUWrapper.h"
#include "LookupTable.h"
#include "ResultWriter.h"
#include "Settings.h"
#include "Types.h"

#include <fmilib.h>

#include <deque>
#include <string>
#include <unordered_map>

class CompositeModel
{
public:
  CompositeModel();
  CompositeModel(const char* descriptionPath);
  ~CompositeModel();

  void instantiateFMU(const std::string& filename, const std::string& instanceName);
  void instantiateTable(const std::string& filename, const std::string& instanceName);
  void setReal(const std::string& var, double value);
  bool setRealInput(Variable& var, double value);
  void setInteger(const std::string& var, int value);
  void setBoolean(const std::string& var, bool value);
  double getReal(const std::string& var);
  int getInteger(const std::string& var);
  bool getBoolean(const std::string& var);
  void addConnection(const std::string& from, const std::string& to);
  void exportDependencyGraph(const std::string& prefix);
  void exportCompositeStructure(const std::string& prefix);

  void describe();
  void exportXML(const char* filename);
  void importXML(const char* filename);

  oms_status_t simulate();
  oms_status_t doSteps(const int numberOfSteps);
  oms_status_t stepUntil(const double timeValue);

  void initialize();
  void terminate();
  void reset();

  oms_status_t getCurrentTime(double *time);

  Settings& getSettings() {return settings;}
  void SetSolverMethod(std::string instanceName, std::string method);

  void setVariableFilter(const char* instanceFilter, const char* variableFilter);

  int getNumberOfInterfaces();
  oms_causality_t getInterfaceCausality(int idx);
  const char* getInterfaceName(int idx);
  const char* getInterfaceVariable(int idx);

private:
  void updateInputs(DirectedGraph& graph);
  void emit();
  void solveAlgLoop(DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC);
  Variable* getVariable(const std::string& varName);

private:
  Settings settings;
  ResultWriter *resultFile;
  std::unordered_map<std::string, FMUWrapper*> fmuInstances;
  std::unordered_map<std::string, LookupTable*> lookupTables;
  std::vector< std::pair<std::string, Variable*> > lookupAssignments;
  std::unordered_map<std::string, double> realParameterList;
  std::unordered_map<std::string, int> integerParameterList;
  std::unordered_map<std::string, bool> booleanParameterList;
  DirectedGraph outputsGraph;
  DirectedGraph initialUnknownsGraph;
  double tcur;
  oms_modelState_t modelState;
  double communicationInterval;

  std::vector<std::string> interfaceNames;
  std::vector<std::string> interfaceVariables;
};

#endif
