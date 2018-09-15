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

oms3::System::System(const oms3::ComRef& cref, oms_system_enu_t type, oms3::Model* parentModel, oms3::System* parentSystem)
  : cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem)
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
    return SystemTLM::NewSystem(cref, parentModel, parentSystem);

  case oms_system_wc:
    return SystemWC::NewSystem(cref, parentModel, parentSystem);

  case oms_system_sc:
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
  auto it = subsystems.find(cref);
  if (it == subsystems.end())
    return NULL;

  return it->second;
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

oms_status_enu_t oms3::System::addSystem(const oms3::ComRef& cref, oms_system_enu_t type, Model* parentModel, System* parentSystem)
{
  if (cref.isValidIdent())
  {
    System* system = System::NewSystem(cref, type, parentModel, parentSystem);
    if (system)
      subsystems[cref] = system;
    return system ? oms_status_ok : oms_status_error;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullName()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSystem(tail, type, parentModel, parentSystem);
}
