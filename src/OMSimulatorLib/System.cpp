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

#include "System.h"

#include "Component.h"
#include "Model.h"
#include "SystemSC.h"
#include "SystemTLM.h"
#include "SystemWC.h"
#include "Types.h"
#include "ssd/Tags.h"

oms3::System::System(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
  : element(oms_element_system, cref), cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem)
{
}

oms3::System::~System()
{
  for (const auto& component : components)
    delete component.second;

  for (const auto& subsystem : subsystems)
    delete subsystem.second;
}

oms3::System* oms3::System::NewSystem(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError("\"" + std::string(cref) + "\" is not a valid ident");
    return NULL;
  }

  if (parentSystem && parentSystem->getSystem(cref))
  {
    logError("Name is already in use");
    return NULL;
  }

  if (parentSystem && parentSystem->getComponent(cref))
  {
    logError("Name is already in use");
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError("Internal error");
    return NULL;
  }

  switch (type)
  {
  case oms_system_tlm:
    if (parentSystem)
    {
      logError("A TLM system must be the the root system of a model.");
      return NULL;
    }
    return SystemTLM::NewSystem(cref, parentModel, parentSystem);

  case oms_system_wc:
    if (parentSystem && oms_system_tlm != parentSystem->getType())
    {
      logError("A WC system must be the root system or a subsystem of a TLM system.");
      return NULL;
    }
    return SystemWC::NewSystem(cref, parentModel, parentSystem);

  case oms_system_sc:
    if (parentSystem && oms_system_wc != parentSystem->getType())
    {
      logError("A SC system must be the root system or a subsystem of a WC system.");
      return NULL;
    }
    return SystemSC::NewSystem(cref, parentModel, parentSystem);
  }

  logError("Internal error");
  return NULL;
}

oms3::ComRef oms3::System::getFullName()
{
  if (parentSystem)
    return parentSystem->getFullName() + this->getName();
  if (parentModel)
    return ComRef(parentModel->getName()) + this->getName();

  logError("Internal error");
  return this->getName();
}

oms3::System* oms3::System::getSystem(const oms3::ComRef& cref)
{
  oms3::System* system = NULL;

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  auto it = subsystems.find(front);
  if (it == subsystems.end())
    return NULL;

  if (tail.isEmpty())
    return it->second;

  return it->second->getSystem(tail);
}

oms3::Component* oms3::System::getComponent(const oms3::ComRef& cref)
{
  auto it = components.find(cref);
  if (it == components.end())
    return NULL;

  return it->second;
}

bool oms3::System::validCref(const oms3::ComRef& cref)
{
  if (!cref.isValidIdent())
    return false;

  if (getSystem(cref))
    return false;

  if (getComponent(cref))
    return false;

  return true;
}

oms_status_enu_t oms3::System::addSubSystem(const oms3::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isValidIdent())
  {
    System* system = System::NewSystem(cref, type, NULL, this);
    if (system)
      subsystems[cref] = system;
    return system ? oms_status_ok : oms_status_error;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullName()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubSystem(tail, type);
}

oms_status_enu_t oms3::System::list(const oms3::ComRef& cref, char** contents)
{
  return logError("not implemented");
}

oms_status_enu_t oms3::System::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getName().c_str();

  for (const auto& subsystem : subsystems)
  {
    pugi::xml_node system_node = node.append_child(oms2::ssd::ssd_system);
    if (oms_status_ok != subsystem.second->exportToSSD(system_node))
      return logError("export of system failed");
  }

  for (const auto& component : components)
  {
    pugi::xml_node component_node = node.append_child(oms2::ssd::ssd_component);
    if (oms_status_ok != component.second->exportToSSD(component_node))
      return logError("export of component failed");
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::System::addConnector(const oms3::ComRef &cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if(subsystem != subsystems.end()) {
    return subsystem->second->addConnector(tail,causality,type);
  }

  if(!cref.isValidIdent()) {
    return logError("Not a valid ident: "+std::string(cref));
  }

  oms3::Connector connector(causality, type, cref);
  this->getElement()->addConnector(connector);

  return oms_status_ok;
}
