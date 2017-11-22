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

#include "Clocks.h"
#include "CompositeModel.h"
#include "CSVWriter.h"
#include "DirectedGraph.h"
#include "Logging.h"
#include "LookupTable.h"
#include "MATWriter.h"
#include "ResultWriter.h"
#include "Settings.h"
#include "Types.h"
#include "Util.h"

#include <fmilib.h>
#include <JM/jm_portability.h>

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/filesystem.hpp>

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

CompositeModel::CompositeModel()
  : fmuInstances(),
    resultFile(NULL)
{
  logTrace();
  modelState = oms_modelState_instantiated;
}

CompositeModel::CompositeModel(const char* descriptionPath)
  : CompositeModel()
{
  logTrace();
  importXML(descriptionPath);

  if (!settings.GetResultFile())
  {
    boost::filesystem::path path(descriptionPath);
    std::string filename = path.stem().string() + "_res.mat";
    settings.SetResultFile(filename.c_str());
  }
}

CompositeModel::~CompositeModel()
{
  logTrace();
  for (auto it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    delete it->second;

  for (auto it=lookupTables.begin(); it != lookupTables.end(); it++)
    delete it->second;
}

void CompositeModel::instantiateFMU(const std::string& filename, const std::string& instanceName)
{
  logTrace();
  OMS_TIC(globalClocks, GLOBALCLOCK_INSTANTIATION);

  fmuInstances[instanceName] = new FMUWrapper(*this, filename, instanceName);
  outputsGraph.includeGraph(fmuInstances[instanceName]->getOutputsGraph());
  initialUnknownsGraph.includeGraph(fmuInstances[instanceName]->getInitialUnknownsGraph());

  OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
}

void CompositeModel::instantiateTable(const std::string& filename, const std::string& instanceName)
{
  logTrace();
  OMS_TIC(globalClocks, GLOBALCLOCK_INSTANTIATION);

  LookupTable *table = new LookupTable();
  table->open(filename);
  lookupTables[instanceName] = table;

  OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
}

void CompositeModel::setReal(const std::string& var, double value)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    logError("CompositeModel::setReal: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
    return;
  }

  Variable *v = fmuInstances[fmuInstance]->getVariable(fmuVar);
  if (v && v->isParameter())
  {
    bool success = fmuInstances[fmuInstance]->setRealParameter(fmuVar, value);

    // store the list of modified parameters
    if (success)
      realParameterList[var] = value;
  }
  else if (v && v->isInput())
    fmuInstances[fmuInstance]->setRealInput(fmuVar, value);
  else
    logError("CompositeModel::setReal failed");
}

bool CompositeModel::setRealInput(Variable& var, double value)
{
  return var.getFMUInstance()->setRealInput(var, value);
}

void CompositeModel::setInteger(const std::string& var, int value)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    logError("CompositeModel::setInteger: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
    return;
  }

  Variable *v = fmuInstances[fmuInstance]->getVariable(fmuVar);
  if (v && v->isParameter())
  {
    bool success = fmuInstances[fmuInstance]->setIntegerParameter(fmuVar, value);

    // store the list of modified parameters
    if (success)
      integerParameterList[var] = value;
  }
  else if (v && v->isInput())
    fmuInstances[fmuInstance]->setIntegerInput(fmuVar, value);
  else
    logError("CompositeModel::setInteger failed");
}

void CompositeModel::setBoolean(const std::string& var, bool value)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    logError("CompositeModel::setBoolean: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
    return;
  }

  Variable *v = fmuInstances[fmuInstance]->getVariable(fmuVar);
  if (v && v->isParameter())
  {
    bool success = fmuInstances[fmuInstance]->setBooleanParameter(fmuVar, value);

    // store the list of modified parameters
    if (success)
      booleanParameterList[var] = value;
  }
  else if (v && v->isInput())
    fmuInstances[fmuInstance]->setBooleanInput(fmuVar, value);
  else
    logError("CompositeModel::setBoolean failed");
}

double CompositeModel::getReal(const std::string& var)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    // TODO: Provide suitable return value to handle unsuccessful calls.
    logFatal("CompositeModel::getReal: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
  }

  return fmuInstances[fmuInstance]->getReal(fmuVar);
}

int CompositeModel::getInteger(const std::string& var)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    // TODO: Provide suitable return value to handle unsuccessful calls.
    logFatal("CompositeModel::getInteger: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
  }

  return fmuInstances[fmuInstance]->getInteger(fmuVar);
}

bool CompositeModel::getBoolean(const std::string& var)
{
  logTrace();
  std::stringstream var_(var);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    // TODO: Provide suitable return value to handle unsuccessful calls.
    logFatal("CompositeModel::getBoolean: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
  }

  return fmuInstances[fmuInstance]->getBoolean(fmuVar);
}

void CompositeModel::addConnection(const std::string& from, const std::string& to)
{
  logTrace();
  OMS_TIC(globalClocks, GLOBALCLOCK_INSTANTIATION);

  std::stringstream var1_(from);
  std::stringstream var2_(to);
  std::string fmuInstance1, fmuInstance2;
  std::string fmuVar1, fmuVar2;

  std::getline(var1_, fmuInstance1, '.');
  std::getline(var1_, fmuVar1);

  std::getline(var2_, fmuInstance2, '.');
  std::getline(var2_, fmuVar2);

  // check if fmuInstance1 is actually a lookup table
  if (lookupTables.find(fmuInstance1) != lookupTables.end())
  {
    if (fmuInstances.find(fmuInstance2) == fmuInstances.end())
    {
      logError("CompositeModel::addConnection: FMU instance \"" + fmuInstance2 + "\" doesn't exist in model");
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }

    Variable *var2 = fmuInstances[fmuInstance2]->getVariable(fmuVar2);

    lookupAssignments.push_back(std::pair<std::string, Variable*>(from, var2));
  }
  else
  {
    if (fmuInstances.find(fmuInstance1) == fmuInstances.end())
    {
      logError("CompositeModel::addConnection: FMU instance \"" + fmuInstance1 + "\" doesn't exist in model");
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }

    if (fmuInstances.find(fmuInstance2) == fmuInstances.end())
    {
      logError("CompositeModel::addConnection: FMU instance \"" + fmuInstance2 + "\" doesn't exist in model");
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }

    Variable *var1 = fmuInstances[fmuInstance1]->getVariable(fmuVar1);
    Variable *var2 = fmuInstances[fmuInstance2]->getVariable(fmuVar2);

    if (!var1)
    {
      logError("CompositeModel::addConnection: output \"" + fmuInstance1 + "." + fmuVar1 + "\" doesn't exist");
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }
    if (!var2)
    {
      logError("CompositeModel::addConnection: input \"" + fmuInstance2 + "." + fmuVar2 + "\" doesn't exist");
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }

    if (var1->isOutput() && var2->isInput())
    {
      outputsGraph.addEdge(*var1, *var2);
      initialUnknownsGraph.addEdge(*var1, *var2);
    }
    else if (var2->isOutput() && var1->isInput())
    {
      outputsGraph.addEdge(*var2, *var1);
      initialUnknownsGraph.addEdge(*var2, *var1);
    }
    else
    {
      logError("CompositeModel::addConnection: Connections can only be made between an output and an input:");
      logInfo("- causality of \"" + from + "\" is " + var1->getCausalityString());
      logInfo("- causality of \"" + to + "\" is " + var2->getCausalityString());
      OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
      return;
    }
  }

  OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
}

void CompositeModel::exportDependencyGraph(const std::string& prefix)
{
  logTrace();
  initialUnknownsGraph.dotExport(prefix + "_initialization.dot");
  outputsGraph.dotExport(prefix + "_simulation.dot");
}

void CompositeModel::exportCompositeStructure(const std::string& prefix)
{
  logTrace();

  /*
   * #dot -Gsplines=none test.dot | neato -n -Gsplines=ortho -Tpng -otest.png
   * digraph G
   * {
   *   graph [rankdir=LR, splines=ortho];
   *
   *   node[shape=record];
   *   A [label="A", height=2, width=2];
   *   B [label="B", height=2, width=2];
   *
   *   A -> B [label="A.y -> B.u"];
   * }
   */
  std::ofstream dotFile(prefix + ".dot");
  dotFile << "#dot -Gsplines=none " << prefix.c_str() << ".dot | neato -n -Gsplines=ortho -Tpng -o" << prefix.c_str() << ".png" << std::endl;
  dotFile << "digraph G" << std::endl;
  dotFile << "{" << std::endl;
  dotFile << "  graph [rankdir=LR, splines=ortho];\n" << std::endl;
  dotFile << "  node[shape=record];" << std::endl;

  for (auto it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    dotFile << "  " << it->first << "[label=\"" << it->first << "\", height=2, width=2];" << std::endl;

  dotFile << std::endl;
  for (int i=0; i<outputsGraph.edges.size(); ++i)
  {
    int output = outputsGraph.edges[i].first;
    int input = outputsGraph.edges[i].second;

    if (outputsGraph.nodes[output].isOutput() && outputsGraph.nodes[input].isInput())
    {
      dotFile << "  " << outputsGraph.nodes[output].getFMUInstanceName() << " -> " << outputsGraph.nodes[input].getFMUInstanceName() << " [label=\"" << outputsGraph.nodes[output].getName() << " -> " << outputsGraph.nodes[input].getName() << "\"];" << std::endl;
    }
  }

  dotFile << "}" << std::endl;
  dotFile.close();
}

void CompositeModel::exportXML(const char* filename)
{
  pugi::xml_document doc;

  // generate XML declaration
  pugi::xml_node declarationNode = doc.append_child(pugi::node_declaration);
  declarationNode.append_attribute("version") = "1.0";
  declarationNode.append_attribute("encoding") = "UTF-8";
  pugi::xml_node model = doc.append_child("Model");
  pugi::xml_node submodels = model.append_child("SubModels");
  pugi::xml_node connections = model.append_child("Connections");
  pugi::xml_node simulationparams = model.append_child("SimulationParams");

  // add simulation settings
  std::string startTime = std::to_string(settings.GetStartTime());
  std::string stopTime = std::to_string(settings.GetStopTime());
  std::string tolerance = std::to_string(settings.GetTolerance());
  std::string communicationInterval = std::to_string(settings.GetCommunicationInterval());

  simulationparams.append_attribute("StartTime") = startTime.c_str();
  simulationparams.append_attribute("StopTime") = stopTime.c_str();
  simulationparams.append_attribute("tolerance") = tolerance.c_str();
  simulationparams.append_attribute("communicationInterval") = communicationInterval.c_str();
  simulationparams.append_attribute("variableFilter") = ".*";

  // list all lookup tables
  for (auto it=lookupTables.begin(); it != lookupTables.end(); it++)
  {
    pugi::xml_node submodel = submodels.append_child("SubModel");
    submodel.append_attribute("Name") = it->first.c_str();
    submodel.append_attribute("ModelFile") = it->second->getFilename().c_str();
  }

  // list all FMUs
  for (auto it=fmuInstances.begin(); it != fmuInstances.end(); it++)
  {
    std::string getfmu = it->second->getFMUPath();
    std::string getinstance = it->second->getFMUInstanceName();
    pugi::xml_node submodel = submodels.append_child("SubModel");
    submodel.append_attribute("Name") = getinstance.c_str();
    submodel.append_attribute("ModelFile") = getfmu.c_str();
    if (it->second->isFMUKindME())
    {
      std::string getsolver= it->second->GetSolverMethodString();
      submodel.append_attribute("solver") = getsolver.c_str();
    }
  }

  for (int i=0; i<lookupAssignments.size(); ++i)
  {
    std::string tofmu = lookupAssignments[i].second->getFMUInstanceName() + '.' + lookupAssignments[i].second->getName();

    pugi::xml_node connection = connections.append_child("Connection");
    connection.append_attribute("From") = lookupAssignments[i].first.c_str();
    connection.append_attribute("To") = tofmu.c_str();
  }

  // add connection information
  const std::vector< std::vector< std::pair<int, int> > >& connectionsOutputs = outputsGraph.getSortedConnections();
  for(int i=0; i<connectionsOutputs.size(); i++)
  {
    for(int j=0; j<connectionsOutputs[i].size(); j++)
    {
      int output = connectionsOutputs[i][j].first;
      int input = connectionsOutputs[i][j].second;
      std::string outputFMU = outputsGraph.nodes[output].getFMUInstanceName();
      std::string outputVar = outputsGraph.nodes[output].getName();
      std::string inputFMU = outputsGraph.nodes[input].getFMUInstanceName();
      std::string inputVar = outputsGraph.nodes[input].getName();
      std::string fromfmu = outputFMU + '.' + outputVar;
      std::string tofmu = inputFMU + '.' + inputVar;

      pugi::xml_node connection = connections.append_child("Connection");
      connection.append_attribute("From") = fromfmu.c_str();
      connection.append_attribute("To") = tofmu.c_str();
    }
  }

  // add simulation parameters
  for (auto realParam = realParameterList.begin(); realParam != realParameterList.end(); ++realParam)
  {
    std::string varname = realParam->first;
    double varvalue = realParam->second;

    std::string fmuInstance;
    std::string fmuVar;
    std::stringstream var_(varname);
    std::getline(var_, fmuInstance, '.');
    std::getline(var_, fmuVar);

    // find the appropriate node instances and add the ModelParams
    pugi::xml_node param = submodels.find_child_by_attribute("SubModel", "Name", fmuInstance.c_str());
    pugi::xml_node modelparams = param.append_child("ModelParams");
    modelparams.append_attribute("Name") = fmuVar.c_str();
    modelparams.append_attribute("Value") = varvalue;
  }
  for (auto integerParam = integerParameterList.begin(); integerParam != integerParameterList.end(); ++integerParam)
  {
    std::string varname = integerParam->first;
    int varvalue = integerParam->second;

    std::string fmuInstance;
    std::string fmuVar;
    std::stringstream var_(varname);
    std::getline(var_, fmuInstance, '.');
    std::getline(var_, fmuVar);

    // find the appropriate node instances and add the ModelParams
    pugi::xml_node param = submodels.find_child_by_attribute("SubModel", "Name", fmuInstance.c_str());
    pugi::xml_node modelparams = param.append_child("ModelParams");
    modelparams.append_attribute("Name") = fmuVar.c_str();
    modelparams.append_attribute("Value") = varvalue;
  }
  for (auto booleanParam = booleanParameterList.begin(); booleanParam != booleanParameterList.end(); ++booleanParam)
  {
    std::string varname = booleanParam->first;
    int varvalue = booleanParam->second;

    std::string fmuInstance;
    std::string fmuVar;
    std::stringstream var_(varname);
    std::getline(var_, fmuInstance, '.');
    std::getline(var_, fmuVar);

    // find the appropriate node instances and add the ModelParams
    pugi::xml_node param = submodels.find_child_by_attribute("SubModel", "Name", fmuInstance.c_str());
    pugi::xml_node modelparams = param.append_child("ModelParams");
    modelparams.append_attribute("Name") = fmuVar.c_str();
    modelparams.append_attribute("Value") = varvalue;
  }

  bool saveSucceeded = doc.save_file(filename);
  if (!saveSucceeded)
    logError("CompositeModel::exportXML: The file is not saved to XML.");
}

void CompositeModel::importXML(const char* filename)
{
  OMS_TIC(globalClocks, GLOBALCLOCK_INSTANTIATION);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result)
  {
    logFatal("CompositeModel::importXML: \"" + std::string(filename) + "\" the file is not loaded");
  }

  pugi::xml_node root = doc.document_element();
  pugi::xml_node submodel = root.child("SubModels");
  pugi::xml_node connection = root.child("Connections");
  pugi::xml_node SimulationParams = root.child("SimulationParams");

  // read interfaces (for interfacing with other master simulators)
  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if(name == "Interface")
    {
      std::string InterfaceName, VariableName;
      for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
      {
        std::string value =ait->name();
        if (value == "Name")
          InterfaceName = ait->value();
        if (value == "Variable")
          VariableName = ait->value();
      }

      interfaceNames.push_back(InterfaceName);
      interfaceVariables.push_back(VariableName);
    }
  }

  // instantiate FMUs after reading from xml
  for (pugi::xml_node_iterator it = submodel.begin(); it != submodel.end(); ++it)
  {
    std::string instancename;
    std::string filename;
    std::string solvername;
    std::string type;
    for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
    {
      std::string value =ait->name();
      if (value == "Name")
      {
        instancename = ait->value();
      }
      else if (value == "ModelFile")
      {
        filename = ait->value();

        if (filename.length() > 5)
          type = filename.substr(filename.length() - 4);
        else
          logFatal("CompositeModel::importXML: Invalid filename: " + filename);
      }
      else if (value == "solver")
      {
        solvername = ait->value();
      }
    }

    if (type == ".fmu" || type == ".FMU")
      instantiateFMU(filename, instancename);
    else
      instantiateTable(filename, instancename);
    if (solvername != "")
    {
      fmuInstances[instancename]->SetSolverMethod(solvername);
    }

    // read and set the parameter from the node instances
    for (pugi::xml_node modelparam = it->first_child(); modelparam; modelparam = modelparam.next_sibling())
    {
      std::string name = modelparam.attribute("Name").as_string();
      double varvalue = modelparam.attribute("Value").as_double();
      std::string varname = instancename + "." + name;
      setReal(varname, varvalue);
    }
  }

  // add connections to FMus after reading from xml
  for (pugi::xml_node_iterator it = connection.begin(); it != connection.end(); ++it)
  {
    std::string fromconnection;
    std::string toconnection;
    for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
    {
      std::string value =ait->name();
      if (value == "From")
      {
        fromconnection = ait->value();
      }
      if (value == "To")
      {
        toconnection = ait->value();
      }
    }
    addConnection(fromconnection, toconnection);
  }

  // read the simulation settings and set
  for (pugi::xml_attribute attr = SimulationParams.first_attribute(); attr; attr = attr.next_attribute())
  {
    std::string name = attr.name();
    std::string value = attr.value();
    if (name == "StartTime")
    {
      if (!value.empty())
        settings.SetStartTime(std::strtod(attr.value(), NULL));
    }
    else if (name == "StopTime")
    {
      if (!value.empty())
        settings.SetStopTime(std::strtod(attr.value(), NULL));
    }
    else if (name == "tolerance")
    {
      if (!value.empty())
        settings.SetTolerance(std::strtod(attr.value(), NULL));
    }
    else if (name == "communicationInterval")
    {
      if (!value.empty())
        settings.SetCommunicationInterval(std::strtod(attr.value(), NULL));
    }
    else if (name == "variableFilter")
    {
      setVariableFilter(".*", attr.value());
    }
  }

  OMS_TOC(globalClocks, GLOBALCLOCK_INSTANTIATION);
}

Variable* CompositeModel::getVariable(const std::string& name)
{
  std::stringstream name_(name);
  std::string fmuInstance;
  std::string fmuVar;

  std::getline(name_, fmuInstance, '.');
  std::getline(name_, fmuVar);

  if (fmuInstances.find(fmuInstance) == fmuInstances.end())
  {
    logError("CompositeModel::getVariable: FMU instance \"" + fmuInstance + "\" doesn't exist in model");
    return NULL;
  }

  return fmuInstances[fmuInstance]->getVariable(fmuVar);
}

void CompositeModel::describe()
{
  logTrace();

  std::unordered_map<std::string, FMUWrapper*>::iterator it;

  std::cout << "# FMU instances" << std::endl;
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
  {
    std::cout << it->first << std::endl;
    if (it->second->isFMUKindME())
      std::cout << "  - " << it->second->getFMUKind() << " (solver: " << it->second->GetSolverMethodString() << ")" << std::endl;
    else
      std::cout << "  - " << it->second->getFMUKind() << std::endl;
    std::cout << "  - path: " << it->second->getFMUPath() << std::endl;
    std::cout << "  - GUID: " << it->second->getGUID() << std::endl;
    std::cout << "  - tool: " << it->second->getGenerationTool() << std::endl;

    std::cout << "  - input interface:" << std::endl;
    std::vector<Variable>& allVariables = it->second->getAllVariables();
    for (int j=0; j<allVariables.size(); j++)
      if (allVariables[j].isInput())
        std::cout << "    - input " << allVariables[j].getName() << std::endl;

    std::cout << "  - output interface:" << std::endl;
    for (int j=0; j<allVariables.size(); j++)
      if (allVariables[j].isOutput())
        std::cout << "    - output " << allVariables[j].getName() << std::endl;

    std::cout << "  - parameters:" << std::endl;
    for (int j=0; j<allVariables.size(); j++)
      if (allVariables[j].isParameter())
        std::cout << "    - parameter " << allVariables[j].getName() << std::endl;
  }

  std::cout << "\n# Lookup tables" << std::endl;
  for (auto it=lookupTables.begin(); it != lookupTables.end(); ++it)
  {
    std::cout << it->first << std::endl;
    std::cout << "  - path: " << it->second->getFilename() << std::endl;
  }

  std::cout << "\n# Simulation settings" << std::endl;
  std::cout << "  - start time: " << settings.GetStartTime() << std::endl;
  std::cout << "  - stop time: " << settings.GetStopTime() << std::endl;
  std::cout << "  - tolerance: " << settings.GetTolerance() << std::endl;
  std::cout << "  - communication interval: " << settings.GetCommunicationInterval() << std::endl;
  std::cout << "  - result file: " << (settings.GetResultFile() ? settings.GetResultFile() : "<no result file>") << std::endl;
  //std::cout << "  - temp directory: " << settings.GetTempDirectory() << std::endl;

  std::cout << "\n# Composite structure" << std::endl;
  std::cout << "## External inputs" << std::endl;
  for(int i=0; i<lookupAssignments.size(); i++)
    std::cout << lookupAssignments[i].first << " -> " << lookupAssignments[i].second->getFMUInstanceName() << "." << lookupAssignments[i].second->getName() << std::endl;
  std::cout << "\n## Initialization" << std::endl;
  // calculate sorting
  const std::vector< std::vector< std::pair<int, int> > >& connectionsInitialUnknowns = initialUnknownsGraph.getSortedConnections();
  for(int i=0; i<connectionsInitialUnknowns.size(); i++)
  {
    if (connectionsInitialUnknowns[i].size() == 1)
    {
      int output = connectionsInitialUnknowns[i][0].first;
      int input = connectionsInitialUnknowns[i][0].second;
      std::string outputFMU = initialUnknownsGraph.nodes[output].getFMUInstanceName();
      std::string outputVar = initialUnknownsGraph.nodes[output].getName();
      std::string inputFMU = initialUnknownsGraph.nodes[input].getFMUInstanceName();
      std::string inputVar = initialUnknownsGraph.nodes[input].getName();
      std::cout << outputFMU << "." << outputVar << " -> " << inputFMU << "." << inputVar << std::endl;
    }
    else
    {
      // Alg. loop
      std::cout << "{";
      for(int j=0; j<connectionsInitialUnknowns[i].size(); j++)
      {
        int output = connectionsInitialUnknowns[i][j].first;
        int input = connectionsInitialUnknowns[i][j].second;
        if (j > 0)
          std::cout << "; ";
        std::string outputFMU = initialUnknownsGraph.nodes[output].getFMUInstanceName();
        std::string outputVar = initialUnknownsGraph.nodes[output].getName();
        std::string inputFMU = initialUnknownsGraph.nodes[input].getFMUInstanceName();
        std::string inputVar = initialUnknownsGraph.nodes[input].getName();
        std::cout << outputFMU << "." << outputVar << " -> " << inputFMU << "." << inputVar;
      }
      std::cout << "}" << std::endl;
    }
  }

  std::cout << "\n## Simulation" << std::endl;
  // calculate sorting
  const std::vector< std::vector< std::pair<int, int> > >& connectionsOutputs = outputsGraph.getSortedConnections();
  for(int i=0; i<connectionsOutputs.size(); i++)
  {
    if (connectionsOutputs[i].size() == 1)
    {
      int output = connectionsOutputs[i][0].first;
      int input = connectionsOutputs[i][0].second;
      std::string outputFMU = outputsGraph.nodes[output].getFMUInstanceName();
      std::string outputVar = outputsGraph.nodes[output].getName();
      std::string inputFMU = outputsGraph.nodes[input].getFMUInstanceName();
      std::string inputVar = outputsGraph.nodes[input].getName();
      std::cout << outputFMU << "." << outputVar << " -> " << inputFMU << "." << inputVar << std::endl;
    }
    else
    {
      // Alg. loop
      std::cout << "{";
      for(int j=0; j<connectionsOutputs[i].size(); j++)
      {
        int output = connectionsOutputs[i][j].first;
        int input = connectionsOutputs[i][j].second;
        if (j > 0)
          std::cout << "; ";
        std::string outputFMU = outputsGraph.nodes[output].getFMUInstanceName();
        std::string outputVar = outputsGraph.nodes[output].getName();
        std::string inputFMU = outputsGraph.nodes[input].getFMUInstanceName();
        std::string inputVar = outputsGraph.nodes[input].getName();
        std::cout << outputFMU << "." << outputVar << " -> " << inputFMU << "." << inputVar;
      }
      std::cout << "}" << std::endl;
    }
  }

  std::cout << std::endl;
}

void CompositeModel::solveAlgLoop(DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC)
{
  const int size = SCC.size();
  const double tolerance = settings.GetTolerance();
  const int maxIterations = 100;
  double maxRes;
  double *res = new double[size]();

  int it=0;
  do
  {
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      const std::string& outputFMU = graph.nodes[output].getFMUInstanceName();
      res[i] = fmuInstances[outputFMU]->getReal(graph.nodes[output]);
    }

    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC[i].second;
      setRealInput(graph.nodes[input], res[i]);
    }

    // calculate residuals
    maxRes = 0.0;
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      const std::string& outputFMU = graph.nodes[output].getFMUInstanceName();
      res[i] -= fmuInstances[outputFMU]->getReal(graph.nodes[output]);

      if (fabs(res[i]) > maxRes)
        maxRes = fabs(res[i]);
    }
  } while(maxRes > tolerance && it < maxIterations);

  delete[] res;

  if (it >= maxIterations)
    logFatal("CompositeModel::solveAlgLoop: max. number of iterations (" + std::to_string(maxIterations) + ") exceeded at time = " + std::to_string(tcur));

  logDebug("CompositeModel::solveAlgLoop: maxRes: " + std::to_string(maxRes) + ", iterations: " + std::to_string(it) + " at time = " + std::to_string(tcur));
}

void CompositeModel::updateInputs(DirectedGraph& graph)
{
  OMS_TIC(globalClocks, GLOBALCLOCK_COMMUNICATION);

  // lookup tables
  for(int i=0; i<lookupAssignments.size(); i++)
  {
    std::stringstream name_(lookupAssignments[i].first);
    std::string lookupInstance;
    std::string var;

    std::getline(name_, lookupInstance, '.');
    std::getline(name_, var);

    double value = lookupTables[lookupInstance]->getValue(var, tcur);
    setRealInput(*lookupAssignments[i].second, value);
  }

  // input = output
  const std::vector< std::vector< std::pair<int, int> > >& sortedConnections = graph.getSortedConnections();
  for(int i=0; i<sortedConnections.size(); i++)
  {
    if (sortedConnections[i].size() == 1)
    {
      int output = sortedConnections[i][0].first;
      int input = sortedConnections[i][0].second;
      const std::string& outputFMU = graph.nodes[output].getFMUInstanceName();
      //std::string& outputVar = graph.nodes[output].getName();
      //const std::string& inputFMU = graph.nodes[input].getFMUInstanceName();
      //std::string& inputVar = graph.nodes[input].getName();
      double value = fmuInstances[outputFMU]->getReal(graph.nodes[output]);
      setRealInput(graph.nodes[input], value);
      //std::cout << inputFMU << "." << inputVar << " = " << outputFMU << "." << outputVar << std::endl;
    }
    else
    {
      solveAlgLoop(graph, sortedConnections[i]);
    }
  }

  OMS_TOC(globalClocks, GLOBALCLOCK_COMMUNICATION);
}

oms_status_t CompositeModel::simulate()
{
  logTrace();

  if (oms_modelState_simulation != modelState)
  {
    logError("CompositeModel::simulate: Model is not in simulation mode.");
    return oms_status_error;
  }

  fmi2_real_t tend = settings.GetStopTime();

  return stepUntil(tend);
}

void CompositeModel::emit()
{
  if (!resultFile)
    return;

  OMS_TIC(globalClocks, GLOBALCLOCK_RESULTFILE);

  // update all signals
  for (auto it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    it->second->updateSignalsForResultFile(resultFile);

  resultFile->emit(tcur);

  OMS_TOC(globalClocks, GLOBALCLOCK_RESULTFILE);
}

oms_status_t CompositeModel::doSteps(const int numberOfSteps)
{
  logTrace();

  if (oms_modelState_simulation != modelState)
  {
    logError("CompositeModel::doSteps: Model is not in simulation mode.");
    return oms_status_error;
  }

  for(int step=0; step<numberOfSteps; step++)
  {
    // do_step
    std::unordered_map<std::string, FMUWrapper*>::iterator it;
    for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
      it->second->doStep(tcur+communicationInterval);
    tcur += communicationInterval;
    emit();

    // input = output
    updateInputs(outputsGraph);
    emit();
  }

  return oms_status_ok;
}

oms_status_t CompositeModel::stepUntil(const double timeValue)
{
  logTrace();

  if (oms_modelState_simulation != modelState)
  {
    logError("CompositeModel::stepUntil: Model is not in simulation mode.");
    return oms_status_error;
  }

  while(tcur < timeValue)
  {
    tcur += communicationInterval;
    if (tcur > timeValue)
      tcur = timeValue;

    // do_step
    std::unordered_map<std::string, FMUWrapper*>::iterator it;
    for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
      it->second->doStep(tcur);
    emit();

    // input = output
    updateInputs(outputsGraph);
    emit();
  }

  return oms_status_ok;
}

void CompositeModel::initialize()
{
  logTrace();

  OMS_TIC(globalClocks, GLOBALCLOCK_INITIALIZATION);

  if (oms_modelState_instantiated != modelState)
  {
    logFatal("CompositeModel::initialize: Model is already in simulation mode.");
  }

  tcur = settings.GetStartTime();
  communicationInterval = settings.GetCommunicationInterval();

  // Enter initialization
  modelState = oms_modelState_initialization;
  std::unordered_map<std::string, FMUWrapper*>::iterator it;
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    it->second->enterInitialization(tcur);

  updateInputs(initialUnknownsGraph);

  // Exit initialization
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    it->second->exitInitialization();
  modelState = oms_modelState_simulation;

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }
  if (settings.GetResultFile())
  {
    OMS_TIC(globalClocks, GLOBALCLOCK_RESULTFILE);
    std::string extension = boost::filesystem::extension(settings.GetResultFile());

    if (".csv" == extension)
      resultFile = new CSVWriter(1);
    else if (".mat" == extension)
      resultFile = new MATWriter(1024);
    else
      logWarning("Unknown result file type: " + extension);

    if (resultFile)
    {
      logInfo("Result file: " + std::string(settings.GetResultFile()));

      // add all signals
      for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
        it->second->registerSignalsForResultFile(resultFile);

      // create result file
      bool rc = resultFile->create(settings.GetResultFile(), tcur, settings.GetStopTime());
      if (!rc)
      {
        delete resultFile;
        resultFile = NULL;
      }
      emit();
    }
    OMS_TOC(globalClocks, GLOBALCLOCK_RESULTFILE);
  }

  OMS_TOC(globalClocks, GLOBALCLOCK_INITIALIZATION);
  OMS_TIC(globalClocks, GLOBALCLOCK_SIMULATION);
}

void CompositeModel::terminate()
{
  logTrace();

  if (oms_modelState_initialization != modelState &&
      oms_modelState_simulation != modelState)
  {
    logInfo("CompositeModel::terminate: No simulation to terminate.");
    return;
  }

  std::unordered_map<std::string, FMUWrapper*>::iterator it;
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    it->second->terminate();

  if (resultFile)
  {
    resultFile->close();
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_instantiated;

  OMS_TOC(globalClocks, GLOBALCLOCK_SIMULATION);
  logInfo("Simulation finished.");

  double cpuStats[GLOBALCLOCK_MAX_INDEX+1];
  globalClocks.getStats(cpuStats, NULL);
  logInfo("time measurement for composite model");
  logInfo("  total: " + std::to_string(cpuStats[GLOBALCLOCK_MAX_INDEX]) + "s");
  for (int i=0; i<GLOBALCLOCK_MAX_INDEX; ++i)
    logInfo("  " + std::string(GlobalClockNames[i]) + ": " + std::to_string(cpuStats[i]) + "s [" + std::to_string(cpuStats[i]/cpuStats[GLOBALCLOCK_MAX_INDEX]*100.0) + "%]");
}

void CompositeModel::reset()
{
  logTrace();
  OMS_TOC(globalClocks, GLOBALCLOCK_SIMULATION);

  std::unordered_map<std::string, FMUWrapper*>::iterator it;
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    it->second->reset();

  modelState = oms_modelState_instantiated;
}

oms_status_t CompositeModel::getCurrentTime(double *time)
{
  logTrace();

  if (oms_modelState_initialization != modelState &&
      oms_modelState_simulation != modelState)
  {
    logError("It is only allowed to call 'getCurrentTime' while running a simulation.");
    return oms_status_error;
  }

  *time = tcur;
  return oms_status_ok;
}

void CompositeModel::SetSolverMethod(std::string instanceName, std::string method)
{
  if (fmuInstances.find(instanceName) == fmuInstances.end())
  {
    logError("CompositeModel::SetSolverMethod: FMU instance \"" + instanceName + "\" doesn't exist in model");
    return;
  }

  fmuInstances[instanceName]->SetSolverMethod(method);
}

void CompositeModel::setVariableFilter(const char* instanceFilter, const char* variableFilter)
{
  std::regex exp(instanceFilter);

  std::unordered_map<std::string, FMUWrapper*>::iterator it;
  for (it=fmuInstances.begin(); it != fmuInstances.end(); it++)
    if (std::regex_match(it->first, exp))
      it->second->setVariableFilter(variableFilter);
}

int CompositeModel::getNumberOfInterfaces()
{
  return interfaceNames.size();
}

oms_causality_t CompositeModel::getInterfaceCausality(int idx)
{
  std::string variableName = interfaceVariables.at(idx);
  std::string fmuInstance;
  std::string fmuVar;
  std::stringstream var_(variableName);
  std::getline(var_, fmuInstance, '.');
  std::getline(var_, fmuVar);

  Variable* var = fmuInstances[fmuInstance]->getVariable(fmuVar);
  if(var->isInput())
    return oms_causality_input;
  else if(var->isOutput())
    return oms_causality_output;
  else if(var->isParameter())
    return oms_causality_parameter;

  return oms_causality_undefined;
}

const char* CompositeModel::getInterfaceName(int idx)
{
  if(interfaceNames.size() <= idx) {
    logError("Query for interface name for non-existing interface.");
    return NULL;
  }

  return interfaceNames.at(idx).c_str();
}

const char* CompositeModel::getInterfaceVariable(int idx)
{
  if(interfaceVariables.size() <= idx) {
    logError("Query for interface variable for non-existing interface.");
    return NULL;
  }

  return interfaceVariables.at(idx).c_str();
}


