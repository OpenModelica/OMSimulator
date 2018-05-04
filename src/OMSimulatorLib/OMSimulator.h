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
 * \return model instance as opaque pointer.
 */
void* oms_newModel();

/**
 * \brief Loads a composite model from xml representation.
 *
 * \param filename Full path to the composite model xml representation.
 * \return model instance as opaque pointer.
 */
void* oms_loadModel(const char* filename);

// TODO saveModel

/**
 * \brief Unloads a composite model.
 *
 * \param model Model as opaque pointer.
 */
void oms_unload(void* model);

/**
 * \brief Instantiates a FMU.
 *
 * \param model        Model as opaque pointer.
 * \param filename     Full path to the FMU.
 * \param instanceName Instance name for further access.
 */
void oms_instantiateFMU(void* model, const char* filename, const char* instanceName);

/**
 * \brief Instantiates a lookup table.
 *
 * \param model        Model as opaque pointer.
 * \param filename     Full path to the lookup table.
 * \param instanceName Instance name for further access.
 */
void oms_instantiateTable(void* model, const char* filename, const char* instanceName);

/**
 * \brief Set parameter and input values of type real.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \param value New value of the given variable.
 */
void oms_setReal(void *model, const char *var, double value);

/**
 * \brief Set parameter and input values of type integer.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \param value New value of the given variable.
 */
void oms_setInteger(void *model, const char *var, int value);

/**
 * \brief Set parameter and input values of type boolean.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \param value New value of the given variable.
 */
void oms_setBoolean(void *model, const char *var, int value);

/// \todo setString

/**
 * \brief Get real value.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \return value of given variable
 */
double oms_getReal(void *model, const char *var);

/**
 * \brief Get integer value.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \return value of given variable
 */
int oms_getInteger(void *model, const char *var);

/**
 * \brief Get boolean value.
 *
 * \param model Model as opaque pointer.
 * \param var   Variable name as string.
 * \return value of given variable
 */
int oms_getBoolean(void *model, const char *var);

/// \todo getString

/**
 * \brief Add a connection from a FMU output to a FMU input.
 *
 * \param model Model as opaque pointer.
 * \param from Name of an FMU output.
 * \param to Name of an FMU input.
 */
void oms_addConnection(void* model, const char* from, const char* to);

/**
 * \param model Model as opaque pointer.
 * \return Error status
 */
oms_status_enu_t oms_simulate(void* model);

/**
 * \brief In case of variable step sizes or events we cannot know the final time value.
 *
 * \param model Model as opaque pointer.
 * \param numberOfSteps Number of steps.
 * \return Error status
 */
oms_status_enu_t oms_doSteps(const void* model, const int numberOfSteps);

/**
 * \brief Performs a sub-simulation run from the current time instance to a given time value
 *
 * \param model Model as opaque pointer.
 * \param timeValue
 * \return Error status
 */
oms_status_enu_t oms_stepUntil(const void* model, const double timeValue);

void oms_describe(void* model);
void oms_exportXML(void* model, const char* filename);
void oms_importXML(void* model, const char* filename);
void oms_exportDependencyGraph(void* model, const char* filename);
void oms_exportCompositeStructure(void* model, const char* filename);

/**
 * \brief Initialize the simulation of a given model.
 *
 * \param model Model as opaque pointer.
 * \return Error status
 */
oms_status_enu_t oms_initialize(void* model);

/**
 * \brief Terminate the simulation of a given model.
 *
 * \param model Model as opaque pointer.
 * \return Error status
 */
oms_status_enu_t oms_terminate(void* model);

/**
 * \brief Reset the FMUs after a simulation run.
 *
 * The FMUs go into the same state as after instantiation.
 *
 * \param model Model as opaque pointer.
 * \return Error status
 */
oms_status_enu_t oms_reset(void* model);

/**
 * \brief Get current simulation time.
 *
 * \param model [in]  Model as opaque pointer.
 * \param time  [out] Minimal time value that is reached by all instances.
 * \return Error status
 */
oms_status_enu_t oms_getCurrentTime(const void* model, double* time);

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
 * \return version string
 */
const char* oms_getVersion();

/**
 * \brief Compares simulation results.
 */
int oms_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);

/**
 * \brief Sets the variable filter
 *
 * \param model          [in] Model as opaque pointer.
 * \param instanceFilter [in] Regex to select the FMU instances.
 * \param variableFilter [in] Regex to select the variables.
 */
void oms_setVariableFilter(void* model, const char* instanceFilter, const char* variableFilter);

/**
 * \brief Returns the number of external interfaces
 *
 * \param model   [in] Model as opaque pointer.
 * \return        Number of interfaces
 */
int oms_getNumberOfInterfaces(void* model);

/**
 * \brief Returns the causality for specified external interface
 *
 * \param model   [in] Model as opaque pointer.
 * \param idx     [in] Interface index
 * \return        Interface causality
 */
oms_causality_enu_t oms_getInterfaceCausality(void* model, int idx);

/**
 * \brief Returns the name for specified external interface
 *
 * \param model   [in] Model as opaque pointer.
 * \param idx     [in] Interface index
 * \return        Interface name
 */
const char* oms_getInterfaceName(void* model, int idx);

/**
 * \brief Returns the local variable mapped to specified external interface
 *
 * \param model   [in] Model as opaque pointer.
 * \param idx     [in] Interface index
 * \return        Mapped variable
 */
const char* oms_getInterfaceVariable(void* model, int idx);

/**
 * \brief Returns the max. number of iterations used to solve algebraic loops.
 *
 * \param model   [in] Model as opaque pointer.
 */
int oms_getMaxIterations(void* model);

/**
 * \brief Sets the max. number of iterations used to solve algebraic loops.
 *
 * \param model           [in] Model as opaque pointer.
 * \param maxIterations   [in] Max. number of iterations used to solve algebraic loops.
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
 * \param ident   [in] Name of the new model instance
 * \return        Error status
 */
oms_status_enu_t oms2_newFMIModel(const char* ident);

/**
 * \brief Creates a new and yet empty TLM composite model.
 *
 * \param ident   [in] Name of the new model instance
 * \return        Error status
 */
oms_status_enu_t oms2_newTLMModel(const char* ident);

/**
 * \brief Unloads a composite model (works for both FMI and TLM).
 *
 * \param ident   [in] Name of the model instance
 * \return        Error status
 */
oms_status_enu_t oms2_unloadModel(const char* ident);

/**
 * \brief Adds a new FMU instance to a given FMI model.
 *
 * \param modelIdent   [in] Identifier of parent model
 * \param fmuPath      [in] Full path to FMU
 * \param fmuIdent     [in] Identifier of new FMU instance
 * \return             Error status
 */
oms_status_enu_t oms2_addFMU(const char* modelIdent, const char* fmuPath, const char* fmuIdent);

/**
 * \brief Adds a new lookup table instance to a given FMI model.
 *
 * \param modelIdent   [in] Identifier of parent model
 * \param tablePath    [in] Full path to lookup table
 * \param tableIdent   [in] Identifier of new lookup table instance
 * \return             Error status
 */
oms_status_enu_t oms2_addTable(const char* modelIdent, const char* tablePath, const char* tableIdent);

/**
 * \brief Deletes a sub-model instance, e.g. FMU, from a given FMI composite
 * model.
 *
 * \param modelIdent      [in] Identifier of parent model
 * \param subModelIdent   [in] Identifier of sub-model instance
 * \return                Error status
 */
oms_status_enu_t oms2_deleteSubModel(const char* modelIdent, const char* subModelIdent);

/**
 * \brief Renames a composite model (either FMI or TLM) or sub-model (e.g. FMU
 * instance).
 *
 * \param identOld   [in] Name of the instance to rename
 * \param identNew   [in] New name of the instance
 * \return           Error status
 */
oms_status_enu_t oms2_rename(const char* identOld, const char* identNew);

/**
 * \brief Loads a FMI composite model from xml representation.
 *
 * \param filename   [in] Path to the composite model xml representation
 * \param ident      [out] Name of the imported model
 * \return           Error status
 */
oms_status_enu_t oms2_loadModel(const char* filename, char** ident);

/**
 * \brief Loads a FMI composite model from xml representation.
 *
 * \param filename   [in] Path to the xml file; An exisiting file will be overwritten
 * \param ident      [in] Name of the model to export
 * \return           Error status
 */
oms_status_enu_t oms2_saveModel(const char* filename, const char* ident);

/**
 * \brief Get element information of a model or sub-model.
 *
 * \param cref      [in] Full identifier of a component
 * \param element   [out] Geometry information
 * \return          Error status
 */
oms_status_enu_t oms2_getElement(const char* cref, oms_element_t** element);

/**
 * \brief Set geometry information to a given component (i.e. model or sub-model).
 *
 * \param cref       [in] Full identifier of a component
 * \param geometry   [in] Geometry information
 * \return           Error status
 */
oms_status_enu_t oms2_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry);

/**
 * \brief Get list of all sub-components from a given component.
 *
 * \param cref       [in] Full identifier of a component
 * \param elements   [out] Array of sub-components (null-terminated array)
 * \return           Error status
 */
oms_status_enu_t oms2_getElements(const char* cref, oms_element_t*** elements);

/**
 * \brief Returns the FMU path of a given component.
 *
 * \param cref         [in] Full identifier of a component
 * \param path         [out] FMU path
 * \return             Error status
 */
oms_status_enu_t oms2_getFMUPath(const char* cref, char** path);

/**
 * \brief Returns FMU specific information.
 *
 * \param cref      [in] Full identifier of a component
 * \param fmuInfo   [out] FMU path
 * \return          Error status
 */
oms_status_enu_t oms2_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo);

/**
 * \brief Get list of all connections from a given component.
 *
 * \param cref          [in] Full identifier of a component
 * \param connections   [out] Array of connections (null-terminated array)
 * \return              Error status
 */
oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections);

/**
 * \brief Adds a new connection to a given parent component.
 *
 * \param cref         [in] Full identifier of a component
 * \param connection   [in] New connection
 * \return             Error status
 */
oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB);

/**
 * \brief Deletes the connection between connectors A and B.
 *
 * \param cref   [in] Full identifier of a component
 * \param conA   [in] Name of connector A
 * \param conB   [in] Name of connector B
 * \return       Error status
 */
oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB);

/**
 * \brief Updates the connection between connectors A and B.
 *
 * \param cref         [in] Full identifier of a component
 * \param conA         [in] Name of connector A
 * \param conB         [in] Name of connector B
 * \param connection   [in] New connection
 * \return             Error status
 */
oms_status_enu_t oms2_updateConnection(const char* cref, const char* conA, const char* conB, const oms_connection_t* connection);

/**
 * \brief Initializes a composite model (works for both FMI and TLM).
 *
 * \param ident   [in] Name of the model instance
 * \return        Error status
 */
oms_status_enu_t oms2_initialize(const char* ident);

/**
 * \brief Simulates a composite model (works for both FMI and TLM).
 *
 * \param ident   [in] Name of the model instance
 * \return        Error status
 */
oms_status_enu_t oms2_simulate(const char* ident);

/**
 * \brief Simulates a composite model in its own thread.
 *
 * \param ident            [in] Name of the model instance
 * \param cbPeriod         [in] Period duration in seconds between calls to callback function cb
 * \param cb               [in] Callback function which to be called periodically
 * \return                 Error status
 */
oms_status_enu_t oms2_simulate_asynchronous(const char* ident, double cbPeriod, void (*cb)(const char* ident, double time, oms_status_enu_t status));

/**
 * \brief Simulates a composite model for a given number of steps (works for both FMI and TLM).
 *
 * \param ident           [in] Name of the model instance
 * \param numberOfSteps   [in] Number of steps
 * \return                Error status
 */
oms_status_enu_t oms2_doSteps(const char* ident, const int numberOfSteps);

/**
 * \brief Simulates a composite model until a given time value (works for both FMI and TLM).
 *
 * \param ident       [in] Name of the model instance
 * \param timeValue   [in] time value
 * \return            Error status
 */
oms_status_enu_t oms2_stepUntil(const char* ident, const double timeValue);

/**
 * \brief Reset the composite model after a simulation run.
 *
 * \date 2018-04-24 Implemented for FMI (no TLM support).
 *
 * The FMUs go into the same state as after instantiation.
 *
 * \param ident       [in] Name of the model instance
 * \return Error status
 */
oms_status_enu_t oms2_reset(const char* ident);

/**
 * \brief Terminates a composite model (works for both FMI and TLM).
 *
 * \param ident   [in] Name of the model instance
 * \return        Error status
 */
oms_status_enu_t oms2_terminate(const char* ident);

/**
 * \brief Sets a callback function for the logging system.
 *
 * \param cb   [in] Function pointer that is called for all messages to the logging system
 */
void oms2_setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message));

/**
 * \brief Enables/Disables debug logging (logDebug and logTrace).
 *
 * \param logLevel   [in] 0 default, 1 default+debug, 2 default+debug+trace
 */
void oms2_setLoggingLevel(int logLevel);

/**
 * \brief Describes model by printing structure to cout.
 *
 * \param cref   [in] Full identifier of a component.
 * \return       Error status
 */
oms_status_enu_t oms2_describe(const char* cref);

/**
 * \brief Adds an external model to a TLM composite model
 *
 * \param cref          [in] Identifier of parent TLM composite model.
 * \param name          [in] Identifier of external model
 * \param modelfile     [in] Absolute path to model file of external model.
 * \param startscript   [in] Start script of external model.
 * \return              Error status
 */
oms_status_enu_t oms2_addExternalModel(const char* cref, const char* name, const char* modelfile, const char* startscript);

/**
 * \brief Adds an external model to a TLM composite model
 *
 * \param cref     [in] Identifier of parent TLM composite model.
 * \param subref   [in] Identifier of sub FMI composite model.
 * \return         Error status
 */
oms_status_enu_t oms2_addFMISubModel(const char* cref, const char* subref);

/**
 * \brief Sets data for TLM socket communication
 *
 * @param cref         [in] Identifier of TLM composite model.
 * @param address      [in] Address to manager server (format: ###.###.###.###)
 * @param managerPort  [in] Port number of manager process
 * @param monitorPort  [in] Port number of monitor process
 */
oms_status_enu_t oms2_setTLMSocketData(const char* cref, const char* address,
                                       int managerPort, int monitorPort);

/**
 * \brief Adds an external model to a TLM composite model
 *
 * \param cref         [in] Identifier of TLM composite model.
 * \param subref       [in] Identifier of submodel.
 * \param name         [in] Identifier of TLM interface.
 * \param dimensions   [in] Number of dimensions of TLM interface (1, 3 or 6)
 * \param causality    [in] Causality of TLM interface.
 * \param domain       [in] Domain of TLM interface
 * \return             Error status
 */
oms_status_enu_t oms2_addTLMInterface(const char *cref, const char* subref, const char *name, int dimensions, oms_causality_enu_t causality, oms_tlm_interpolation_t interpolation, const char* domain,
                                      const char **sigrefs, int nsigrefs);


/**
 * \brief Adds an external model to a TLM composite model
 *
 * \param cref    [in] Identifier of TLM composite model.
 * \param from    [in] TLM interface (format: submodel:interface)
 * \param to      [in] TLM interface (format: submodel:interface)
 * \param delay   [in] Time delay of connection.
 * \param alpha   [in] Numerical damping factor of connection.
 * \param Zf      [in] Translational characteristic impedance of connection.
 * \param Zfr     [in] Rotational characteristic impedance of connection.
 * \return        Error status
 */
oms_status_enu_t oms2_addTLMConnection(const char* cref, const char* from, const char* to, double delay, double alpha, double Zf, double Zfr);

/**
 * \brief Redirects logging output to file or std streams. The warning/error counters are reset.
 *
 * \param filename   [in] "" to redirect to std streams and proper filename to redirect to file.
 * \return           Error status
 */
oms_status_enu_t oms2_setLogFile(const char* filename);

/**
 * \brief Set new temp directory
 *
 * \param path   [in] Path to new temp directory
 * \return       Error status
 */
oms_status_enu_t oms2_setTempDirectory(const char* path);

/**
 * \brief Set a new working directory
 *
 * \param path   [in] Path to new working directory
 * \return       Error status
 */
oms_status_enu_t oms2_setWorkingDirectory(const char* path);

/**
 * \brief Get real value.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getReal(const char* signal, double* value);

/**
 * \brief Set real value of given signal.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setReal(const char* signal, double value);

/**
 * \brief Get real value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getRealParameter(const char* signal, double* value);

/**
 * \brief Set real value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [in] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setRealParameter(const char* signal, double value);

/**
 * \brief Get integer value of given signal.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getInteger(const char* signal, int* value);

/**
 * \brief Set integer value of given signal.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [in] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setInteger(const char* signal, int value);

/**
 * \brief Get integer value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getIntegerParameter(const char* signal, int* value);

/**
 * \brief Set integer value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [in] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setIntegerParameter(const char* signal, int value);

/**
 * \brief Get boolean value of given signal.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getBoolean(const char* signal, bool* value);

/**
 * \brief Set boolean value of given signal.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [in] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setBoolean(const char* signal, bool value);

/**
 * \brief Get boolean value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [out] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_getBooleanParameter(const char* signal, bool* value);

/**
 * \brief Set boolean value of given parameter.
 *
 * \param signal   [in] Name of the signal to fetch (e.g. model.fmuA:var)
 * \param value    [in] Signal value
 * \return         Error status
 */
oms_status_enu_t oms2_setBooleanParameter(const char* signal, bool value);

/**
 * \brief Set the start time of the simulation.
 *
 * \param cref        [in] Name of the model instance
 * \param startTime   [in] Start time
 * \return            Error status
 */
oms_status_enu_t oms2_setStartTime(const char* cref, double startTime);

/**
 * \brief Set the stop time of the simulation.
 *
 * \param cref       [in] Name of the model instance
 * \param stopTime   [in] Stop time
 * \return           Error status
 */
oms_status_enu_t oms2_setStopTime(const char* cref, double stopTime);

/**
 * \brief Set the communication interval of the simulation.
 *
 * \param cref                    [in] Name of the model instance
 * \param communicationInterval   [in] Communication interval
 * \return                        Error status
 */
oms_status_enu_t oms2_setCommunicationInterval(const char* cref, double communicationInterval);

/**
 * \brief Set the result file of the simulation.
 *
 * \param cref       [in] Name of the model instance
 * \param filename   [in] Result file
 * \return           Error status
 */
oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename);


/**
 * \brief Set master algorithm variant that shall be used (default: "standard").
 *
 * Supported master algorithms: "standard"
 *
 * Experimental master algorithms (no stable API!): "pctpl", "pmrchannela", "pmrchannelcv", "pmrchannelm"
 *
 * \param cref              [in] Name of the model instance
 * \param masterAlgorithm   [in] Master algorithm.
 * \return                  Error status
 */
oms_status_enu_t oms2_setMasterAlgorithm(const char* cref, const char* masterAlgorithm);

/**
 * \brief Export the composite structure of a given model to a dot file.
 *
 * \param cref       [in] Name of the model instance
 * \param filename   [in] Path to the dot file; An exisiting file will be overwritten
 * \return           Error status
 */
oms_status_enu_t oms2_exportCompositeStructure(const char* cref, const char* filename);

/**
 * \brief Export the dependency graphs of a given model to a dot file.
 *
 * \param cref             [in] Name of the model instance
 * \param initialization   [in] Path to the dot file; An exisiting file will be overwritten
 * \param simulation       [in] Path to the dot file; An exisiting file will be overwritten
 * \return                 Error status
 */
oms_status_enu_t oms2_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);

#ifdef __cplusplus
}
#endif

#endif
