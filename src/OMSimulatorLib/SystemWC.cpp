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

#include "SystemWC.h"

#include "Component.h"
#include "ComponentFMUCS.h"
#include "Model.h"
#include "Types.h"
#include "ssd/Tags.h"

oms3::SystemWC::SystemWC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms3::System(cref, oms_system_wc, parentModel, parentSystem)
{
}

oms3::SystemWC::~SystemWC()
{
}

oms3::System* oms3::SystemWC::NewSystem(const oms3::ComRef& cref, oms3::Model* parentModel, oms3::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError("\"" + std::string(cref) + "\" is not a valid ident");
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError("Internal error");
    return NULL;
  }

  System* system = new SystemWC(cref, parentModel, parentSystem);
  return system;
}

oms_status_enu_t oms3::SystemWC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms2::ssd::ssd_simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child("FixedStepMaster");
  node_solver.append_attribute("description") = solverName.c_str();
  node_solver.append_attribute("stepSize") = std::to_string(stepSize).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  solverName = node.child("FixedStepMaster").attribute("description").as_string();
  stepSize = node.child("FixedStepMaster").attribute("stepSize").as_double();
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::instantiate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->instantiate())
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::initialize()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->terminate())
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::stepUntil(double stopTime)
{
  while (time < stopTime)
  {
    double tNext = time+stepSize;
    if (tNext > stopTime)
      tNext = stopTime;

    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(tNext));

    for (const auto& subsystem : getSubSystems())
      if (oms_status_ok != subsystem.second->stepUntil(tNext))
        return oms_status_error;

    for (const auto& component : getComponents())
    {
      if (component.second->getType() != oms_component_fmu)
        return logError("Unexpected component type");

      if (oms_status_ok != dynamic_cast<ComponentFMUCS*>(component.second)->stepUntil(tNext))
        return oms_status_error;
    }

    updateInputs(outputsGraph, false);
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemWC::updateInputs(oms3::DirectedGraph& graph, bool discrete)
{
  // input := output
  const std::vector< std::vector< std::pair<int, int> > >& sortedConnections = graph.getSortedConnections();
  for(int i=0; i<sortedConnections.size(); i++)
  {
    if (sortedConnections[i].size() == 1)
    {
      int output = sortedConnections[i][0].first;
      int input = sortedConnections[i][0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        getReal(graph.getNodes()[output].getName(), value);
        setReal(graph.getNodes()[input].getName(), value);
      }
      else
        return logError("Only continuous connections are supported yet");
    }
    else
    {
      return logError("Alg. loops are not supported yet");
    }
  }
  return oms_status_ok;
}
