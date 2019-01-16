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

#ifndef _OMSIMULATOR_H_
#define _OMSIMULATOR_H_

#include "Types.h"

/* define OMSimulatorLib_EXPORTS *only* when building the DLL */
#if defined(OMS_STATIC)
  #define OMSAPI
  #define OMSCALL
#else
  #if defined(_MSC_VER) || defined(__MINGW32__)
    #ifdef OMSimulatorLib_EXPORTS
      #define OMSAPI __declspec(dllexport)
      #define OMSCALL __cdecl
    #else
      #define OMSAPI __declspec(dllimport)
      #define OMSCALL __cdecl
    #endif
  #else
    #define OMSAPI
    #define OMSCALL
  #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ************************************ */
/* oms3                                 */
/*                                      */
/* ************************************ */

OMSAPI oms_status_enu_t OMSCALL oms3_addBus(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_addConnection(const char* crefA, const char* crefB);
OMSAPI oms_status_enu_t OMSCALL oms3_addConnector(const char* cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
OMSAPI oms_status_enu_t OMSCALL oms3_addConnectorToBus(const char* busCref, const char* connectorCref);
OMSAPI oms_status_enu_t OMSCALL oms3_addConnectorToTLMBus(const char* busCref, const char* connectorCref, const char *type);
OMSAPI oms_status_enu_t OMSCALL oms3_addExternalModel(const char* cref, const char* path, const char* startscript);
OMSAPI oms_status_enu_t OMSCALL oms3_addSignalsToResults(const char* cref, const char* regex);
OMSAPI oms_status_enu_t OMSCALL oms3_addSubModel(const char* cref, const char* fmuPath);
OMSAPI oms_status_enu_t OMSCALL oms3_addSystem(const char* cref, oms_system_enu_t type);
OMSAPI oms_status_enu_t OMSCALL oms3_addTLMBus(const char* cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
OMSAPI oms_status_enu_t OMSCALL oms3_addTLMConnection(const char* crefA, const char* crefB, double delay, double alpha, double linearimpedance, double angularimpedance);
OMSAPI oms_status_enu_t OMSCALL oms3_cancelSimulation_asynchronous(const char* cref);
OMSAPI int OMSCALL oms3_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);
OMSAPI oms_status_enu_t OMSCALL oms3_copySystem(const char* source, const char* target);
OMSAPI oms_status_enu_t OMSCALL oms3_delete(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_deleteConnection(const char* crefA, const char* crefB);
OMSAPI oms_status_enu_t OMSCALL oms3_deleteConnectorFromBus(const char* busCref, const char* connectorCref);
OMSAPI oms_status_enu_t OMSCALL oms3_deleteConnectorFromTLMBus(const char* busCref, const char* connectorCref);
OMSAPI oms_status_enu_t OMSCALL oms3_export(const char* cref, const char* filename);
OMSAPI oms_status_enu_t OMSCALL oms3_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);
OMSAPI oms_status_enu_t OMSCALL oms3_extractFMIKind(const char* filename, oms_fmi_kind_enu_t* kind);
OMSAPI oms_status_enu_t OMSCALL oms3_freeMemory(void* obj);
OMSAPI oms_status_enu_t OMSCALL oms3_getBoolean(const char* cref, bool* value);
OMSAPI oms_status_enu_t OMSCALL oms3_getBus(const char* cref, oms3_busconnector_t** busConnector);
OMSAPI oms_status_enu_t OMSCALL oms3_getComponentType(const char* cref, oms_component_enu_t* type);
OMSAPI oms_status_enu_t OMSCALL oms3_getConnections(const char* cref, oms3_connection_t*** connections);
OMSAPI oms_status_enu_t OMSCALL oms3_getConnector(const char* cref, oms_connector_t** connector);
OMSAPI oms_status_enu_t OMSCALL oms3_getElement(const char* cref, oms3_element_t** element);
OMSAPI oms_status_enu_t OMSCALL oms3_getElements(const char* cref, oms3_element_t*** elements);
OMSAPI oms_status_enu_t OMSCALL oms3_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo);
OMSAPI oms_status_enu_t OMSCALL oms3_getInteger(const char* cref, int* value);
OMSAPI oms_status_enu_t OMSCALL oms3_getModelState(const char* cref, oms_modelState_enu_t* modelState);
OMSAPI oms_status_enu_t OMSCALL oms3_getReal(const char* cref, double* value);
OMSAPI oms_status_enu_t OMSCALL oms3_getStartTime(const char* cref, double* startTime);
OMSAPI oms_status_enu_t OMSCALL oms3_getStopTime(const char* cref, double* stopTime);
OMSAPI oms_status_enu_t OMSCALL oms3_getSubModelPath(const char* cref, char** path);
OMSAPI oms_status_enu_t OMSCALL oms3_getSystemType(const char* cref, oms_system_enu_t* type);
OMSAPI oms_status_enu_t OMSCALL oms3_getTLMBus(const char* cref, oms3_tlmbusconnector_t** tlmBusConnector);
OMSAPI oms_status_enu_t OMSCALL oms3_getTLMVariableTypes(oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation, char ***types, char ***descriptions);
OMSAPI const char* OMSCALL oms3_getVersion();
OMSAPI oms_status_enu_t OMSCALL oms3_import(const char* filename, char** cref);
OMSAPI oms_status_enu_t OMSCALL oms3_importString(const char* contents, char** cref);
OMSAPI oms_status_enu_t OMSCALL oms3_initialize(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_instantiate(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_list(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms3_listUnconnectedConnectors(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms3_newModel(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_parseModelName(const char* contents, char** cref);
OMSAPI oms_status_enu_t OMSCALL oms3_removeSignalsFromResults(const char* cref, const char* regex);
OMSAPI oms_status_enu_t OMSCALL oms3_rename(const char* cref, const char* newCref);
OMSAPI oms_status_enu_t OMSCALL oms3_reset(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_RunFile(const char* filename);
OMSAPI oms_status_enu_t OMSCALL oms3_setBoolean(const char* cref, bool value);
OMSAPI oms_status_enu_t OMSCALL oms3_setBusGeometry(const char* bus, const ssd_connector_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms3_setCommandLineOption(const char* cmd);
OMSAPI oms_status_enu_t OMSCALL oms3_setConnectionGeometry(const char* crefA, const char* crefB, const ssd_connection_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms3_setConnectorGeometry(const char* cref, const ssd_connector_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms3_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms3_setFixedStepSize(const char* cref, double stepSize);
OMSAPI oms_status_enu_t OMSCALL oms3_setInteger(const char* cref, int value);
OMSAPI oms_status_enu_t OMSCALL oms3_setLogFile(const char* filename);
OMSAPI void OMSCALL oms3_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message));
OMSAPI oms_status_enu_t OMSCALL oms3_setLoggingInterval(const char* cref, double loggingInterval);
OMSAPI oms_status_enu_t OMSCALL oms3_setLoggingLevel(int logLevel);
OMSAPI void OMSCALL oms3_setMaxLogFileSize(const unsigned long size);
OMSAPI oms_status_enu_t OMSCALL oms3_setReal(const char* cref, double value);
OMSAPI oms_status_enu_t OMSCALL oms3_setResultFile(const char* cref, const char* filename, int bufferSize);
OMSAPI oms_status_enu_t OMSCALL oms3_setSignalFilter(const char* cref, const char* regex);
OMSAPI oms_status_enu_t OMSCALL oms3_setSolver(const char* cref, const char* solver);
OMSAPI oms_status_enu_t OMSCALL oms3_setStartTime(const char* cref, double startTime);
OMSAPI oms_status_enu_t OMSCALL oms3_setStopTime(const char* cref, double stopTime);
OMSAPI oms_status_enu_t OMSCALL oms3_setTempDirectory(const char* newTempDir);
OMSAPI oms_status_enu_t OMSCALL oms3_setTLMBusGeometry(const char* bus, const ssd_connector_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms3_setTLMConnectionParameters(const char* crefA, const char* crefB, const oms3_tlm_connection_parameters_t* parameters);
OMSAPI oms_status_enu_t OMSCALL oms3_setTLMPositionAndOrientation(const char *cref, double x1, double x2, double x3, double A11, double A12, double A13, double A21, double A22, double A23, double A31, double A32, double A33);
OMSAPI oms_status_enu_t OMSCALL oms3_setTLMSocketData(const char* cref, const char* address, int managerPort, int monitorPort);
OMSAPI oms_status_enu_t OMSCALL oms3_setTolerance(const char* cref, double tolerance);
OMSAPI oms_status_enu_t OMSCALL oms3_setWorkingDirectory(const char* newWorkingDir);
OMSAPI oms_status_enu_t OMSCALL oms3_simulate_asynchronous(const char* cref, void (*cb)(const char* cref, double time, oms_status_enu_t status));
OMSAPI oms_status_enu_t OMSCALL oms3_simulate(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms3_stepUntil(const char* cref, double stopTime);
OMSAPI oms_status_enu_t OMSCALL oms3_terminate(const char* cref);

/* not implemented yet */
oms_status_enu_t oms3_setSimulationInformation(const char* cref, ssd_simulation_information_t* info);
oms_status_enu_t oms3_getSimulationInformation(const char* cref, ssd_simulation_information_t** info);

#ifdef __cplusplus
}
#endif

#endif
