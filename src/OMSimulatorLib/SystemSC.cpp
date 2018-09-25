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

#include "SystemSC.h"

#include "Component.h"
#include "Model.h"
#include "Types.h"
#include "ssd/Tags.h"

oms3::SystemSC::SystemSC(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms3::System(cref, oms_system_sc, parentModel, parentSystem)
{
}

oms3::SystemSC::~SystemSC()
{
}

oms3::System* oms3::SystemSC::NewSystem(const oms3::ComRef& cref, oms3::Model* parentModel, oms3::System* parentSystem)
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

  System* system = new SystemSC(cref, parentModel, parentSystem);
  return system;
}

oms_status_enu_t oms3::SystemSC::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms2::ssd::ssd_simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child("VariableStepSolver");
  node_solver.append_attribute("description") = solverName.c_str();
  node_solver.append_attribute("absoluteTolerance") = std::to_string(absoluteTolerance).c_str();
  node_solver.append_attribute("relativeTolerance") = std::to_string(relativeTolerance).c_str();
  node_solver.append_attribute("minimumStepSize") = std::to_string(minimumStepSize).c_str();
  node_solver.append_attribute("maximumStepSize") = std::to_string(maximumStepSize).c_str();
  node_solver.append_attribute("initialStepSize") = std::to_string(initialStepSize).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemSC::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  solverName = node.child("VariableStepSolver").attribute("description").as_string();
  absoluteTolerance = node.child("VariableStepSolver").attribute("absoluteTolerance").as_double();
  relativeTolerance = node.child("VariableStepSolver").attribute("relativeTolerance").as_double();
  minimumStepSize = node.child("VariableStepSolver").attribute("minimumStepSize").as_double();
  maximumStepSize = node.child("VariableStepSolver").attribute("maximumStepSize").as_double();
  initialStepSize = node.child("VariableStepSolver").attribute("initialStepSize").as_double();
  return oms_status_ok;
}
