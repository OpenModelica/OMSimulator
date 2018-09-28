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
#include "Types.h"
#include "Version.h"

#include <string>
#include <boost/filesystem.hpp>

/* ************************************ */
/* oms3                                 */
/*                                      */
/* Experimental API                     */
/* ************************************ */

const char* oms3_getVersion()
{
  return oms_git_version;
}

oms_status_enu_t oms3_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
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

oms_status_enu_t oms3_delete(const char* cref_)
{
  oms3::ComRef cref(cref_);

  if (cref.isValidIdent())
    return oms3::Scope::GetInstance().deleteModel(cref);
  else
    return logError_OnlyForModel;
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
  oms3::ComRef modelCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(tail);
  if (!system) {
    return logError_SystemNotInModel(modelCref, tail);
  }

  system->setGeometry(*reinterpret_cast<const oms3::ssd::ElementGeometry*>(geometry));
  return oms_status_ok;
}

oms_status_enu_t oms3_addConnector(const char *cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  logTrace();

  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if(!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if(!system) {
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
  if (std::string(cmd) == "--suppressPath=true")
    oms3::Flags::SuppressPath(true);
  else if (std::string(cmd) == "--suppressPath=false")
    oms3::Flags::SuppressPath(false);
  else
    return logError("Unknown flag or option: \"" + std::string(cmd) + "\"");

  return oms_status_ok;
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
  logTrace();

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

oms_status_enu_t oms3_setConnectorGeometry(const char *cref, const ssd_connector_geometry_t *geometry)
{
  logTrace();

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

  return system->setConnectorGeometry(tail, reinterpret_cast<const oms2::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms3_setConnectionGeometry(const char *crefA, const char *crefB, const ssd_connection_geometry_t *geometry)
{
  logTrace();

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

  return system->setConnectionGeometry(tailA,tailB, reinterpret_cast<const oms2::ssd::ConnectionGeometry*>(geometry));
}

oms_status_enu_t oms3_getConnections(const char *cref, oms3_connection_t ***connections)
{
  logTrace();

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
  logTrace();
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

  return system->setBusGeometry(tail, reinterpret_cast<const oms2::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms3_addTLMBus(const char *cref, const char *domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
  logTrace();
  oms3::ComRef tail(cref);
  oms3::ComRef modelCref = tail.pop_front();
  oms3::ComRef systemCref = tail.pop_front();
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if(!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if(!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addTLMBus(tail, domain, dimensions, interpolation);
}

oms_status_enu_t oms3_getTLMBus(const char* cref, oms3_tlmbusconnector_t** tlmBusConnector)
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

  oms3::TLMBusConnector** tlmBusConnector_ = reinterpret_cast<oms3::TLMBusConnector**>(tlmBusConnector);
  *tlmBusConnector_ = system->getTLMBusConnector(tail);
  return oms_status_ok;
}

oms_status_enu_t oms3_addConnectorToBus(const char *busCref, const char *connectorCref)
{
  logTrace();
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError("Bus and connector must belong to same model");
  if (systemCref != connectorTail.pop_front())
    return logError("Bus and connector must belong to same system");
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

oms_status_enu_t oms3_addConnectorToTLMBus(const char *busCref, const char *connectorCref, const char* type)
{
  logTrace();
  oms3::ComRef busTail(busCref);
  oms3::ComRef modelCref = busTail.pop_front();
  oms3::ComRef systemCref = busTail.pop_front();
  oms3::ComRef connectorTail(connectorCref);
  if (modelCref != connectorTail.pop_front())
    return logError("Bus and connector must belong to same model");
  if (systemCref != connectorTail.pop_front())
    return logError("Bus and connector must belong to same system");
  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if (!model) {
    return logError_ModelNotInScope(modelCref);
  }
  oms3::System* system = model->getSystem(systemCref);
  if (!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }
  return system->addConnectorToTLMBus(busTail, connectorTail, type);
}

oms_status_enu_t oms3_setTLMBusGeometry(const char* cref, const ssd_connector_geometry_t* geometry)
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

  return system->setTLMBusGeometry(tail, reinterpret_cast<const oms2::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms3_addTLMConnection(const char *crefA, const char *crefB, double delay, double alpha, double impedance, double impedancerot)
{
  logTrace();

  oms3::ComRef tailA(crefA);
  oms3::ComRef modelCref = tailA.pop_front();
  oms3::ComRef systemCref = tailA.pop_front();

  oms3::ComRef tailB(crefB);
  tailB.pop_front();
  tailB.pop_front();

  oms3::Model* model = oms3::Scope::GetInstance().getModel(modelCref);
  if(!model) {
    return logError_ModelNotInScope(modelCref);
  }

  oms3::System* system = model->getSystem(systemCref);
  if(!system) {
    return logError_SystemNotInModel(modelCref, systemCref);
  }

  return system->addTLMConnection(tailA,tailB,delay,alpha,impedance,impedancerot);
}

/* ************************************ */
/* OMSimulator 2.0                      */
/*                                      */
/* TODO: replace prefix oms2 with oms   */
/* ************************************ */

oms_status_enu_t oms2_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
}

void oms2_setMaxLogFileSize(const unsigned long size)
{
  Log::setMaxLogFileSize(size);
}

const char* oms2_getVersion()
{
  return oms_git_version;
}

int oms2_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol)
{
  ResultReader* readerA = ResultReader::newReader(filenameA);
  ResultReader* readerB = ResultReader::newReader(filenameB);

  ResultReader::Series* seriesA = readerA->getSeries(var);
  ResultReader::Series* seriesB = readerB->getSeries(var);

  //for (int i=0; i<seriesA->length; ++i)
  //  std::cout << seriesA->time[i] << " - " << seriesA->value[i] << std::endl;
  //for (int i=0; i<seriesB->length; ++i)
  //  std::cout << seriesB->time[i] << " - " << seriesB->value[i] << std::endl;

  bool rc = ResultReader::compareSeries(seriesA, seriesB, relTol, absTol);

  ResultReader::deleteSeries(&seriesA);
  ResultReader::deleteSeries(&seriesB);

  delete readerA;
  delete readerB;

  return rc ? 1 : 0;
}

oms_status_enu_t oms2_newFMIModel(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().newFMIModel(oms2::ComRef(ident));
}

oms_status_enu_t oms2_newTLMModel(const char* ident)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().newTLMModel(oms2::ComRef(ident));
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_unloadModel(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().unloadModel(oms2::ComRef(ident));
}

oms_status_enu_t oms2_addFMU(const char* modelIdent, const char* fmuPath, const char* fmuIdent)
{
  logTrace();
  return oms2::Scope::GetInstance().addFMU(oms2::ComRef(modelIdent), std::string(fmuPath), oms2::ComRef(fmuIdent));
}

oms_status_enu_t oms2_addTable(const char* modelIdent, const char* tablePath, const char* tableIdent)
{
  logTrace();
  return oms2::Scope::GetInstance().addTable(oms2::ComRef(modelIdent), std::string(tablePath), oms2::ComRef(tableIdent));
}

oms_status_enu_t oms2_deleteSubModel(const char* modelIdent, const char* subModelIdent)
{
  logTrace();
  return oms2::Scope::GetInstance().deleteSubModel(oms2::ComRef(modelIdent), oms2::ComRef(subModelIdent));
}

oms_status_enu_t oms2_rename(const char* identOld, const char* identNew)
{
  logTrace();
  return oms2::Scope::GetInstance().rename(oms2::ComRef(identOld), oms2::ComRef(identNew));
}

oms_status_enu_t oms2_loadModel(const char* filename, char** ident)
{
  logTrace();
  oms2::Model* model = oms2::Scope::GetInstance().loadModel(filename);

  if (!model) {
    return oms_status_error;
  }

  oms_element_t* element = reinterpret_cast<oms_element_t*>(model->getElement());
  *ident = element->name;
  return oms_status_ok;
}

oms_status_enu_t oms2_parseString(const char* contents, char** ident)
{
  logTrace();
  return oms2::Scope::GetInstance().parseString(contents, ident);
}

oms_status_enu_t oms2_loadString(const char* contents, char** ident)
{
  logTrace();
  oms2::Model* model = oms2::Scope::GetInstance().loadModel(contents);

  if (!model) {
    return oms_status_error;
  }

  oms_element_t* element = reinterpret_cast<oms_element_t*>(model->getElement());
  *ident = element->name;
  return oms_status_ok;
}

oms_status_enu_t oms2_saveModel(const char* ident, const char* filename)
{
  logTrace();
  return oms2::Scope::GetInstance().saveModel(oms2::ComRef(ident), filename);
}

oms_status_enu_t oms2_listModel(const char* ident, char** contents)
{
  logTrace();
  return oms2::Scope::GetInstance().listModel(oms2::ComRef(ident), contents);
}

oms_status_enu_t oms2_getElement(const char* cref, oms_element_t** element)
{
  logTrace();
  return oms2::Scope::GetInstance().getElement(oms2::ComRef(cref), reinterpret_cast<oms2::Element**>(element));
}

oms_status_enu_t oms2_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry)
{
  logTrace();
  return oms2::Scope::GetInstance().setElementGeometry(oms2::ComRef(cref), reinterpret_cast<const oms2::ssd::ElementGeometry*>(geometry));
}

oms_status_enu_t oms2_getElements(const char* cref, oms_element_t*** elements)
{
  logTrace();
  return oms2::Scope::GetInstance().getElements(oms2::ComRef(cref), reinterpret_cast<oms2::Element***>(elements));
}

oms_status_enu_t oms2_getSubModelPath(const char* cref, char** path)
{
  logTrace();
  return oms2::Scope::GetInstance().getSubModelPath(oms2::ComRef(cref), path);
}

oms_status_enu_t oms2_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo)
{
  logTrace();
  return oms2::Scope::GetInstance().getFMUInfo(oms2::ComRef(cref), reinterpret_cast<const oms2::FMUInfo**>(fmuInfo));
}

oms_status_enu_t oms2_setConnectorGeometry(const char* connector, const ssd_connector_geometry_t* geometry)
{
  logTrace();
  return oms2::Scope::GetInstance().setConnectorGeometry(oms2::SignalRef(connector), reinterpret_cast<const oms2::ssd::ConnectorGeometry*>(geometry));
}

oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections)
{
  logTrace();
  return oms2::Scope::GetInstance().getConnections(oms2::ComRef(cref), reinterpret_cast<oms2::Connection***>(connections));
}

oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();

  if(oms2::SignalRef::isValid(conA) && oms2::SignalRef::isValid(conB))
  {
    // case 1: FMU:signal1 -> FMU:signal2
    return oms2::Scope::GetInstance().addConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
  }
  else if(oms2::ComRef::isValidIdent(conA) && oms2::ComRef::isValidIdent(conB))
  {
    // case 2: FMU -> solver
    return oms2::Scope::GetInstance().connectSolver(oms2::ComRef(cref), oms2::ComRef(conA), oms2::ComRef(conB));
  }
  else
    return logWarning("[oms2_addConnection] invalid arguments");
}

oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();

  if(oms2::SignalRef::isValid(conA) && oms2::SignalRef::isValid(conB))
  {
    // case 1: FMU:signal1 -> FMU:signal2
    return oms2::Scope::GetInstance().deleteConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
  }
  else if(oms2::ComRef::isValidIdent(conA) && oms2::ComRef::isValidIdent(conB))
  {
    // case 2: FMU -> solver
    return oms2::Scope::GetInstance().unconnectSolver(oms2::ComRef(cref), oms2::ComRef(conA), oms2::ComRef(conB));
  }
  else
    return logWarning("[oms2_deleteConnection] invalid arguments");
}

oms_status_enu_t oms2_updateConnection(const char* cref, const char* conA, const char* conB, const oms_connection_t* connection)
{
  logTrace();
  return oms2::Scope::GetInstance().updateConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB), reinterpret_cast<const oms2::Connection*>(connection));
}

oms_status_enu_t oms2_initialize(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().initialize(oms2::ComRef(ident));
}

oms_status_enu_t oms2_simulate(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().simulate(oms2::ComRef(ident));
}

oms_status_enu_t oms2_doSteps(const char* ident, const int numberOfSteps)
{
  logTrace();
  return oms2::Scope::GetInstance().doSteps(oms2::ComRef(ident), numberOfSteps);
}

oms_status_enu_t oms2_stepUntil(const char* ident, const double timeValue)
{
  logTrace();
  return oms2::Scope::GetInstance().stepUntil(oms2::ComRef(ident), timeValue);
}

oms_status_enu_t oms2_reset(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().reset(oms2::ComRef(ident));
}

oms_status_enu_t oms2_simulate_asynchronous(const char* ident, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  logTrace();
  return oms2::Scope::GetInstance().simulate_asynchronous(oms2::ComRef(ident), cb);
}

void oms2_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message))
{
  logTrace();
  Log::setLoggingCallback(cb);
}

void oms2_setLoggingLevel(int logLevel)
{
  Log::setLoggingLevel(logLevel);
}

oms_status_enu_t oms2_getReal(const char* signal, double* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getReal(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setReal(const char* signal, double value)
{
  logTrace();
  return oms2::Scope::GetInstance().setReal(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_getRealParameter(const char* signal, double* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getRealParameter(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setRealParameter(const char* signal, double value)
{
  logTrace();
  return oms2::Scope::GetInstance().setRealParameter(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_getInteger(const char* signal, int* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getInteger(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setInteger(const char* signal, int value)
{
  logTrace();
  return oms2::Scope::GetInstance().setInteger(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_getIntegerParameter(const char* signal, int* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getIntegerParameter(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setIntegerParameter(const char* signal, int value)
{
  logTrace();
  return oms2::Scope::GetInstance().setIntegerParameter(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_getBooleanParameter(const char* signal, bool* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getBooleanParameter(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setBooleanParameter(const char* signal, bool value)
{
  logTrace();
  return oms2::Scope::GetInstance().setBooleanParameter(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_getBoolean(const char* signal, bool* value)
{
  logTrace();
  return oms2::Scope::GetInstance().getBoolean(oms2::SignalRef(signal), *value);
}

oms_status_enu_t oms2_setBoolean(const char* signal, bool value)
{
  logTrace();
  return oms2::Scope::GetInstance().setBoolean(oms2::SignalRef(signal), value);
}

oms_status_enu_t oms2_setTempDirectory(const char* path)
{
  logTrace();
  return oms2::Scope::GetInstance().setTempDirectory(path);
}

oms_status_enu_t oms2_setWorkingDirectory(const char* path)
{
  logTrace();
  return oms2::Scope::GetInstance().setWorkingDirectory(path);
}

oms_status_enu_t oms2_getComponentType(const char* ident, oms_element_type_enu_t* type)
{
  logTrace();
  if (!type) {
    return logError("oms2_getComponentType: type is NULL pointer");
  }

  *type = oms_component_none;
  return logError("oms2_getComponentType: not implemented yet");
}

oms_status_enu_t oms2_describe(const char* cref)
{
  logTrace();
  return oms2::Scope::GetInstance().describeModel(oms2::ComRef(cref));
}

oms_status_enu_t oms2_addExternalModel(const char *cref, const char *name, const char *modelfile, const char *startscript)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().addExternalModel(oms2::ComRef(cref), oms2::ComRef(name), modelfile, startscript);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_addTLMInterface(const char *cref, const char *subref, const char *name, int dimensions, oms_causality_enu_t causality, oms_tlm_interpolation_t interpolation, const char *domain, const char **sigrefs, int nsigrefs)
{
  logTrace();
#if !defined(NO_TLM)
  std::vector<oms2::SignalRef>vSigrefs;
  for(int i=0; i<nsigrefs; ++i) {
    vSigrefs.push_back(oms2::SignalRef(sigrefs[i]));
  }
  return oms2::Scope::GetInstance().addTLMInterface(oms2::ComRef(cref), oms2::ComRef(subref), oms2::ComRef(name), dimensions, causality, domain, interpolation, vSigrefs);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_setTLMPositionAndOrientation(const char *cref, const char *ifc, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33)
{
  logTrace();
#if !defined(NO_TLM)
  std::vector<double> x = {x1,x2,x3};
  std::vector<double> A = {A11,A12,A13,A21,A22,A23,A31,A32,A33};
  std::string ifcstr = ifc;
  if(ifcstr.find(':') == std::string::npos) {
    ifcstr.append(":");
  }
  return oms2::Scope::GetInstance().setTLMPositionAndOrientation(oms2::ComRef(cref), oms2::SignalRef(ifcstr), x, A);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_addTLMConnection(const char *cref, const char *from, const char *to, double delay, double alpha, double Zf, double Zfr)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().addTLMConnection(oms2::ComRef(cref), oms2::SignalRef(from), oms2::SignalRef(to), delay, alpha, Zf, Zfr);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_addFMISubModel(const char *cref, const char *subref)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().addFMISubModel(oms2::ComRef(cref), oms2::ComRef(subref));
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_setTLMSocketData(const char* cref, const char* address,
                                       int managerPort, int monitorPort)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().setTLMSocketData(oms2::ComRef(cref), address,
                                                     managerPort, monitorPort);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_setTLMInitialValues(const char *cref, const char *ifc, const double values[], int nvalues)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().setTLMInitialValues(oms2::ComRef(cref), oms2::SignalRef(ifc), std::vector<double>(values, values+nvalues));
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_getStartTime(const char* cref, double* startTime)
{
  logTrace();
  return oms2::Scope::GetInstance().getStartTime(oms2::ComRef(cref), startTime);
}

oms_status_enu_t oms2_setStartTime(const char* cref, double startTime)
{
  logTrace();
  return oms2::Scope::GetInstance().setStartTime(oms2::ComRef(cref), startTime);
}

oms_status_enu_t oms2_getStopTime(const char* cref, double* stopTime)
{
  logTrace();
  return oms2::Scope::GetInstance().getStopTime(oms2::ComRef(cref), stopTime);
}

oms_status_enu_t oms2_setStopTime(const char* cref, double stopTime)
{
  logTrace();
  return oms2::Scope::GetInstance().setStopTime(oms2::ComRef(cref), stopTime);
}

oms_status_enu_t oms2_setCommunicationInterval(const char* cref, double communicationInterval)
{
  logTrace();
  return oms2::Scope::GetInstance().setCommunicationInterval(oms2::ComRef(cref), communicationInterval);
}

oms_status_enu_t oms2_setLoggingInterval(const char* cref, double loggingInterval)
{
  logTrace();
  return oms2::Scope::GetInstance().setLoggingInterval(oms2::ComRef(cref), loggingInterval);
}

oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename, int bufferSize)
{
  logTrace();
  return oms2::Scope::GetInstance().setResultFile(oms2::ComRef(cref), std::string(filename), bufferSize);
}

oms_status_enu_t oms2_setMasterAlgorithm(const char* ident, const char* masterAlgorithm)
{
  logTrace();
  return oms2::Scope::GetInstance().setMasterAlgorithm(oms2::ComRef(ident), std::string(masterAlgorithm));
}

oms_status_enu_t experimental_setActivationRatio(const char* cref, int k)
{
  logTrace();
  return oms2::Scope::GetInstance().setActivationRatio(oms2::ComRef(cref), k);
}

oms_status_enu_t experimental_simulate_realtime(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().simulate_realtime(oms2::ComRef(ident));
}

oms_status_enu_t oms2_exportCompositeStructure(const char* cref, const char* filename)
{
  logTrace();
  return oms2::Scope::GetInstance().exportCompositeStructure(oms2::ComRef(cref), std::string(filename));
}

oms_status_enu_t oms2_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation)
{
  logTrace();
  return oms2::Scope::GetInstance().exportDependencyGraphs(oms2::ComRef(cref), std::string(initialization), std::string(simulation));
}

oms_status_enu_t oms2_getCurrentTime(const char* model, double* time)
{
  logTrace();
  return oms2::Scope::GetInstance().getCurrentTime(oms2::ComRef(model), time);

}

oms_status_enu_t oms2_setTLMLoggingLevel(const char *cref, const int level)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().setTLMLoggingLevel(oms2::ComRef(cref), level);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_setLoggingSamples(const char *cref, const int samples)
{
  logTrace();
#if !defined(NO_TLM)
  return oms2::Scope::GetInstance().setLoggingSamples(oms2::ComRef(cref), samples);
#else
  return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms2_addSignalsToResults(const char* cref, const char* regex)
{
  logTrace();
  return oms2::Scope::GetInstance().addSignalsToResults(oms2::ComRef(cref), std::string(regex));
}

oms_status_enu_t oms2_removeSignalsFromResults(const char* cref, const char* regex)
{
  logTrace();
  return oms2::Scope::GetInstance().removeSignalsFromResults(oms2::ComRef(cref), std::string(regex));
}

oms_status_enu_t oms2_setFlags(const char* cref, const char* flags)
{
  logTrace();
  return oms2::Scope::GetInstance().setFlags(oms2::ComRef(cref), std::string(flags));
}

oms_status_enu_t oms2_addSolver(const char* model, const char* name, const char* solver)
{
  logTrace();
  return oms2::Scope::GetInstance().addSolver(oms2::ComRef(model), oms2::ComRef(name), std::string(solver));
}

oms_status_enu_t oms2_freeMemory(void* obj)
{
  logTrace();
  if (obj)
  {
    free(obj);
  }
  return oms_status_ok;
}

int oms2_exists(const char* cref)
{
  logTrace();
  return oms2::Scope::GetInstance().exists(oms2::ComRef(cref)) ? 1 : 0;
}

