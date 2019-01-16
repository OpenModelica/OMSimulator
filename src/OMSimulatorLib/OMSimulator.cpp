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

#include "OMSimulator.h"

#include "Component.h"
#include "ComRef.h"
#include "Element.h"
#include "Flags.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "MatReader.h"
#include "Model.h"
#include "ResultReader.h"
#include "Scope.h"
#include "System.h"
#if !defined(NO_TLM)
  #include "SystemTLM.h"
  #include "TLMBusConnector.h"
#endif
#include "Types.h"
#include "Version.h"
#include <boost/filesystem.hpp>
#include <miniunz.h>
#include <pugixml.hpp>
#include <string>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

/* ************************************ */
/* oms3                                 */
/*                                      */
/* ************************************ */

const char* oms3_getVersion()
{
  return oms_git_version;
}

void oms3_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message))
{
  Log::setLoggingCallback(cb);
}

oms_status_enu_t oms3_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
}

oms_status_enu_t oms3_setLoggingInterval(const char* cref_, double loggingInterval)
{
  oms3::ComRef cref(cref_);

  if (cref.isValidIdent())
  {
    oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
    if (!model)
      return logError_ModelNotInScope(cref);

    return model->setLoggingInterval(loggingInterval);
  }
  else
    return logError_OnlyForModel;
}

oms_status_enu_t oms3_setLoggingLevel(int logLevel)
{
  return Log::setLoggingLevel(logLevel);
}

void oms3_setMaxLogFileSize(const unsigned long size)
{
  Log::setMaxLogFileSize(size);
}

oms_status_enu_t oms3_setTempDirectory(const char* newTempDir)
{
  return oms3::Scope::GetInstance().setTempDirectory(newTempDir);
}

oms_status_enu_t oms3_setWorkingDirectory(const char* newWorkingDir)
{
  return oms3::Scope::GetInstance().setWorkingDirectory(newWorkingDir);
}

oms_status_enu_t oms3_newModel(const char* cref)
{
  return oms3::Scope::GetInstance().newModel(oms3::ComRef(cref));
}

oms_status_enu_t oms3_rename(const char* cref_, const char* newCref_)
{
  oms3::ComRef cref(cref_);
  oms3::ComRef newCref(newCref_);

  if (cref.isValidIdent())
    return oms3::Scope::GetInstance().renameModel(cref, newCref);
  else
    return logError_OnlyForModel;
}

int oms3_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol)
{
  oms3::ResultReader* readerA = oms3::ResultReader::newReader(filenameA);
  oms3::ResultReader* readerB = oms3::ResultReader::newReader(filenameB);

  oms3::ResultReader::Series* seriesA = readerA->getSeries(var);
  oms3::ResultReader::Series* seriesB = readerB->getSeries(var);

  //for (int i=0; i<seriesA->length; ++i)
  //  std::cout << seriesA->time[i] << " - " << seriesA->value[i] << std::endl;
  //for (int i=0; i<seriesB->length; ++i)
  //  std::cout << seriesB->time[i] << " - " << seriesB->value[i] << std::endl;

  bool rc = oms3::ResultReader::compareSeries(seriesA, seriesB, relTol, absTol);

  oms3::ResultReader::deleteSeries(&seriesA);
  oms3::ResultReader::deleteSeries(&seriesB);

  delete readerA;
  delete readerB;

  return rc ? 1 : 0;
}

oms_status_enu_t oms3_delete(const char* cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  if (tail.isEmpty())
    return oms3::Scope::GetInstance().deleteModel(front);
  else
    return model->delete_(tail);
}

oms_status_enu_t oms3_export(const char* cref_, const char* filename)
{
  return oms3::Scope::GetInstance().exportModel(oms3::ComRef(cref_), std::string(filename));
}

oms_status_enu_t oms3_import(const char* filename, char** cref)
{
  return oms3::Scope::GetInstance().importModel(std::string(filename), cref);
}

oms_status_enu_t oms3_list(const char* cref_, char** contents)
{
  oms3::ComRef tail(cref_);
  oms3::ComRef front = tail.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->list(tail, contents);
}

oms_status_enu_t oms3_listUnconnectedConnectors(const char* cref_, char** contents)
{
  oms3::ComRef tail(cref_);
  oms3::ComRef front = tail.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms3::System* system = model->getSystem(tail);
  if (!system)
    return logError_SystemNotInModel(front, tail);

  return system->listUnconnectedConnectors(contents);
}

oms_status_enu_t oms3_parseModelName(const char* contents, char** cref)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3_importString(const char* contents, char** cref)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3_addSystem(const char* cref_, oms_system_enu_t type)
{
  oms3::ComRef cref(cref_);
  oms3::ComRef modelCref = cref.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  return model->addSystem(cref, type);
}

oms_status_enu_t oms3_copySystem(const char* source, const char* target)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3_freeMemory(void* obj)
{
  logTrace();
  if (obj)
    free(obj);
  return oms_status_ok;
}

oms_status_enu_t oms3_getElement(const char* cref_, oms3_element_t** element)
{
  return oms3::Scope::GetInstance().getElement(oms3::ComRef(cref_), reinterpret_cast<oms3::Element**>(element));
}

oms_status_enu_t oms3_getElements(const char* cref, oms3_element_t*** elements)
{
  return oms3::Scope::GetInstance().getElements(oms3::ComRef(cref), reinterpret_cast<oms3::Element***>(elements));
}

oms_status_enu_t oms3_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry)
{
  if (!geometry)
  {
    logWarning("[oms3_setElementGeometry] NULL pointer");
    return oms_status_warning;
  }

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms3::System* system = model->getSystem(tail);
  if (system)
  {
    system->setGeometry(*reinterpret_cast<const oms3::ssd::ElementGeometry*>(geometry));
    return oms_status_ok;
  }

  oms3::Component* component = model->getComponent(tail);
  if (component)
  {
    component->setGeometry(*reinterpret_cast<const oms3::ssd::ElementGeometry*>(geometry));
    return oms_status_ok;
  }

  return logError("Model \"" + std::string(model->getCref()) + "\" does not contain system or component \"" + std::string(tail) + "\"");
}

oms_status_enu_t oms3_addConnector(const char *cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addConnector(tail, causality, type);
}

oms_status_enu_t oms3_getConnector(const char* cref, oms_connector_t** connector)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, tail);
  }

  oms3::Connector** connector_ = reinterpret_cast<oms3::Connector**>(connector);
  *connector_ = system->getConnector(tail);
  return oms_status_ok;
}

oms_status_enu_t oms3_setCommandLineOption(const char* cmd)
{
  return oms3::Flags::SetCommandLineOption(std::string(cmd));
}

oms_status_enu_t oms3_getSystemType(const char* cref, oms_system_enu_t* type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();

  *type = oms_system_none;

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(tail);
  if (!system) {
    return logError_SystemNotInModel(modelCref, tail);
  }

  *type = system->getType();
  return oms_status_ok;
}

oms_status_enu_t oms3_addConnection(const char *crefA, const char *crefB)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addConnection(tailA,tailB);
}

oms_status_enu_t oms3_deleteConnection(const char *crefA, const char *crefB)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->deleteConnection(tailA,tailB);
}

oms_status_enu_t oms3_setConnectorGeometry(const char *cref, const ssd_connector_geometry_t *geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms3::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setConnectorGeometry(tail, reinterpret_cast<const oms3::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms3_setConnectionGeometry(const char *crefA, const char *crefB, const ssd_connection_geometry_t *geometry)
{
  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms3::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setConnectionGeometry(tailA, tailB, reinterpret_cast<const oms3::ssd::ConnectionGeometry*>(geometry));
}

oms_status_enu_t oms3_setTLMConnectionParameters(const char* crefA, const char* crefB, const oms3_tlm_connection_parameters_t* parameters)
{
#if !defined(NO_TLM)
  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms3::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setTLMConnectionParameters(tailA, tailB, parameters);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_getConnections(const char *cref, oms3_connection_t ***connections)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  (*connections) = reinterpret_cast<oms3_connection_t**>(system->getConnections(tail));

  return oms_status_ok;
}

oms_status_enu_t oms3_addBus(const char *cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addBus(tail);
}

oms_status_enu_t oms3_getBus(const char* cref, oms3_busconnector_t** busConnector)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  oms3::BusConnector** busConnector_ = reinterpret_cast<oms3::BusConnector**>(busConnector);
  *busConnector_ = system->getBusConnector(tail);
  return oms_status_ok;
}

oms_status_enu_t oms3_setBusGeometry(const char* cref, const ssd_connector_geometry_t* geometry)
{
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->setBusGeometry(tail, reinterpret_cast<const oms3::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms3_addTLMBus(const char *cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addTLMBus(tail, domain, dimensions, interpolation);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_getTLMBus(const char* cref, oms3_tlmbusconnector_t** tlmBusConnector)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  oms3::TLMBusConnector** tlmBusConnector_ = reinterpret_cast<oms3::TLMBusConnector**>(tlmBusConnector);
  *tlmBusConnector_ = system->getTLMBusConnector(tail);
  return oms_status_ok;
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_addConnectorToBus(const char *busCref, const char *connectorCref)
{
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addConnectorToBus(busTail, connectorTail);
}

oms_status_enu_t oms3_deleteConnectorFromBus(const char *busCref, const char *connectorCref)
{
  logTrace();
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->deleteConnectorFromBus(busTail, connectorTail);
}

oms_status_enu_t oms3_addConnectorToTLMBus(const char *busCref, const char *connectorCref, const char* type)
{
#if !defined(NO_TLM)
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addConnectorToTLMBus(busTail, connectorTail, type);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_deleteConnectorFromTLMBus(const char *busCref, const char *connectorCref)
{
#if !defined(NO_TLM)
  logTrace();
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->deleteConnectorFromTLMBus(busTail, connectorTail);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_setTLMBusGeometry(const char* cref, const ssd_connector_geometry_t* geometry)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->setTLMBusGeometry(tail, reinterpret_cast<const oms3::ssd::ConnectorGeometry*>(geometry));
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_addTLMConnection(const char *crefA, const char *crefB, double delay, double alpha, double linearimpedance, double angularimpedance)
{
#if !defined(NO_TLM)
  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addTLMConnection(tailA,tailB,delay,alpha,linearimpedance,angularimpedance);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_addExternalModel(const char *cref, const char *path, const char *startscript)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addExternalModel(tail, path, startscript);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_addSubModel(const char* cref, const char* fmuPath)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->addSubModel(tail, fmuPath);
}

oms_status_enu_t oms3_getComponentType(const char* cref, oms_component_enu_t* type)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms3::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  *type = component->getType();
  return oms_status_ok;
}

oms_status_enu_t oms3_getSubModelPath(const char* cref, char** path)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms3::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  *path = (char*)component->getPath().c_str();
  return oms_status_ok;
}

oms_status_enu_t oms3_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms3::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  if (component->getType() == oms_component_fmu)
  {
    *reinterpret_cast<const oms3::FMUInfo**>(fmuInfo) = component->getFMUInfo();
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms3_instantiate(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->instantiate();
}

oms_status_enu_t oms3_initialize(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->initialize();
}

oms_status_enu_t oms3_simulate_asynchronous(const char* cref_, void (*cb)(const char* cref, double time, oms_status_enu_t status))
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->simulate_asynchronous(cb);
}

oms_status_enu_t oms3_cancelSimulation_asynchronous(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->cancelSimulation_asynchronous();
}

oms_status_enu_t oms3_simulate(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->simulate();
}

oms_status_enu_t oms3_stepUntil(const char* cref_, double stopTime)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->stepUntil(stopTime);
}

oms_status_enu_t oms3_reset(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->reset();
}

static int do_simulation(std::string model, std::chrono::duration<double> timeout)
{
  std::mutex m;
  std::condition_variable cv;
  int done=0;
  std::string phase = "Timeout occurred during instantiation";

  std::thread t([&m, &cv, &done, &phase, timeout]()
  {
    std::unique_lock<std::mutex> l(m);
    if (cv.wait_for(l, timeout) == std::cv_status::timeout)
    {
      if (!done && timeout > std::chrono::seconds(0))
      {
        logError(phase);
        exit(1);
      }
    }
  });

  oms3_instantiate(model.c_str());
  phase = "Timeout occurred during initialization";
  oms3_initialize(model.c_str());
  phase = "Timeout occurred during simulation";
  oms3_simulate(model.c_str());

  done=1;

  cv.notify_one();
  t.join();

  return 0;
}

oms_status_enu_t oms3_RunFile(const char* filename_)
{
  oms_status_enu_t status;
  std::string filename(filename_);
  std::string type = "";
  if (filename.length() > 4)
    type = filename.substr(filename.length() - 4);
  else
    return logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");

  if (type == ".fmu")
  {
    std::string modelName("model");
    std::string systemName = modelName + ".root";
    std::string fmuName = systemName + ".fmu";
    oms_fmi_kind_enu_t kind;

    status = oms3_extractFMIKind(filename.c_str(), &kind);
    if (oms_status_ok != status) return logError("oms3_extractFMIKind failed");

    status = oms3_newModel(modelName.c_str());
    if (oms_status_ok != status) return logError("oms3_newModel failed");

    if (kind == oms_fmi_kind_me_and_cs)
      status = oms3_addSystem(systemName.c_str(), oms3::Flags::DefaultModeIsCS() ? oms_system_wc : oms_system_sc);
    else
      status = oms3_addSystem(systemName.c_str(), (kind == oms_fmi_kind_cs) ? oms_system_wc : oms_system_sc);
    if (oms_status_ok != status) return logError("oms3_addSystem failed");

    status = oms3_addSubModel(fmuName.c_str(), filename.c_str());
    if (oms_status_ok != status) return logError("oms3_addSubModel failed");

    if (oms3::Flags::ResultFile() != "<default>")
      oms3_setResultFile(modelName.c_str(), oms3::Flags::ResultFile().c_str(), 1);
    oms3_setStartTime(modelName.c_str(), oms3::Flags::StartTime());
    oms3_setStopTime(modelName.c_str(), oms3::Flags::StopTime());
    oms3_setTolerance(modelName.c_str(), oms3::Flags::Tolerance());
    oms3_setFixedStepSize(systemName.c_str(), (oms3::Flags::StopTime() - oms3::Flags::StartTime()) / (oms3::Flags::Intervals()-1));
    oms3_setSolver(systemName.c_str(), oms3::Flags::Solver().c_str());

    status = do_simulation(modelName, std::chrono::duration<double>(oms3::Flags::Timeout())) ? oms_status_error : oms_status_ok;
    oms3_terminate(modelName.c_str());
    oms3_delete(modelName.c_str());
    return status;
  }
  else if (type == ".ssp")
  {
    char* cref;
    oms3_import(filename.c_str(), &cref);

    if (oms3::Flags::ResultFile() != "<default>")
      oms3_setResultFile(cref, oms3::Flags::ResultFile().c_str(), 1);
    oms3_setStartTime(cref, oms3::Flags::StartTime());
    oms3_setStopTime(cref, oms3::Flags::StopTime());
    oms3_setTolerance(cref, oms3::Flags::Tolerance());

    status = do_simulation(std::string(cref), std::chrono::duration<double>(oms3::Flags::Timeout())) ? oms_status_error : oms_status_ok;
    oms3_terminate(cref);
    oms3_delete(cref);
    return status;
  }
  else
    return logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");

  return oms_status_ok;
}

oms_status_enu_t oms3_terminate(const char* cref_)
{
  oms3::ComRef cref(cref_);

  oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->terminate();
}

oms_status_enu_t oms3_setTLMSocketData(const char *cref, const char *address, int managerPort, int monitorPort)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_tlm)
    return logError_OnlyForTlmSystem;

  oms3::SystemTLM* tlmsystem = reinterpret_cast<oms3::SystemTLM*>(system);
  return tlmsystem->setSocketData(address, managerPort, monitorPort);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_setTLMPositionAndOrientation(const char *cref, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33)
{
#if !defined(NO_TLM)
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_tlm)
    return logError_OnlyForTlmSystem;

  oms3::SystemTLM* tlmsystem = reinterpret_cast<oms3::SystemTLM*>(system);
  std::vector<double> x = {x1,x2,x3};
  std::vector<double> A = {A11,A12,A13,A21,A22,A23,A31,A32,A33};
  return tlmsystem->setPositionAndOrientation(tail, x, A);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);

  if (!model)
    return logError_ModelNotInScope(front);

  oms3::System* system = model->getSystem(tail);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), tail);

  return system->exportDependencyGraphs(initialization, simulation);
}

oms_status_enu_t oms3_getReal(const char* cref, double* value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getReal(tail, *value);
}

oms_status_enu_t oms3_getInteger(const char* cref, int* value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getInteger(tail, *value);
}

oms_status_enu_t oms3_getBoolean(const char* cref, bool* value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getBoolean(tail, *value);
}

oms_status_enu_t oms3_setReal(const char* cref, double value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setReal(tail, value);
}

oms_status_enu_t oms3_setInteger(const char* cref, int value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setInteger(tail, value);
}

oms_status_enu_t oms3_setBoolean(const char* cref, bool value)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms3::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setBoolean(tail, value);
}

oms_status_enu_t oms3_setResultFile(const char* cref_, const char* filename, int bufferSize)
{
  oms3::ComRef cref(cref_);

  if (cref.isValidIdent())
  {
    oms3::Model* model = oms3::Scope::GetInstance().getModel(cref);
    if (!model)
      return logError_ModelNotInScope(cref);

    return model->setResultFile(filename, bufferSize);
  }
  else
    return logError_OnlyForModel;
}

oms_status_enu_t oms3_setSignalFilter(const char* cref, const char* regex)
{
  oms_status_enu_t status;

  status = oms3_removeSignalsFromResults(cref, ".*");
  if (oms_status_ok != status) return status;

  status = oms3_addSignalsToResults(cref, regex);
  if (oms_status_ok != status) return status;

  return oms_status_ok;
}

oms_status_enu_t oms3_addSignalsToResults(const char* cref, const char* regex)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->addSignalsToResults(regex);
}

oms_status_enu_t oms3_removeSignalsFromResults(const char* cref, const char* regex)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->removeSignalsFromResults(regex);
}

oms_status_enu_t oms3_getStartTime(const char* cref, double* startTime)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *startTime = model->getStartTime();
  return oms_status_ok;
}

oms_status_enu_t oms3_setStartTime(const char* cref, double startTime)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->setStartTime(startTime);
}

oms_status_enu_t oms3_getStopTime(const char* cref, double* stopTime)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *stopTime = model->getStopTime();
  return oms_status_ok;
}

oms_status_enu_t oms3_setStopTime(const char* cref, double stopTime)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->setStopTime(stopTime);
}

oms_status_enu_t oms3_setFixedStepSize(const char* cref, double stepSize)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms3::System* system = model->getSystem(tail);
  if (system)
    return system->setFixedStepSize(stepSize);

  return logError_SystemNotInModel(model->getCref(), front);
}

oms_status_enu_t oms3_getModelState(const char* cref, oms_modelState_enu_t* modelState)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *modelState = model->getModelState();
  return oms_status_ok;
}

oms_status_enu_t oms3_getTLMVariableTypes(oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation, char ***types, char ***descriptions)
{
#if !defined(NO_TLM)
  std::vector<std::string> variableTypes = oms3::TLMBusConnector::getVariableTypes(domain, dimensions, interpolation);
  (*types) = new char*[variableTypes.size()+1];
  for(int i=0; i<variableTypes.size(); ++i) {
    (*types)[i] = new char[variableTypes[i].size()+1];
    strcpy((*types)[i], variableTypes[i].c_str());
  }
  (*types)[variableTypes.size()] = NULL;

  std::vector<std::string> variableDescriptions = oms3::TLMBusConnector::getVariableDescriptions(domain, dimensions, interpolation);
  (*descriptions) = new char*[variableDescriptions.size()+1];
  for(int i=0; i<variableDescriptions.size(); ++i) {
    (*descriptions)[i] = new char[variableDescriptions[i].size()+1];
    strcpy((*descriptions)[i], variableDescriptions[i].c_str());
  }
  (*descriptions)[variableDescriptions.size()] = NULL;

  return oms_status_ok;
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms3_extractFMIKind(const char* filename, oms_fmi_kind_enu_t* kind)
{
  if (!kind)
    return logError("Invalid argument \"kind=NULL\"");

  // Usage: miniunz [-e] [-x] [-v] [-l] [-o] [-p password] file.zip [file_to_extr.] [-d extractdir]
  //        -e  Extract without pathname (junk paths)
  //        -x  Extract with pathname
  //        -v  list files
  //        -l  list files
  //        -d  directory to extract into
  //        -o  overwrite files without prompting
  //        -p  extract crypted file using password

  std::string cd = oms3::Scope::GetInstance().getWorkingDirectory();
  int argc = 6;
  char **argv = new char*[argc];
  int i=0;
  argv[i++] = (char*)"miniunz";
  argv[i++] = (char*)"-xo";
  argv[i++] = (char*)filename;
  argv[i++] = (char*)"modelDescription.xml";
  argv[i++] = (char*)"-d";
  argv[i++] = (char*)oms3::Scope::GetInstance().getTempDirectory().c_str();
  int status = ::miniunz(argc, argv);
  delete[] argv;
  oms3::Scope::GetInstance().setWorkingDirectory(cd);

  if (status != 0)
    return logError("failed to extract modelDescription.xml from \"" + std::string(filename) + "\"");

  std::string xml_file = oms3::Scope::GetInstance().getTempDirectory() + "/modelDescription.xml";
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(xml_file.c_str());
  if (!result)
    return logError("loading \"" + xml_file + "\" failed (" + std::string(result.description()) + ")");
  const pugi::xml_node node = doc.document_element(); // ssd:SystemStructureDescription

  bool cs = (std::string(node.child("CoSimulation").attribute("modelIdentifier").as_string()) != "");
  bool me = (std::string(node.child("ModelExchange").attribute("modelIdentifier").as_string()) != "");

  if (me && cs)
    *kind = oms_fmi_kind_me_and_cs;
  else if (me)
    *kind = oms_fmi_kind_me;
  else if (cs)
    *kind = oms_fmi_kind_cs;
  else
  {
    *kind = oms_fmi_kind_unknown;
    return oms_status_error;
  }

  return oms_status_ok;
}

oms_status_enu_t oms3_setSolver(const char* cref, const char* solver)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3_setTolerance(const char* cref, double tolerance)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms3::System* system = model->getSystem(tail);
  if (system)
    return system->setTolerance(tolerance);

  return logError_SystemNotInModel(model->getCref(), front);
}
