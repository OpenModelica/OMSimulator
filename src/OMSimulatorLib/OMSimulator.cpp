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

#include "CompositeModel.h"
#include "MatReader.h"
#include "oms2/ComRef.h"
#include "oms2/Element.h"
#include "oms2/FMICompositeModel.h"
#include "oms2/Logging.h"
#include "oms2/Scope.h"
#include "oms2/TLMCompositeModel.h"
#include "ResultReader.h"
#include "Settings.h"
#include "Types.h"
#include "Version.h"

#include <string>

#include <boost/filesystem.hpp>

void* oms_newModel()
{
  logTrace();
  CompositeModel* pModel = new CompositeModel();
  return (void*)pModel;
}

void* oms_loadModel(const char* filename)
{
  logTrace();
  CompositeModel* pModel = new CompositeModel(filename);
  return (void*)pModel;
}

void oms_unload(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_unload: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  delete pModel;
}

void oms_instantiateFMU(void* model, const char* filename, const char* instanceName)
{
  logTrace();
  if (!model)
  {
    logError("oms_instantiateFMU: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->instantiateFMU(filename, instanceName);
}

void oms_instantiateTable(void* model, const char* filename, const char* instanceName)
{
  logTrace();
  if (!model)
  {
    logError("oms_instantiateTable: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->instantiateTable(filename, instanceName);
}

void oms_setReal(void *model, const char *var, double value)
{
  logTrace();
  if (!model)
  {
    logError("oms_setReal: invalid pointer");
    return;
  }

  CompositeModel *pModel = (CompositeModel *)model;
  pModel->setReal(var, value);
}

void oms_setInteger(void *model, const char *var, int value)
{
  logTrace();
  if (!model)
  {
    logError("oms_setInteger: invalid pointer");
    return;
  }

  CompositeModel *pModel = (CompositeModel *)model;
  pModel->setInteger(var, value);
}

void oms_setBoolean(void *model, const char *var, int value)
{
  logTrace();
  if (!model)
  {
    logError("oms_setBoolean: invalid pointer");
    return;
  }

  CompositeModel *pModel = (CompositeModel *)model;
  pModel->setBoolean(var, value);
}

double oms_getReal(void *model, const char *var)
{
  logTrace();
  if (!model)
  {
    /// \todo Provide suitable return value to handle unsuccessful calls.
    logError("oms_getReal: invalid pointer");
  }

  CompositeModel *pModel = (CompositeModel *)model;
  return pModel->getReal(var);
}

int oms_getInteger(void *model, const char *var)
{
  logTrace();
  if (!model)
  {
    /// \todo Provide suitable return value to handle unsuccessful calls.
    logError("oms_getInteger: invalid pointer");
  }

  CompositeModel *pModel = (CompositeModel *)model;
  return pModel->getInteger(var);
}

int oms_getBoolean(void *model, const char *var)
{
  logTrace();
  if (!model)
  {
    /// \todo Provide suitable return value to handle unsuccessful calls.
    logError("oms_getBoolean: invalid pointer");
  }

  CompositeModel *pModel = (CompositeModel *)model;
  return pModel->getBoolean(var);
}

void oms_addConnection(void* model, const char* from, const char* to)
{
  logTrace();
  if (!model)
  {
    logError("oms_addConnection: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->addConnection(from, to);
}

oms_status_enu_t oms_simulate(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_simulate: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->simulate();
}

oms_status_enu_t oms_doSteps(const void* model, const int numberOfSteps)
{
  logTrace();
  if (!model)
  {
    logError("oms_doSteps: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->doSteps(numberOfSteps);
}

oms_status_enu_t oms_stepUntil(const void* model, const double timeValue)
{
  logTrace();
  if (!model)
  {
    logError("oms_stepUntil: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->stepUntil(timeValue);
}

void oms_describe(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_describe: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->describe();
}

void oms_exportXML(void* model, const char* filename)
{
  logTrace();
  if (!model)
  {
    logError("oms_exportXML: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->exportXML(filename);
}

void oms_importXML(void* model, const char* filename)
{
  logTrace();
  if (!model)
  {
    logError("oms_importXML: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->importXML(filename);
}

void oms_exportDependencyGraph(void* model, const char* filename)
{
  logTrace();
  if (!model)
  {
    logError("oms_exportDependencyGraph: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->exportDependencyGraph(filename);
}

void oms_exportCompositeStructure(void* model, const char* filename)
{
  logTrace();
  if (!model)
  {
    logError("oms_exportCompositeStructure: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->exportCompositeStructure(filename);
}

oms_status_enu_t oms_initialize(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_initialize: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->initialize();
  return oms_status_ok;
}

oms_status_enu_t oms_terminate(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_terminate: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->terminate();
  return oms_status_ok;
}

oms_status_enu_t oms_reset(void* model)
{
  logTrace();
  if (!model)
  {
    logError("oms_reset: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->reset();
  return oms_status_ok;
}

oms_status_enu_t oms_getCurrentTime(const void* model, double* time)
{
  logTrace();
  if (!model)
  {
    logError("oms_getCurrentTime: invalid pointer");
    return oms_status_error;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getCurrentTime(time);
}

oms_status_enu_t oms_setTempDirectory(const char* path)
{
  logTrace();
  return oms2::Scope::SetTempDirectory(path);
}

void oms_setWorkingDirectory(const char* path)
{
  logTrace();

  if (!boost::filesystem::is_directory(path))
  {
    logError("oms_setWorkingDirectory: set working directory to \"" + std::string(path) + "\" failed");
  }

  boost::filesystem::current_path(path);
}

void oms_setStartTime(void* model, double startTime)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->getSettings().SetStartTime(startTime);
}

void oms_setStopTime(void* model, double stopTime)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->getSettings().SetStopTime(stopTime);
}

void oms_setTolerance(void* model, double tolerance)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->getSettings().SetTolerance(tolerance);
}

void oms_setCommunicationInterval(void* model, double CommunicationInterval)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->getSettings().SetCommunicationInterval(CommunicationInterval);
}

void oms_setResultFile(void* model, const char* filename)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->getSettings().SetResultFile(filename);
}

void oms_setSolverMethod(void* model, const char* instanceName, const char* method)
{
  logTrace();
  CompositeModel* pModel = (CompositeModel*)model;
  pModel->SetSolverMethod(instanceName, method);
}

oms_status_enu_t oms_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
}

const char* oms_getVersion()
{
  return oms_git_version;
}

int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol)
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

void oms_setVariableFilter(void* model, const char* instanceFilter, const char* variableFilter)
{
  logTrace();
  if (!model)
  {
    logError("oms_unload: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->setVariableFilter(instanceFilter, variableFilter);
}

int oms_getNumberOfInterfaces(void *model)
{
  if (!model)
  {
    return -1;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getNumberOfInterfaces();
}

oms_causality_enu_t oms_getInterfaceCausality(void *model, int idx)
{
  if (!model)
  {
    return oms_causality_undefined;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getInterfaceCausality(idx);
}

const char *oms_getInterfaceName(void *model, int idx)
{
  if (!model)
  {
    logError("oms_getInterfaceName: invalid pointer");
    return NULL;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getInterfaceName(idx);
}

const char *oms_getInterfaceVariable(void *model, int idx)
{
  if (!model)
  {
    logError("oms_getInterfaceName: invalid pointer");
    return NULL;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getInterfaceVariable(idx);
}

int oms_getMaxIterations(void* model)
{
  if (!model)
  {
    logError("oms_getMaxIterations: invalid pointer");
    return 0;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  return pModel->getMaxIterations();
}

void oms_setMaxIterations(void* model, int maxIterations)
{
  if (!model)
  {
    logError("oms_setMaxIterations: invalid pointer");
    return;
  }

  CompositeModel* pModel = (CompositeModel*)model;
  pModel->setMaxIterations(maxIterations);
}

/* ************************************ */
/* OMSimulator 2.0                      */
/*                                      */
/* TODO: replace prefix oms2 with oms   */
/* ************************************ */

oms_status_enu_t oms2_newFMIModel(const char* ident)
{
  logTrace();
  return oms2::Scope::newFMIModel(oms2::ComRef(ident));
}

oms_status_enu_t oms2_newTLMModel(const char* ident)
{
  logTrace();
  return oms2::Scope::newTLMModel(oms2::ComRef(ident));
}

oms_status_enu_t oms2_unloadModel(const char* ident)
{
  logTrace();
  return oms2::Scope::unloadModel(oms2::ComRef(ident));
}

oms_status_enu_t oms2_addFMU(const char* modelIdent, const char* fmuPath, const char* fmuIdent)
{
  logTrace();
  return oms2::Scope::addFMU(oms2::ComRef(modelIdent), fmuPath, oms2::ComRef(fmuIdent));
}

oms_status_enu_t oms2_deleteSubModel(const char* modelIdent, const char* subModelIdent)
{
  logTrace();
  return oms2::Scope::deleteSubModel(oms2::ComRef(modelIdent), oms2::ComRef(subModelIdent));
}

oms_status_enu_t oms2_rename(const char* identOld, const char* identNew)
{
  logTrace();
  return oms2::Scope::rename(oms2::ComRef(identOld), oms2::ComRef(identNew));
}

oms_status_enu_t oms2_loadModel(const char* filename, char** ident)
{
  logTrace();
  oms2::Model* model = oms2::Scope::loadModel(filename);

  if (!model)
    return oms_status_error;

  *ident = const_cast<char*>(model->getName().c_str());
  return oms_status_ok;
}

oms_status_enu_t oms2_saveModel(const char* filename, const char* ident)
{
  logTrace();
  return oms2::Scope::saveModel(filename, oms2::ComRef(ident));
}

oms_status_enu_t oms2_getElement(const char* cref, oms_element_t** element)
{
  logTrace();
  return oms2::Scope::getElement(oms2::ComRef(cref), reinterpret_cast<oms2::Element**>(element));
}

oms_status_enu_t oms2_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry)
{
  logTrace();
  return oms2::Scope::setElementGeometry(oms2::ComRef(cref), reinterpret_cast<const oms2::ssd::ElementGeometry*>(geometry));
}

oms_status_enu_t oms2_getElements(const char* cref, oms_element_t*** elements)
{
  logTrace();
  return oms2::Scope::getElements(oms2::ComRef(cref), reinterpret_cast<oms2::Element***>(elements));
}

oms_status_enu_t oms2_getFMUPath(const char* cref, char** path)
{
  logTrace();
  return oms2::Scope::getFMUPath(oms2::ComRef(cref), path);
}

oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections)
{
  logTrace();
  return oms2::Scope::getConnections(oms2::ComRef(cref), reinterpret_cast<oms2::Connection***>(connections));
}

oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();
  return oms2::Scope::addConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
}

oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();
  return oms2::Scope::deleteConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
}

oms_status_enu_t oms2_updateConnection(const char* cref, const char* conA, const char* conB, const oms_connection_t* connection)
{
  logTrace();
  return oms2::Scope::updateConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB), reinterpret_cast<const oms2::Connection*>(connection));
}

oms_status_enu_t oms2_simulate_asynchronous(const char* ident, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  logTrace();
  logError("oms2_simulate_asynchronous: not implemented yet");
  return oms_status_error;
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
