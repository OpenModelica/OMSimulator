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

#include "MatReader.h"
#include "ComRef.h"
#include "Element.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "Model.h"
#include "Scope.h"
#include "ResultReader.h"
#include "Types.h"
#include "Version.h"

#include <string>
#include <boost/filesystem.hpp>

/* ************************************ */
/* OMSimulator 2.0                      */
/*                                      */
/* TODO: replace prefix oms2 with oms   */
/* ************************************ */

oms_status_enu_t oms2_setLogFile(const char* filename)
{
  return Log::setLogFile(filename);
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
  return oms2::Scope::GetInstance().newTLMModel(oms2::ComRef(ident));
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

  if (!model)
    return oms_status_error;

  oms_element_t* element = reinterpret_cast<oms_element_t*>(model->getElement());
  *ident = element->name;
  return oms_status_ok;
}

oms_status_enu_t oms2_saveModel(const char* filename, const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().saveModel(filename, oms2::ComRef(ident));
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

oms_status_enu_t oms2_getFMUPath(const char* cref, char** path)
{
  logTrace();
  return oms2::Scope::GetInstance().getFMUPath(oms2::ComRef(cref), path);
}

oms_status_enu_t oms2_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo)
{
  logTrace();
  return oms2::Scope::GetInstance().getFMUInfo(oms2::ComRef(cref), reinterpret_cast<const oms2::FMUInfo**>(fmuInfo));
}

oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections)
{
  logTrace();
  return oms2::Scope::GetInstance().getConnections(oms2::ComRef(cref), reinterpret_cast<oms2::Connection***>(connections));
}

oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();
  return oms2::Scope::GetInstance().addConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
}

oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB)
{
  logTrace();
  return oms2::Scope::GetInstance().deleteConnection(oms2::ComRef(cref), oms2::SignalRef(conA), oms2::SignalRef(conB));
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

oms_status_enu_t oms2_terminate(const char* ident)
{
  logTrace();
  return oms2::Scope::GetInstance().terminate(oms2::ComRef(ident));
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
  if (!type)
  {
    logError("oms2_getComponentType: type is NULL pointer");
    return oms_status_error;
  }

  *type = oms_component_none;
  logError("oms2_getComponentType: not implemented yet");
  return oms_status_error;
}

oms_status_enu_t oms2_describe(const char* cref)
{
  logTrace();
  return oms2::Scope::GetInstance().describeModel(oms2::ComRef(cref));
}

oms_status_enu_t oms2_addExternalModel(const char *cref, const char *name, const char *modelfile, const char *startscript)
{
  logTrace();
  return oms2::Scope::GetInstance().addExternalModel(oms2::ComRef(cref), oms2::ComRef(name), modelfile, startscript);
}

oms_status_enu_t oms2_addTLMInterface(const char *cref, const char *subref, const char *name, int dimensions, oms_causality_enu_t causality, oms_tlm_interpolation_t interpolation, const char *domain, const char **sigrefs, int nsigrefs)
{
  logTrace();
  std::vector<oms2::SignalRef>vSigrefs;
  for(int i=0; i<nsigrefs; ++i) {
    vSigrefs.push_back(oms2::SignalRef(sigrefs[i]));
  }
  return oms2::Scope::GetInstance().addTLMInterface(oms2::ComRef(cref), oms2::ComRef(subref), oms2::ComRef(name), dimensions, causality, domain, interpolation, vSigrefs);
}

oms_status_enu_t oms2_addTLMConnection(const char *cref, const char *from, const char *to, double delay, double alpha, double Zf, double Zfr)
{
  logTrace();
  return oms2::Scope::GetInstance().addTLMConnection(oms2::ComRef(cref), oms2::SignalRef(from), oms2::SignalRef(to), delay, alpha, Zf, Zfr);
}

oms_status_enu_t oms2_addFMISubModel(const char *cref, const char *subref)
{
  logTrace();
  return oms2::Scope::GetInstance().addFMISubModel(oms2::ComRef(cref), oms2::ComRef(subref));
}

oms_status_enu_t oms2_setTLMSocketData(const char* cref, const char* address,
                                       int managerPort, int monitorPort)
{
  logTrace();
  return oms2::Scope::GetInstance().setTLMSocketData(oms2::ComRef(cref), address,
                                                     managerPort, monitorPort);
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

oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename)
{
  logTrace();
  return oms2::Scope::GetInstance().setResultFile(oms2::ComRef(cref), std::string(filename));
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
