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

#include "OMSimulator.h"

#include "Component.h"
#include "ComRef.h"
#include "Element.h"
#include "ExternalModelInfo.h"
#include "Flags.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "MatReader.h"
#include "Model.h"
#include "ResultReader.h"
#include "Scope.h"
#include "SignalDerivative.h"
#include "System.h"
#include "SystemWC.h"
#if !defined(NO_TLM)
  #include "SystemTLM.h"
  #include "TLMBusConnector.h"
  #include "ExternalModel.h"
#endif
#include "Types.h"
#include "Version.h"
#include <miniunz.h>
#include <pugixml.hpp>
#include <string>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#if defined(OMS_STATIC)
extern "C"
{
  #include <OMSimulatorLua.c>
}
#endif

const char* oms_getVersion()
{
  return oms_git_version;
}

void oms_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message))
{
  Log::setLoggingCallback(cb);
}

oms_status_enu_t oms_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
}

oms_status_enu_t oms_setLoggingInterval(const char* cref_, double loggingInterval)
{
  oms::ComRef cref(cref_);

  if (cref.isValidIdent())
  {
    oms::Model* model = oms::Scope::GetInstance().getModel(cref);
    if (!model)
      return logError_ModelNotInScope(cref);

    return model->setLoggingInterval(loggingInterval);
  }
  else
    return logError_OnlyForModel;
}

oms_status_enu_t oms_setLoggingLevel(int logLevel)
{
  return Log::setLoggingLevel(logLevel);
}

void oms_setMaxLogFileSize(const unsigned long size)
{
  Log::setMaxLogFileSize(size);
}

oms_status_enu_t oms_setTempDirectory(const char* newTempDir)
{
  return oms::Scope::GetInstance().setTempDirectory(newTempDir);
}

oms_status_enu_t oms_setWorkingDirectory(const char* newWorkingDir)
{
  return oms::Scope::GetInstance().setWorkingDirectory(newWorkingDir);
}

oms_status_enu_t oms_newModel(const char* cref)
{
  if (!oms::Scope::GetInstance().newModel(oms::ComRef(cref)))
    return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms_rename(const char* cref_, const char* newCref_)
{
  oms::ComRef cref(cref_);
  oms::ComRef newCref(newCref_);

  if (cref.isValidIdent())
    return oms::Scope::GetInstance().renameModel(cref, newCref);
  else
    return logError_OnlyForModel;
}

int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol)
{
  oms::ResultReader* readerA = oms::ResultReader::newReader(filenameA);
  oms::ResultReader* readerB = oms::ResultReader::newReader(filenameB);

  oms::ResultReader::Series* seriesA = readerA->getSeries(var);
  oms::ResultReader::Series* seriesB = readerB->getSeries(var);

  //for (int i=0; i<seriesA->length; ++i)
  //  std::cout << seriesA->time[i] << " - " << seriesA->value[i] << std::endl;
  //for (int i=0; i<seriesB->length; ++i)
  //  std::cout << seriesB->time[i] << " - " << seriesB->value[i] << std::endl;

  bool rc = oms::ResultReader::compareSeries(seriesA, seriesB, relTol, absTol);

  oms::ResultReader::deleteSeries(&seriesA);
  oms::ResultReader::deleteSeries(&seriesB);

  delete readerA;
  delete readerB;

  return rc ? 1 : 0;
}

oms_status_enu_t oms_delete(const char* cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (tail.isEmpty())
    return oms::Scope::GetInstance().deleteModel(front);

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);
  return model->delete_(tail);
}

oms_status_enu_t oms_export(const char* cref, const char* filename)
{
  return oms::Scope::GetInstance().exportModel(oms::ComRef(cref), std::string(filename));
}

oms_status_enu_t oms_importFile(const char* filename, char** cref)
{
  return oms::Scope::GetInstance().importModel(std::string(filename), cref);
}

oms_status_enu_t oms_list(const char* cref_, char** contents)
{
  oms::ComRef tail(cref_);
  oms::ComRef front = tail.pop_front();
  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->list(tail, contents);
}

oms_status_enu_t oms_listUnconnectedConnectors(const char* cref_, char** contents)
{
  oms::ComRef tail(cref_);
  oms::ComRef front = tail.pop_front();
  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (!system)
    return logError_SystemNotInModel(front, tail);

  return system->listUnconnectedConnectors(contents);
}

oms_status_enu_t oms_loadSnapshot(const char* cref_, const char* snapshot)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->loadSnapshot(snapshot);
}

oms_status_enu_t oms_parseModelName(const char* contents, char** cref)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms_addSystem(const char* cref_, oms_system_enu_t type)
{
  oms::ComRef cref(cref_);
  oms::ComRef modelCref = cref.pop_front();
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  return model->addSystem(cref, type);
}

oms_status_enu_t oms_copySystem(const char* source, const char* target)
{
  return logError_NotImplemented;
}

void oms_freeMemory(void* obj)
{
  logTrace();
  if (obj)
    free(obj);
}

oms_status_enu_t oms_getElement(const char* cref, oms_element_t** element)
{
  return oms::Scope::GetInstance().getElement(oms::ComRef(cref), reinterpret_cast<oms::Element**>(element));
}

oms_status_enu_t oms_getElements(const char* cref, oms_element_t*** elements)
{
  return oms::Scope::GetInstance().getElements(oms::ComRef(cref), reinterpret_cast<oms::Element***>(elements));
}

oms_status_enu_t oms_getFixedStepSize(const char* cref, double* stepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  system->getStepSize(NULL, NULL, stepSize);
  return oms_status_ok;
}

oms_status_enu_t oms_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry)
{
  if (!geometry)
  {
    logWarning("[oms_setElementGeometry] NULL pointer");
    return oms_status_warning;
  }

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (system)
  {
    system->setGeometry(*reinterpret_cast<const oms::ssd::ElementGeometry*>(geometry));
    return oms_status_ok;
  }

  oms::Component* component = model->getComponent(tail);
  if (component)
  {
    component->setGeometry(*reinterpret_cast<const oms::ssd::ElementGeometry*>(geometry));
    return oms_status_ok;
  }

  return logError("Model \"" + std::string(model->getCref()) + "\" does not contain system or component \"" + std::string(tail) + "\"");
}

oms_status_enu_t oms_addConnector(const char *cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addConnector(tail, causality, type);
}

oms_status_enu_t oms_getConnector(const char* cref, oms_connector_t** connector)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, tail);
  }

  oms::Connector** connector_ = reinterpret_cast<oms::Connector**>(connector);
  *connector_ = system->getConnector(tail);
  return oms_status_ok;
}

oms_status_enu_t oms_setCommandLineOption(const char* cmd)
{
  return oms::Flags::SetCommandLineOption(std::string(cmd));
}

oms_status_enu_t oms_getSystemType(const char* cref, oms_system_enu_t* type)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();

  *type = oms_system_none;

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms::System* system = model->getSystem(tail);
  if (!system)
    return logError_SystemNotInModel(modelCref, tail);

  *type = system->getType();
  return oms_status_ok;
}

oms_status_enu_t oms_addConnection(const char *crefA, const char *crefB)
{
  oms::ComRef tailA(crefA);
  oms::ComRef modelCref = tailA.pop_front();
  oms::ComRef systemCref = tailA.pop_front();

  oms::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addConnection(tailA,tailB);
}

oms_status_enu_t oms_deleteConnection(const char *crefA, const char *crefB)
{
  oms::ComRef tailA(crefA);
  oms::ComRef modelCref = tailA.pop_front();
  oms::ComRef systemCref = tailA.pop_front();

  oms::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->deleteConnection(tailA,tailB);
}

oms_status_enu_t oms_setConnectorGeometry(const char *cref, const ssd_connector_geometry_t *geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setConnectorGeometry(tail, reinterpret_cast<const oms::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms_setConnectionGeometry(const char *crefA, const char *crefB, const ssd_connection_geometry_t *geometry)
{
  oms::ComRef tailA(crefA);
  oms::ComRef modelCref = tailA.pop_front();
  oms::ComRef systemCref = tailA.pop_front();

  oms::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setConnectionGeometry(tailA, tailB, reinterpret_cast<const oms::ssd::ConnectionGeometry*>(geometry));
}

oms_status_enu_t oms_setTLMConnectionParameters(const char* crefA, const char* crefB, const oms_tlm_connection_parameters_t* parameters)
{
#if !defined(NO_TLM)
  oms::ComRef tailA(crefA);
  oms::ComRef modelCref = tailA.pop_front();
  oms::ComRef systemCref = tailA.pop_front();

  oms::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model)
    return logError_ModelNotInScope(modelCref);

  oms::System* system = model->getSystem(systemCref);
  if (!system)
    return logError_SystemNotInModel(modelCref, systemCref);

  return system->setTLMConnectionParameters(tailA, tailB, parameters);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_getConnections(const char *cref, oms_connection_t ***connections)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  (*connections) = reinterpret_cast<oms_connection_t**>(system->getConnections(tail));

  return oms_status_ok;
}

oms_status_enu_t oms_addBus(const char *cref)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addBus(tail);
}

oms_status_enu_t oms_getBus(const char* cref, oms_busconnector_t** busConnector)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  oms::BusConnector** busConnector_ = reinterpret_cast<oms::BusConnector**>(busConnector);
  *busConnector_ = system->getBusConnector(tail);
  return oms_status_ok;
}

oms_status_enu_t oms_setBusGeometry(const char* cref, const ssd_connector_geometry_t* geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->setBusGeometry(tail, reinterpret_cast<const oms::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms_addTLMBus(const char *cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addTLMBus(tail, domain, dimensions, interpolation);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_getTLMBus(const char* cref, oms_tlmbusconnector_t** tlmBusConnector)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  oms::TLMBusConnector** tlmBusConnector_ = reinterpret_cast<oms::TLMBusConnector**>(tlmBusConnector);
  *tlmBusConnector_ = system->getTLMBusConnector(tail);
  return oms_status_ok;
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_addConnectorToBus(const char *busCref, const char *connectorCref)
{
  oms::ComRef busTail(busCref);
  oms::ComRef modelCref = busTail.pop_front();
  oms::ComRef systemCref = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addConnectorToBus(busTail, connectorTail);
}

oms_status_enu_t oms_deleteConnectorFromBus(const char *busCref, const char *connectorCref)
{
  logTrace();
  oms::ComRef busTail(busCref);
  oms::ComRef modelCref = busTail.pop_front();
  oms::ComRef systemCref = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->deleteConnectorFromBus(busTail, connectorTail);
}

oms_status_enu_t oms_addConnectorToTLMBus(const char *busCref, const char *connectorCref, const char* type)
{
#if !defined(NO_TLM)
  oms::ComRef busTail(busCref);
  oms::ComRef modelCref = busTail.pop_front();
  oms::ComRef systemCref = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addConnectorToTLMBus(busTail, connectorTail, type);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_deleteConnectorFromTLMBus(const char *busCref, const char *connectorCref)
{
#if !defined(NO_TLM)
  logTrace();
  oms::ComRef busTail(busCref);
  oms::ComRef modelCref = busTail.pop_front();
  oms::ComRef systemCref = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameModel(busCref, connectorCref);
  if (systemCref != connectorTail.pop_front())
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->deleteConnectorFromTLMBus(busTail, connectorTail);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_setTLMBusGeometry(const char* cref, const ssd_connector_geometry_t* geometry)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->setTLMBusGeometry(tail, reinterpret_cast<const oms::ssd::ConnectorGeometry*>(geometry));
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_addTLMConnection(const char *crefA, const char *crefB, double delay, double alpha, double linearimpedance, double angularimpedance)
{
#if !defined(NO_TLM)
  oms::ComRef tailA(crefA);
  oms::ComRef modelCref = tailA.pop_front();
  oms::ComRef systemCref = tailA.pop_front();

  oms::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addTLMConnection(tailA,tailB,delay,alpha,linearimpedance,angularimpedance);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_addExternalModel(const char *cref, const char *path, const char *startscript)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef modelCref = tail.pop_front();
  oms::ComRef systemCref = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addExternalModel(tail, path, startscript);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_addSubModel(const char* cref, const char* fmuPath)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->addSubModel(tail, fmuPath);
}

oms_status_enu_t oms_getComponentType(const char* cref, oms_component_enu_t* type)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  *type = component->getType();
  return oms_status_ok;
}

oms_status_enu_t oms_getSubModelPath(const char* cref, char** path)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  *path = (char*)component->getPath().c_str();
  return oms_status_ok;
}

oms_status_enu_t oms_getExternalModelInfo(const char* cref, const oms_external_tlm_model_info_t** externalModelInfo)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  if (component->getType() == oms_component_external)
  {
    *reinterpret_cast<const oms::ExternalModelInfo**>(externalModelInfo) = reinterpret_cast<oms::ExternalModel*>(component)->getExternalModelInfo();
    return oms_status_ok;
  }

  return oms_status_error;
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  oms::Component* component = system->getComponent(tail);
  if (!component)
    return logError_ComponentNotInSystem(system, tail);

  if (component->getType() == oms_component_fmu)
  {
    *reinterpret_cast<const oms::FMUInfo**>(fmuInfo) = component->getFMUInfo();
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms_instantiate(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->instantiate();
}

oms_status_enu_t oms_initialize(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->initialize();
}

oms_status_enu_t oms_simulate_asynchronous(const char* cref_, void (*cb)(const char* cref, double time, oms_status_enu_t status))
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->simulate_asynchronous(cb);
}

oms_status_enu_t oms_cancelSimulation_asynchronous(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->cancelSimulation_asynchronous();
}

oms_status_enu_t oms_simulate(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->simulate();
}

oms_status_enu_t oms_stepUntil(const char* cref_, double stopTime)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->stepUntil(stopTime);
}

oms_status_enu_t oms_reset(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
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

  oms_instantiate(model.c_str());
  phase = "Timeout occurred during initialization";
  oms_initialize(model.c_str());
  phase = "Timeout occurred during simulation";
  oms_simulate(model.c_str());

  done=1;

  cv.notify_one();
  t.join();

  return 0;
}

oms_status_enu_t oms_RunFile(const char* filename_)
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

    status = oms_extractFMIKind(filename.c_str(), &kind);
    if (oms_status_ok != status) return logError("oms_extractFMIKind failed");

    status = oms_newModel(modelName.c_str());
    if (oms_status_ok != status) return logError("oms_newModel failed");

    oms::Model* model = oms::Scope::GetInstance().getModel(oms::ComRef(modelName));
    if (!model)
      return logError_ModelNotInScope(oms::ComRef(modelName));
    model->setIsolatedFMUModel();

    if (kind == oms_fmi_kind_me_and_cs)
      status = oms_addSystem(systemName.c_str(), oms::Flags::DefaultModeIsCS() ? oms_system_wc : oms_system_sc);
    else
      status = oms_addSystem(systemName.c_str(), (kind == oms_fmi_kind_cs) ? oms_system_wc : oms_system_sc);
    if (oms_status_ok != status) return logError("oms_addSystem failed");

    status = oms_addSubModel(fmuName.c_str(), filename.c_str());
    if (oms_status_ok != status) return logError("oms_addSubModel failed");

    if (oms::Flags::ResultFile() != "<default>")
      oms_setResultFile(modelName.c_str(), oms::Flags::ResultFile().c_str(), 1);
    oms_setStartTime(modelName.c_str(), oms::Flags::StartTime());
    oms_setStopTime(modelName.c_str(), oms::Flags::StopTime());
    oms_setTolerance(modelName.c_str(), oms::Flags::Tolerance(), oms::Flags::Tolerance());
    oms_setFixedStepSize(systemName.c_str(), (oms::Flags::StopTime() - oms::Flags::StartTime()) / (oms::Flags::Intervals()-1));
    if (kind == oms_fmi_kind_me_and_cs)
      oms_setSolver(systemName.c_str(), oms::Flags::DefaultModeIsCS() ? oms::Flags::MasterAlgorithm() : oms::Flags::Solver());
    else
      oms_setSolver(systemName.c_str(), (kind == oms_fmi_kind_cs) ? oms::Flags::MasterAlgorithm() : oms::Flags::Solver());

    status = do_simulation(modelName, std::chrono::duration<double>(oms::Flags::Timeout())) ? oms_status_error : oms_status_ok;
    oms_terminate(modelName.c_str());
    oms_delete(modelName.c_str());
    return status;
  }
  else if (type == ".ssp")
  {
    char* cref;
    oms_importFile(filename.c_str(), &cref);

    if (oms::Flags::ResultFile() != "<default>")
      oms_setResultFile(cref, oms::Flags::ResultFile().c_str(), 1);
    oms_setStartTime(cref, oms::Flags::StartTime());
    oms_setStopTime(cref, oms::Flags::StopTime());
    oms_setTolerance(cref, oms::Flags::Tolerance(), oms::Flags::Tolerance());

    status = do_simulation(std::string(cref), std::chrono::duration<double>(oms::Flags::Timeout())) ? oms_status_error : oms_status_ok;
    oms_terminate(cref);
    oms_delete(cref);
    return status;
  }
  else if (type == ".lua")
  {
#if defined(OMS_STATIC)
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_OMSimulatorLua(L);
    if (luaL_loadfile(L, filename.c_str()))
      return logError(lua_tostring(L, -1));

    if (lua_pcall(L, 0, 0, 0))
      return logError(lua_tostring(L, -1));

    lua_close(L);
#else
    return logError("Lua is only supported in the static version of OMSimulatorLib");
#endif
  }
  else
    return logError("Not able to process file '" + filename + "'\nUse OMSimulator --help for more information.");

  return oms_status_ok;
}

oms_status_enu_t oms_terminate(const char* cref_)
{
  oms::ComRef cref(cref_);

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  return model->terminate();
}

oms_status_enu_t oms_setTLMSocketData(const char *cref, const char *address, int managerPort, int monitorPort)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  oms::SystemTLM* tlmsystem = reinterpret_cast<oms::SystemTLM*>(system);
  return tlmsystem->setSocketData(address, managerPort, monitorPort);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_setTLMPositionAndOrientation(const char *cref, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  oms::SystemTLM* tlmsystem = reinterpret_cast<oms::SystemTLM*>(system);
  std::vector<double> x = {x1,x2,x3};
  std::vector<double> A = {A11,A12,A13,A21,A22,A23,A31,A32,A33};
  return tlmsystem->setPositionAndOrientation(tail, x, A);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);

  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), tail);

  return system->exportDependencyGraphs(initialization, simulation);
}

oms_status_enu_t oms_getReal(const char* cref, double* value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getReal(tail, *value);
}

oms_status_enu_t oms_getSolver(const char* cref, oms_solver_enu_t* solver)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  *solver = system->getSolver();
  return oms_status_ok;
}

oms_status_enu_t oms_getInteger(const char* cref, int* value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getInteger(tail, *value);
}

oms_status_enu_t oms_getBoolean(const char* cref, bool* value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->getBoolean(tail, *value);
}

oms_status_enu_t oms_setReal(const char* cref, double value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setReal(tail, value);
}

oms_status_enu_t oms_setRealInputDerivative(const char* cref, double value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_wc)
    return logError_OnlyForSystemWC;

  oms::SystemWC* systemWC = reinterpret_cast<oms::SystemWC*>(system);
  return systemWC->setRealInputDerivative(tail, oms::SignalDerivative(value));
}

oms_status_enu_t oms_setInteger(const char* cref, int value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setInteger(tail, value);
}

oms_status_enu_t oms_setBoolean(const char* cref, bool value)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  return system->setBoolean(tail, value);
}

oms_status_enu_t oms_setResultFile(const char* cref_, const char* filename, int bufferSize)
{
  oms::ComRef cref(cref_);

  if (cref.isValidIdent())
  {
    oms::Model* model = oms::Scope::GetInstance().getModel(cref);
    if (!model)
      return logError_ModelNotInScope(cref);

    return model->setResultFile(filename, bufferSize);
  }
  else
    return logError_OnlyForModel;
}

oms_status_enu_t oms_setSignalFilter(const char* cref, const char* regex)
{
  oms_status_enu_t status;

  status = oms_removeSignalsFromResults(cref, ".*");
  if (oms_status_ok != status) return status;

  status = oms_addSignalsToResults(cref, regex);
  if (oms_status_ok != status) return status;

  return oms_status_ok;
}

oms_status_enu_t oms_addSignalsToResults(const char* cref, const char* regex)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->addSignalsToResults(regex);
}

oms_status_enu_t oms_removeSignalsFromResults(const char* cref, const char* regex)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->removeSignalsFromResults(regex);
}

oms_status_enu_t oms_getStartTime(const char* cref, double* startTime)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *startTime = model->getStartTime();
  return oms_status_ok;
}

oms_status_enu_t oms_setStartTime(const char* cref, double startTime)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->setStartTime(startTime);
}

oms_status_enu_t oms_getStopTime(const char* cref, double* stopTime)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *stopTime = model->getStopTime();
  return oms_status_ok;
}

oms_status_enu_t oms_setStopTime(const char* cref, double stopTime)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  return model->setStopTime(stopTime);
}

oms_status_enu_t oms_setFixedStepSize(const char* cref, double stepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (system)
    return system->setFixedStepSize(stepSize);

  return logError_SystemNotInModel(model->getCref(), front);
}

oms_status_enu_t oms_setVariableStepSize(const char* cref, double initialStepSize, double minimumStepSize, double maximumStepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (system)
    return system->setVariableStepSize(initialStepSize, minimumStepSize, maximumStepSize);

  return logError_SystemNotInModel(model->getCref(), front);
}

oms_status_enu_t oms_getModelState(const char* cref, oms_modelState_enu_t* modelState)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  *modelState = model->getModelState();
  return oms_status_ok;
}

oms_status_enu_t oms_getTLMVariableTypes(oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation, char ***types, char ***descriptions)
{
#if !defined(NO_TLM)
  std::vector<std::string> variableTypes = oms::TLMBusConnector::getVariableTypes(domain, dimensions, interpolation);
  (*types) = new char*[variableTypes.size()+1];
  for(int i=0; i<variableTypes.size(); ++i) {
    (*types)[i] = new char[variableTypes[i].size()+1];
    strcpy((*types)[i], variableTypes[i].c_str());
  }
  (*types)[variableTypes.size()] = NULL;

  std::vector<std::string> variableDescriptions = oms::TLMBusConnector::getVariableDescriptions(domain, dimensions, interpolation);
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

oms_status_enu_t oms_getTolerance(const char* cref, double* absoluteTolerance, double* relativeTolerance)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  system->getTolerance(absoluteTolerance, relativeTolerance);
  return oms_status_ok;
}

oms_status_enu_t oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  system->getStepSize(initialStepSize, minimumStepSize, maximumStepSize);
  return oms_status_ok;
}

oms_status_enu_t oms_extractFMIKind(const char* filename, oms_fmi_kind_enu_t* kind)
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

  std::string cd = oms::Scope::GetInstance().getWorkingDirectory();
  int argc = 6;
  char **argv = new char*[argc];
  int i=0;
  argv[i++] = (char*)"miniunz";
  argv[i++] = (char*)"-xo";
  argv[i++] = (char*)filename;
  argv[i++] = (char*)"modelDescription.xml";
  argv[i++] = (char*)"-d";
  argv[i++] = (char*)oms::Scope::GetInstance().getTempDirectory().c_str();
  int status = ::miniunz(argc, argv);
  delete[] argv;
  oms::Scope::GetInstance().setWorkingDirectory(cd);

  if (status != 0)
    return logError("failed to extract modelDescription.xml from \"" + std::string(filename) + "\"");

  std::string xml_file = oms::Scope::GetInstance().getTempDirectory() + "/modelDescription.xml";
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

oms_status_enu_t oms_setSolver(const char* cref, oms_solver_enu_t solver)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (system)
    return system->setSolver(solver);

  return logError_SystemNotInModel(model->getCref(), front);
}

oms_status_enu_t oms_setTolerance(const char* cref, double absoluteTolerance, double relativeTolerance)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  oms::System* system = model->getSystem(tail);
  if (system)
    return system->setTolerance(absoluteTolerance, relativeTolerance);

  return logError_SystemNotInModel(model->getCref(), front);
}

oms_status_enu_t oms_addEventIndicator(const char* cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_wc)
    return logError_OnlyForSystemWC;

  oms::SystemWC* systemWC = reinterpret_cast<oms::SystemWC*>(system);
  return systemWC->addEventIndicator(tail);
}

oms_status_enu_t oms_addTimeIndicator(const char* cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_wc)
    return logError_OnlyForSystemWC;

  oms::SystemWC* systemWC = reinterpret_cast<oms::SystemWC*>(system);
  return systemWC->addTimeIndicator(tail);
}

oms_status_enu_t oms_addStaticValueIndicator(const char* cref, double lower, double upper, double stepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_wc)
    return logError_OnlyForSystemWC;

  oms::SystemWC* systemWC = reinterpret_cast<oms::SystemWC*>(system);
  return systemWC->addStaticValueIndicator(tail, lower, upper, stepSize);
}

oms_status_enu_t oms_addDynamicValueIndicator(const char* cref, const char* lower, const char* upper, double stepSize)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = oms::Scope::GetInstance().getModel(front);
  if (!model)
    return logError_ModelNotInScope(front);

  front = tail.pop_front();
  oms::System* system = model->getSystem(front);
  if (!system)
    return logError_SystemNotInModel(model->getCref(), front);

  if (system->getType() != oms_system_wc)
    return logError_OnlyForSystemWC;

  oms::SystemWC* systemWC = reinterpret_cast<oms::SystemWC*>(system);
  return systemWC->addDynamicValueIndicator(tail, oms::ComRef(lower), oms::ComRef(upper), stepSize);
}
