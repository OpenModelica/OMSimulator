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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief Creates an empty composite model.
 *
 * @return model instance as opaque pointer.
 */
void* oms_newModel();

/**
 * \brief Loads a composite model from xml representation.
 *
 * @param filename Full path to the composite model xml representation.
 * @return model instance as opaque pointer.
 */
void* oms_loadModel(const char* filename);

// TODO saveModel

/**
 * \brief Unloads a composite model.
 *
 * @param model Model as opaque pointer.
 */
void oms_unload(void* model);

/**
 * \brief Instantiates a FMU.
 *
 * @param model        Model as opaque pointer.
 * @param filename     Full path to the FMU.
 * @param instanceName Instance name for further access.
 */
void oms_instantiateFMU(void* model, const char* filename, const char* instanceName);

/**
 * \brief Instantiates a lookup table.
 *
 * @param model        Model as opaque pointer.
 * @param filename     Full path to the lookup table.
 * @param instanceName Instance name for further access.
 */
void oms_instantiateTable(void* model, const char* filename, const char* instanceName);

/**
 * \brief Set parameter and input values of type real.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @param value New value of the given variable.
 */
void oms_setReal(void *model, const char *var, double value);

/**
 * \brief Set parameter and input values of type integer.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @param value New value of the given variable.
 */
void oms_setInteger(void *model, const char *var, int value);

/**
 * \brief Set parameter and input values of type boolean.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @param value New value of the given variable.
 */
void oms_setBoolean(void *model, const char *var, int value);

// TODO: setString

/**
 * \brief Get real value.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @return value of given variable
 */
double oms_getReal(void *model, const char *var);

/**
 * \brief Get integer value.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @return value of given variable
 */
int oms_getInteger(void *model, const char *var);

/**
 * \brief Get boolean value.
 *
 * @param model Model as opaque pointer.
 * @param var   Variable name as string.
 * @return value of given variable
 */
int oms_getBoolean(void *model, const char *var);

// TODO: getString

/**
 * \brief Add a connection from a FMU output to a FMU input.
 *
 * @param model Model as opaque pointer.
 * @param from Name of an FMU output.
 * @param to Name of an FMU input.
 */
void oms_addConnection(void* model, const char* from, const char* to);

/**
 * @param model Model as opaque pointer.
 * @return Error status.
 */
oms_status_t oms_simulate(void* model);

/**
 * \brief In case of variable step sizes or events we cannot know the final time value.
 *
 * @param model Model as opaque pointer.
 * @param numberOfSteps Number of steps.
 * @return Error status.
 */
oms_status_t oms_doSteps(const void* model, const int numberOfSteps);

/**
 * \brief Performs a sub-simulation run from the current time instance to a given time value
 *
 * @param model Model as opaque pointer.
 * @param timeValue
 * @return Error status.
 */
oms_status_t oms_stepUntil(const void* model, const double timeValue);

void oms_describe(void* model);
void oms_exportXML(void* model, const char* filename);
void oms_importXML(void* model, const char* filename);
void oms_exportDependencyGraph(void* model, const char* filename);
void oms_exportCompositeStructure(void* model, const char* filename);

/**
 * \brief Initialize the simulation of a given model.
 *
 * @param model Model as opaque pointer.
 * @return Error status.
 */
oms_status_t oms_initialize(void* model);

/**
 * \brief Terminate the simulation of a given model.
 *
 * @param model Model as opaque pointer.
 * @return Error status.
 */
oms_status_t oms_terminate(void* model);

/**
 * \brief Reset the FMUs after a simulation run.
 *
 * The FMUs go into the same state as after instantiation.
 *
 * @param model Model as opaque pointer.
 * @return Error status.
 */
oms_status_t oms_reset(void* model);

/**
 * \brief Get current simulation time.
 *
 * @param model [in]  Model as opaque pointer.
 * @param time  [out] Minimal time value that is reached by all instances.
 * @return Error status.
 */
oms_status_t oms_getCurrentTime(const void* model, double* time);

/* Global settings */
void oms_setTempDirectory(const char* path);
void oms_setWorkingDirectory(const char* path);

/* Local settings */
void oms_setStartTime(void* model, double startTime);
void oms_setStopTime(void* model, double stopTime);
void oms_setTolerance(void* model, double tolerance);
void oms_setCommunicationInterval(void* model, double communicationInterval);
void oms_setResultFile(void* model, const char* filename);
void oms_setSolverMethod(void* model, const char* instanceName, const char* method);

/**
 * \brief Returns the library's version string.
 *
 * @return version string
 */
const char* oms_getVersion();

/**
 * \brief Compares simulation results.
 */
int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);

/**
 * \brief Sets the variable filter
 *
 * @param model          [in] Model as opaque pointer.
 * @param instanceFilter [in] Regex to select the FMU instances.
 * @param variableFilter [in] Regex to select the variables.
 */
void oms_setVariableFilter(void* model, const char* instanceFilter, const char* variableFilter);

/**
 * \brief Returns the number of external interfaces
 *
 * @param model   [in] Model as opaque pointer.
 * @return        Number of interfaces
 */
int oms_getNumberOfInterfaces(void* model);

/**
 * \brief Returns the causality for specified external interface
 *
 * @param model   [in] Model as opaque pointer.
 * @param idx     [in] Interface index
 * @return        Interface causality
 */
oms_causality_t oms_getInterfaceCausality(void* model, int idx);

/**
 * \brief Returns the name for specified external interface
 *
 * @param model   [in] Model as opaque pointer.
 * @param idx     [in] Interface index
 * @return        Interface name
 */
const char* oms_getInterfaceName(void* model, int idx);

/**
 * \brief Returns the local variable mapped to specified external interface
 *
 * @param model   [in] Model as opaque pointer.
 * @param idx     [in] Interface index
 * @return        Mapped variable
 */
const char* oms_getInterfaceVariable(void* model, int idx);

/**
 * \brief Returns the max. number of iterations used to solve algebraic loops.
 *
 * @param model   [in] Model as opaque pointer.
 */
int oms_getMaxIterations(void* model);

/**
 * \brief Sets the max. number of iterations used to solve algebraic loops.
 *
 * @param model           [in] Model as opaque pointer.
 * @param maxIterations   [in] Max. number of iterations used to solve algebraic loops.
 */
void oms_setMaxIterations(void* model, int maxIterations);

/* ************************************ */
/* OMSimulator 2.0                      */
/*                                      */
/* TODO: replace prefix oms2 with oms   */
/* ************************************ */

/**
 * \brief Creates a new and yet empty FMI composite model.
 *
 * @param ident   [in] Name of the new model instance.
 */
oms_status_t oms2_newFMIModel(const char* ident);

/**
 * \brief Creates a new and yet empty TLM composite model.
 *
 * @param ident   [in] Name of the new model instance.
 */
oms_status_t oms2_newTLMModel(const char* ident);

/**
 * \brief Unloads a composite model (works for FMI and TLM).
 *
 * @param ident   [in] Name of the model instance.
 */
oms_status_t oms2_unloadModel(const char* ident);

/**
 * \brief Unloads a composite model (works for FMI and TLM).
 *
 * @param identOld   [in] Name of the model instance to rename.
 * @param identNew   [in] New name of the model instance.
 */
oms_status_t oms2_renameModel(const char* identOld, const char* identNew);

oms_status_t oms2_loadModel(const char* filename, const char** ident);
oms_status_t oms2_saveModel(const char* ident, const char* filename);
oms_status_t oms2_getIconAnnotation(const char* ident, const void** annotation);
oms_status_t oms2_setIconAnnotation(const char* ident, const void* annotation);
oms_status_t oms2_getDiagramAnnotation(const char* ident, const void** annotation);
oms_status_t oms2_setDiagramAnnotation(const char* ident, const void* annotation);
oms_status_t oms2_getComponents(const char* ident, const oms_component_t** oms_component);
oms_status_t oms2_getConnections(const char* ident, const oms_component_t** oms_connections);
oms_status_t oms2_getConnectionAnnotation(const char* identA, const char* identB, const char** annotation);
oms_status_t oms2_setConnectionAnnotation(const char* identA, const char* identB, const char** annotation);
oms_status_t oms2_simulate_asynchronous(const char* ident, void (*cb)(const char* ident, double time, oms_status_t status));

/**
 * \brief Sets a callback function for the logging system.
 *
 * @param cb   [in] Function pointer that is called for all messages to the logging system.
 */
void oms2_setLoggingCallback(void (*cb)(oms_message_type_t type, const char* message));

/**
 * \brief Enables/Disables debug logging (logDebug and logTrace).
 *
 * @param useDebugLogging   [in] 0 to disable debug logging, all other values to enable debug logging
 */
void oms2_setDebugLogging(int useDebugLogging);

/**
 * \brief Redirects logging output to file or std streams. The warning/error counters are reset.
 *
 * @param filename   [in] "" to redirect to std streams and proper filename to redirect to file.
 */
void oms_setLogFile(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
