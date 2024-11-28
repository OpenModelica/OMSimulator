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

#include "Component.h"

#include "Flags.h"
#include "Model.h"
#include "OMSFileSystem.h"
#include "System.h"

#include <cstring>
#include <stdarg.h>

void oms::fmi2logger(fmi2ComponentEnvironment env, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...)
{
  if ((status == fmi2OK || status == fmi2Pending) && !logDebugEnabled())
  {
    // When frequently called for debug logging during simulation, avoid costly formatting.
    return;
  }

  int len;
  char msg[1000];
  va_list argp;
  va_start(argp, message);
  len = vsnprintf(msg, 1000, message, argp);

  if (Flags::SuppressPath())
  {
    // Remove path information from instanceName
    const char* lastSlash = strrchr(instanceName, '/');
    const char* lastBackslash = strrchr(instanceName, '\\');
    const char* lastSeparator = lastSlash > lastBackslash ? lastSlash : lastBackslash;
    if (lastSeparator)
      instanceName = lastSeparator + 1;
  }

  switch (status)
  {
  case fmi2OK:
    logDebug("[fmi2OK] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  case fmi2Pending:
    logDebug("[fmi2Pending] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  case fmi2Warning:
    logWarning("[fmi2Warning] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  case fmi2Discard:
    logError("[fmi2Discard] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  case fmi2Error:
    logError("[fmi2Error] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  case fmi2Fatal:
    logError("[fmi2Fatal] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  default:
    logError("[unknown] " + std::string(instanceName) + "/" + std::string(category) + ": " + msg);
    break;
  }
}

oms::Component::Component(const ComRef& cref, oms_component_enu_t type, System* parentSystem, const std::string& path)
  : element(oms_element_component, cref), cref(cref), type(type), parentSystem(parentSystem), path(path)
{
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);
}

oms::Component::~Component()
{
  for (const auto& connector : connectors)
    if (connector)
      delete connector;
}

oms::ComRef oms::Component::getFullCref() const
{
  return parentSystem->getFullCref() + cref;
}

oms::Model& oms::Component::getModel() const
{
  return parentSystem->getModel();
}

oms::Connector* oms::Component::getConnector(const ComRef& cref)
{
  for (auto &connector : connectors)
    if (connector && connector->getName() == cref)
      return connector;

  return NULL;
}

oms_status_enu_t oms::Component::deleteConnector(const ComRef& cref)
{
  for (int i=0; i<connectors.size(); ++i)
  {
    if (connectors[i] && connectors[i]->getName() == cref)
    {
      // delete startValues associated with components connector
      Component* component = parentSystem->getComponent(getCref());
      component->deleteStartValue(cref);
      delete connectors[i];
      connectors.pop_back();
      connectors[i] = connectors.back();
      connectors.back() = NULL;
      element.setConnectors(&connectors[0]);
      return oms_status_ok;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms::Component::rename(const oms::ComRef& newCref)
{
  ComRef oldCref = this->cref;
  this->cref = newCref;
  this->renameValues(oldCref, newCref); // rename values in ssv files (only for FMUs)
  return oms_status_ok;
}
