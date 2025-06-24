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

#include "oms3/Model.h"
#include "Logging.h"
#include "ComRef.h"
#include <iostream>
using namespace oms;

oms3::Model::Model(char *model_json_desc)
{
  try
  {
    nlohmann::json model_json;
    model_json = nlohmann::json::parse(model_json_desc);
    if (model_json.is_null())
    {
      logError("Model JSON description is null");
      return;
    }
    if (!model_json.contains("simulation units"))
    {
      logError("Model JSON description does not contain 'simulationUnits'");
      return;
    }
    parseSimulationUnits(model_json);
    dumpSimulationUnits();
  }
  catch(const std::exception& e)
  {
    logError("Failed to parse model JSON description: " + std::string(e.what()));
  }
}

oms3::Model::~Model()
{
}

void oms3::Model::parseSimulationUnits(nlohmann::json &model_json)
{
  for (const auto &unit_json : model_json["simulation units"])
  {
    if (!unit_json.contains("components"))
    {
      logError("Simulation unit JSON does not contain 'components'");
      return;
    }
    std::vector<oms3::Component> components;
    std::vector<std::string> variables;
    std::vector<oms::Connection> connections;
    oms3::Solver solver;
    if (unit_json.contains("components"))
    {
      for (const auto &component_json : unit_json["components"])
      {
        std::string name = component_json.value("name", "");
        std::string type = component_json.value("type", "");
        std::string path = component_json.value("path", "");
        if (component_json.contains("variables"))
        {
          for (const auto &var : component_json["variables"])
          {
            variables.push_back(var.get<std::string>());
          }
        }
        oms3::Component component = Component(name, type, path, variables);
        components.push_back(component);
      }
    }
    if (unit_json.contains("solver"))
    {
      std::string solverType = unit_json["solver"].value("type", "default");
      double stepSize = unit_json["solver"].value("stepSize", 0.01);
      double tolerance = unit_json["solver"].value("tolerance", 1e-6);
      solver = oms3::Solver(solverType, stepSize, tolerance);
    }
    else
    {
      // If no solver is specified, use a default solver
      solver = oms3::Solver("default", 0.01, 1e-6);
    }

    if (unit_json.contains("connections"))
    {
      for (const auto &connection_json : unit_json["connections"])
      {
        oms::ComRef startElement = oms::ComRef(connection_json.value("start element", ""));
        oms::ComRef startConnector = oms::ComRef(connection_json.value("start connector", ""));
        oms::ComRef endElement = oms::ComRef(connection_json.value("end element", ""));
        oms::ComRef endConnector = oms::ComRef(connection_json.value("end connector", ""));

        oms::ComRef crefA = startConnector;
        if (!startElement.isEmpty())
          crefA = startElement + startConnector;
        oms::ComRef crefB = endConnector;
        if (!endElement.isEmpty())
          crefB = endElement + endConnector;
        oms::Connection connection = oms::Connection(crefA, crefB, false);
        connections.push_back(connection);
      }
    }
    oms3::SimulationUnit simulationUnit = oms3::SimulationUnit(components, solver, connections);
    simulationUnits.push_back(simulationUnit);
  }
}

void oms3::Model::dumpSimulationUnits()
{
  logInfo("|-- Number of Simulation Units: " + std::to_string(simulationUnits.size()));
  for (const auto &unit : simulationUnits)
  {
    logInfo("|-- Components: " + std::to_string(unit.components.size()));
    for (const auto &component : unit.components)
    {
      logInfo("|-- |-- Name: " + component.name + ", Type: " + component.type + ", Path: " + component.path);
    }
    logInfo("|-- Solver:");
    logInfo("|-- |-- Type: " + unit.solver.type + ", Step Size: " + std::to_string(unit.solver.stepSize) +
            ", Tolerance: " + std::to_string(unit.solver.tolerance));
    logInfo("|-- Connections: " + std::to_string(unit.connections.size()));
    for (const auto &connection : unit.connections)
    {
      logInfo("|-- |-- " + std::string(connection.getSignalA()) + " --> " + std::string(connection.getSignalB()));
    }
  }
}
