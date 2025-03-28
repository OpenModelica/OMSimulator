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

#ifndef _OMSIMULATOR_H_
#define _OMSIMULATOR_H_

#include "OMSimulator/Types.h"

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

OMSAPI oms_status_enu_t OMSCALL oms_activateVariant(const char* crefA, const char* crefB);
OMSAPI oms_status_enu_t OMSCALL oms_addBus(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_addConnection(const char* crefA, const char* crefB, bool suppressUnitConversion);
OMSAPI oms_status_enu_t OMSCALL oms_addConnector(const char* cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
OMSAPI oms_status_enu_t OMSCALL oms_addConnectorToBus(const char* busCref, const char* connectorCref);
OMSAPI oms_status_enu_t OMSCALL oms_addDynamicValueIndicator(const char* signal, const char* lower, const char* upper, double stepSize);
OMSAPI oms_status_enu_t OMSCALL oms_addEventIndicator(const char* signal);
OMSAPI oms_status_enu_t OMSCALL oms_addResources(const char* cref, const char* path);
OMSAPI oms_status_enu_t OMSCALL oms_addSignalsToResults(const char* cref, const char* regex);
OMSAPI oms_status_enu_t OMSCALL oms_addStaticValueIndicator(const char* signal, double lower, double upper, double stepSize);
OMSAPI oms_status_enu_t OMSCALL oms_addSubModel(const char* cref, const char* fmuPath);
OMSAPI oms_status_enu_t OMSCALL oms_addSystem(const char* cref, oms_system_enu_t type);
OMSAPI oms_status_enu_t OMSCALL oms_addTimeIndicator(const char* signal);
OMSAPI int OMSCALL oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);
OMSAPI oms_status_enu_t OMSCALL oms_copySystem(const char* source, const char* target);
OMSAPI oms_status_enu_t OMSCALL oms_delete(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_deleteConnection(const char* crefA, const char* crefB);
OMSAPI oms_status_enu_t OMSCALL oms_deleteConnectorFromBus(const char* busCref, const char* connectorCref);
OMSAPI oms_status_enu_t OMSCALL oms_deleteResources(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_doStep(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_duplicateVariant(const char* crefA, const char* crefB);
OMSAPI oms_status_enu_t OMSCALL oms_export(const char* cref, const char* filename);
OMSAPI oms_status_enu_t OMSCALL oms_exportDependencyGraphs(const char* cref, const char* initialization, const char* event, const char* simulation);
OMSAPI oms_status_enu_t OMSCALL oms_exportSnapshot(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms_exportSSMTemplate(const char * cref, const char * filename);
OMSAPI oms_status_enu_t OMSCALL oms_exportSSVTemplate(const char* cref, const char* filename);
OMSAPI void OMSCALL oms_freeMemory(void* obj);
OMSAPI oms_status_enu_t OMSCALL oms_freeState(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_getBoolean(const char* cref, bool* value);
OMSAPI oms_status_enu_t OMSCALL oms_getBus(const char* cref, oms_busconnector_t** busConnector);
OMSAPI oms_status_enu_t OMSCALL oms_getComponentType(const char* cref, oms_component_enu_t* type);
OMSAPI oms_status_enu_t OMSCALL oms_getConnections(const char* cref, oms_connection_t*** connections);
OMSAPI oms_status_enu_t OMSCALL oms_getConnector(const char* cref, oms_connector_t** connector);
OMSAPI oms_status_enu_t OMSCALL oms_getDirectionalDerivative(const char* unknownCref, const char* knownCref, double* value);
OMSAPI oms_status_enu_t OMSCALL oms_getElement(const char* cref, oms_element_t** element);
OMSAPI oms_status_enu_t OMSCALL oms_getElements(const char* cref, oms_element_t*** elements);
OMSAPI oms_status_enu_t OMSCALL oms_getFixedStepSize(const char* cref, double* stepSize);
OMSAPI oms_status_enu_t OMSCALL oms_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo);
OMSAPI oms_status_enu_t OMSCALL oms_getInteger(const char* cref, int* value);
OMSAPI oms_status_enu_t OMSCALL oms_getModelState(const char* cref, oms_modelState_enu_t* modelState);
OMSAPI oms_status_enu_t OMSCALL oms_getReal(const char* cref, double* value);
OMSAPI oms_status_enu_t OMSCALL oms_getResultFile(const char* cref, char** filename, int* bufferSize);
OMSAPI oms_status_enu_t OMSCALL oms_getSolver(const char* cref, oms_solver_enu_t* solver);
OMSAPI oms_status_enu_t OMSCALL oms_getStartTime(const char* cref, double* startTime);
OMSAPI oms_status_enu_t OMSCALL oms_getState(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_getStopTime(const char* cref, double* stopTime);
OMSAPI oms_status_enu_t OMSCALL oms_getString(const char* cref, char** value);
OMSAPI oms_status_enu_t OMSCALL oms_getSubModelPath(const char* cref, char** path);
OMSAPI oms_status_enu_t OMSCALL oms_getSystemType(const char* cref, oms_system_enu_t* type);
OMSAPI oms_status_enu_t OMSCALL oms_getTime(const char* cref, double* time);
OMSAPI oms_status_enu_t OMSCALL oms_getTolerance(const char* cref, double* relativeTolerance);
OMSAPI oms_status_enu_t OMSCALL oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize);
OMSAPI const char* OMSCALL oms_getVersion();
OMSAPI oms_status_enu_t OMSCALL oms_getWorkingDirectory(char** newWorkingDir);
OMSAPI oms_status_enu_t OMSCALL oms_importFile(const char* filename, char** cref);
OMSAPI oms_status_enu_t OMSCALL oms_importSnapshot(const char* cref, const char* snapshot, char** newCref);
OMSAPI oms_status_enu_t OMSCALL oms_initialize(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_instantiate(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_list(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms_listUnconnectedConnectors(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms_listVariants(const char* cref, char** contents);
OMSAPI oms_status_enu_t OMSCALL oms_loadSnapshot(const char* cref, const char* snapshot, char** newCref);
OMSAPI oms_status_enu_t OMSCALL oms_newModel(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_newResources(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_reduceSSV(const char* cref, const char* ssvfile, const char* ssmfile, const char* filepath);
OMSAPI oms_status_enu_t OMSCALL oms_referenceResources(const char* cref, const char* ssmFile);
OMSAPI oms_status_enu_t OMSCALL oms_removeSignalsFromResults(const char* cref, const char* regex);
OMSAPI oms_status_enu_t OMSCALL oms_rename(const char* cref, const char* newCref);
OMSAPI oms_status_enu_t OMSCALL oms_replaceSubModel(const char* cref, const char* fmuPath, bool dryRun, int* warningCount);
OMSAPI oms_status_enu_t OMSCALL oms_reset(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_RunFile(const char* filename);
OMSAPI oms_status_enu_t OMSCALL oms_setBoolean(const char* cref, bool value);
OMSAPI oms_status_enu_t OMSCALL oms_setBusGeometry(const char* bus, const ssd_connector_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms_setCommandLineOption(const char* cmd);
OMSAPI oms_status_enu_t OMSCALL oms_setConnectionGeometry(const char* crefA, const char* crefB, const ssd_connection_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms_setConnectorGeometry(const char* cref, const ssd_connector_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry);
OMSAPI oms_status_enu_t OMSCALL oms_setFixedStepSize(const char* cref, double stepSize);
OMSAPI oms_status_enu_t OMSCALL oms_setInteger(const char* cref, int value);
OMSAPI oms_status_enu_t OMSCALL oms_setLogFile(const char* filename);
OMSAPI void OMSCALL oms_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message));
OMSAPI oms_status_enu_t OMSCALL oms_setLoggingInterval(const char* cref, double loggingInterval);
OMSAPI oms_status_enu_t OMSCALL oms_setLoggingLevel(int logLevel);
OMSAPI void OMSCALL oms_setMaxLogFileSize(const unsigned long size);
OMSAPI oms_status_enu_t OMSCALL oms_setReal(const char* cref, double value);
OMSAPI oms_status_enu_t OMSCALL oms_setRealInputDerivative(const char* cref, double value);
OMSAPI oms_status_enu_t OMSCALL oms_setResultFile(const char* cref, const char* filename, int bufferSize);
OMSAPI oms_status_enu_t OMSCALL oms_setSolver(const char* cref, oms_solver_enu_t solver);
OMSAPI oms_status_enu_t OMSCALL oms_setStartTime(const char* cref, double startTime);
OMSAPI oms_status_enu_t OMSCALL oms_setState(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_setStopTime(const char* cref, double stopTime);
OMSAPI oms_status_enu_t OMSCALL oms_setString(const char* cref, const char* value);
OMSAPI oms_status_enu_t OMSCALL oms_setTempDirectory(const char* newTempDir);
OMSAPI oms_status_enu_t OMSCALL oms_setTolerance(const char* cref, double relativeTolerance);
OMSAPI oms_status_enu_t OMSCALL oms_setUnit(const char* cref, const char* value);
OMSAPI oms_status_enu_t OMSCALL oms_setVariableStepSize(const char* cref, double initialStepSize, double minimumStepSize, double maximumStepSize);
OMSAPI oms_status_enu_t OMSCALL oms_setWorkingDirectory(const char* newWorkingDir);
OMSAPI oms_status_enu_t OMSCALL oms_simulate(const char* cref);
OMSAPI oms_status_enu_t OMSCALL oms_stepUntil(const char* cref, double stopTime);
OMSAPI oms_status_enu_t OMSCALL oms_terminate(const char* cref);

#ifdef __cplusplus
}
#endif

#endif
