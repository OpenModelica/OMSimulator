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
#include "System.h"
#include "TLMBusConnector.h"
#include <OMSFileSystem.h>

void oms::fmiLogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
  switch (log_level)
  {
  case jm_log_level_info:    // Informative messages
    logDebug("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_warning: // Non-critical issues
    logWarning("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_error:   // Errors that may be not critical for some FMUs
  case jm_log_level_fatal:   // Unrecoverable errors
    logError("module " + std::string(module) + ": " + std::string(message));
    break;
  case jm_log_level_verbose: // Verbose messages
  case jm_log_level_debug:   // Debug messages. Only enabled if library is configured with FMILIB_ENABLE_LOG_LEVEL_DEBUG
    logDebug("[log level " + std::string(jm_log_level_to_string(log_level)) + "] module " + std::string(module) + ": " + std::string(message));
  }
}

void oms::fmi2logger(fmi2_component_environment_t env, fmi2_string_t instanceName, fmi2_status_t status, fmi2_string_t category, fmi2_string_t message, ...)
{
  if ((status == fmi2_status_ok || status == fmi2_status_pending) && !logDebugEnabled())
  {
    // When frequently called for debug logging during simulation, avoid costly formatting.
    return;
  }

  int len;
  char msg[1000];
  va_list argp;
  va_start(argp, message);
  len = vsnprintf(msg, 1000, message, argp);

  switch (status)
  {
  case fmi2_status_ok:
  case fmi2_status_pending:
    logDebug(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  case fmi2_status_warning:
    logWarning(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  case fmi2_status_discard:
  case fmi2_status_error:
  case fmi2_status_fatal:
    logError(std::string(instanceName) + " (" + category + "): " + msg);
    break;
  default:
    logWarning("fmiStatus = " + std::string(fmi2_status_to_string(status)) + "; " + instanceName + " (" + category + "): " + msg);
  }
}

oms::Component::Component(const ComRef& cref, oms_component_enu_t type, System* parentSystem, const std::string& path)
  : element(oms_element_component, cref), cref(cref), type(type), parentSystem(parentSystem), path(path)
{
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

#if !defined(NO_TLM)
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
#endif
}

oms::Component::~Component()
{
  for (const auto& connector : connectors)
    if (connector)
      delete connector;

#if !defined(NO_TLM)
  for (const auto tlmbusconnector : tlmbusconnectors)
    if(tlmbusconnector)
      delete tlmbusconnector;
#endif

  deleteResources();
}

oms::ComRef oms::Component::getFullCref() const
{
  return parentSystem->getFullCref() + cref;
}

oms::Model* oms::Component::getModel() const
{
  return parentSystem ? parentSystem->getModel() : NULL;
}

oms_status_enu_t oms::Component::addTLMBus(const oms::ComRef &cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
#if !defined(NO_TLM)
  if(!cref.isValidIdent()) {
    return logError("Not a valid ident: "+std::string(cref));
  }
  oms::TLMBusConnector* bus = new oms::TLMBusConnector(cref, domain, dimensions, interpolation,nullptr,this);
  tlmbusconnectors.back() = bus;
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
  return oms_status_ok;
#else
  return LOG_NO_TLM();
#endif
}

#if !defined(NO_TLM)
oms::TLMBusConnector *oms::Component::getTLMBusConnector(const oms::ComRef &cref)
{
  for(auto &tlmbusconnector : tlmbusconnectors) {
    if(tlmbusconnector && tlmbusconnector->getName() == cref)
      return tlmbusconnector;
  }
  return NULL;
}
#endif

oms_status_enu_t oms::Component::addConnectorToTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref, const std::string type)
{
#if !defined(NO_TLM)

  //Check that connector exists in component
  bool found = false;
  for(auto& connector : connectors)
    if (connector && connector->getName() == connectorCref)
      found = true;
  if (!found)
    return logError_ConnectorNotInComponent(connectorCref, this);

  for(auto& bus : tlmbusconnectors)
  {
    if (bus && bus->getName() == busCref)
    {
      return bus->addConnector(connectorCref,type);
    }
  }
  return logError_TlmBusNotInComponent(busCref, this);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::Component::deleteConnectorFromTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
#if !defined(NO_TLM)
  for(auto& bus : tlmbusconnectors)
    if(bus && bus->getName() == busCref)
      return bus->deleteConnector(connectorCref);

  return logError_BusNotInComponent(busCref, this);
#else
  return LOG_NO_TLM();
#endif
}

oms::Connector* oms::Component::getConnector(const ComRef& cref)
{
  for (auto &connector : connectors)
  {
    if (connector && connector->getName() == cref)
      return connector;
  }

  return NULL;
}

oms_status_enu_t oms::Component::deleteConnector(const ComRef& cref)
{
  for (int i=0; i<connectors.size(); ++i)
  {
    if (connectors[i] && connectors[i]->getName() == cref)
    {
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

oms_status_enu_t oms::Component::deleteResources()
{
  // delete temp directory
  if (Flags::DeleteTempFiles())
  {
    if (!tempDir.empty() && filesystem::is_directory(tempDir))
    {
      try
      {
        filesystem::remove_all(tempDir);
        logDebug("removed temp directory: \"" + tempDir + "\"");
      }
      catch (const std::exception& e)
      {
        logWarning("temp directory \"" + tempDir + "\" couldn't be removed\n" + e.what());
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Component::faultInjection(const oms::ComRef& signal, oms_fault_type_enu_t faultType, double faultValue)
{
  return oms_status_error;
}
