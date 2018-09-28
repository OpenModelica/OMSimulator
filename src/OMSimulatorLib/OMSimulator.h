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

/* ************************************ */
/* oms3                                 */
/*                                      */
/* Experimental API                     */
/* ************************************ */

const char* oms3_getVersion();
oms_status_enu_t oms3_setLogFile(const char* filename);
void oms3_setMaxLogFileSize(const unsigned long size);
oms_status_enu_t oms3_setTempDirectory(const char* newTempDir);
oms_status_enu_t oms3_setWorkingDirectory(const char* newWorkingDir);
oms_status_enu_t oms3_newModel(const char* cref);
oms_status_enu_t oms3_rename(const char* cref, const char* newCref);
oms_status_enu_t oms3_delete(const char* cref);
oms_status_enu_t oms3_export(const char* cref, const char* filename);
oms_status_enu_t oms3_import(const char* filename, char** cref);
oms_status_enu_t oms3_list(const char* cref, char** contents);
oms_status_enu_t oms3_parseModelName(const char* contents, char** cref);
oms_status_enu_t oms3_importString(const char* contents, char** cref);
oms_status_enu_t oms3_addSystem(const char* cref, oms_system_enu_t type);
oms_status_enu_t oms3_copySystem(const char* source, const char* target);
oms_status_enu_t oms3_getElement(const char* cref, oms3_element_t** element);
oms_status_enu_t oms3_getElements(const char* cref, oms3_element_t*** elements);
oms_status_enu_t oms3_setElementGeometry(const char* cref, const ssd_element_geometry_t* geometry);
oms_status_enu_t oms3_addConnector(const char* cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);
oms_status_enu_t oms3_getConnector(const char* cref, oms_connector_t** connector);
oms_status_enu_t oms3_setCommandLineOption(const char* cmd);
oms_status_enu_t oms3_getSystemType(const char* cref, oms_system_enu_t* type);
oms_status_enu_t oms3_setConnectorGeometry(const char* cref, const ssd_connector_geometry_t* geometry);
oms_status_enu_t oms3_setConnectionGeometry(const char* crefA, const char* crefB, const ssd_connection_geometry_t* geometry);
oms_status_enu_t oms3_getConnections(const char* cref, oms3_connection_t*** connections);
oms_status_enu_t oms3_addConnection(const char* crefA, const char* crefB);
oms_status_enu_t oms3_addBus(const char* cref);
oms_status_enu_t oms3_getBus(const char* cref, oms3_busconnector_t** busConnector);
oms_status_enu_t oms3_addConnectorToBus(const char* busCref, const char* connectorCref);
oms_status_enu_t oms3_setBusGeometry(const char* bus, const ssd_connector_geometry_t* geometry);
oms_status_enu_t oms3_addTLMBus(const char* cref, const char* domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
oms_status_enu_t oms3_getTLMBus(const char* cref, oms3_tlmbusconnector_t** tlmBusConnector);
oms_status_enu_t oms3_addConnectorToTLMBus(const char* busCref, const char* connectorCref, const char *type);
oms_status_enu_t oms3_setTLMBusGeometry(const char* bus, const ssd_connector_geometry_t* geometry);
oms_status_enu_t oms3_addTLMConnection(const char* crefA, const char* crefB, double delay, double alpha, double impedance, double impedancerot);

/* not implemented yet */
oms_status_enu_t oms3_addSubModel(const char* cref, const char* fmuPath);
oms_status_enu_t oms3_addExternalModel(const char* cref, const char* path, const char* startscript);
oms_status_enu_t oms3_setSimulationInformation(const char* cref, ssd_simulation_information_t* info);
oms_status_enu_t oms3_getSimulationInformation(const char* cref, ssd_simulation_information_t** info);
oms_status_enu_t oms3_getSubModelPath(const char* cref, char** path);
oms_status_enu_t oms3_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo);

/* ************************************ */
/* oms2                                 */
/*                                      */
/*                                      */
/* ************************************ */

/**
 * \brief Returns the library's version string.
 *
 * \return version string
 */
const char* oms2_getVersion();

/**
 * \brief Compares a given signal of two result files within absolute and relative tolerances.
 */
int oms2_compareSimulationResults(const char* filenameA, const char* filenameB, const char* var, double relTol, double absTol);

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
 * \brief Loads a FMI composite model from xml file.
 *
 * \param filename   [in] Path to the composite model xml file
 * \param ident      [out] Name of the imported model
 * \return           Error status
 */
oms_status_enu_t oms2_loadModel(const char* filename, char** ident);

/**
 * \brief Parses a composite model from xml representation.
 * Memory is allocated for ident. The caller is responsible to call free on it.
 *
 * \param contents   [in] Composite model xml contents
 * \param ident      [out] Name of the parsed model
 * \return           Error status
 */
oms_status_enu_t oms2_parseString(const char* contents, char** ident);

/**
 * \brief Loads a composite model from xml representation.
 *
 * \param contents   [in] Composite model xml contents
 * \param ident      [out] Name of the imported model
 * \return           Error status
 */
oms_status_enu_t oms2_loadString(const char* contents, char** ident);

/**
 * \brief Loads a FMI composite model from xml representation.
 *
 * \param ident      [in] Name of the model to export
 * \param filename   [in] Path to the xml file; An existing file will be overwritten
 * \return           Error status
 */
oms_status_enu_t oms2_saveModel(const char* ident, const char* filename);

/**
 * \brief Lists the contents of a composite model.
 * Memory is allocated for contents. The caller is responsible to call free on it.
 *
 * \param ident      [in] Name of the model
 * \param contents   [out] Contents of the model
 * \return           Error status
 */
oms_status_enu_t oms2_listModel(const char* ident, char** contents);

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
 * \brief Returns the path of a given component.
 *
 * \param cref         [in] Full identifier of a component
 * \param path         [out] path
 * \return             Error status
 */
oms_status_enu_t oms2_getSubModelPath(const char* cref, char** path);

/**
 * \brief Returns FMU specific information.
 *
 * \param cref      [in] Full identifier of a component
 * \param fmuInfo   [out] FMU path
 * \return          Error status
 */
oms_status_enu_t oms2_getFMUInfo(const char* cref, const oms_fmu_info_t** fmuInfo);

/**
 * \brief Set geometry information to a given connector.
 *
 * \param cref       [in] Full identifier of a connector.
 * \param geometry   [in] Geometry information
 * \return           Error status
 */
oms_status_enu_t oms2_setConnectorGeometry(const char* connector, const ssd_connector_geometry_t* geometry);

/**
 * \brief Get list of all connections from a given component.
 *
 * \param cref          [in] Full identifier of a component
 * \param connections   [out] Array of connections (null-terminated array)
 * \return              Error status
 */
oms_status_enu_t oms2_getConnections(const char* cref, oms_connection_t*** connections);

/**
 * \brief Adds a new connection between connectors A and B to a given parent
 * component.
 * This command can be used to connects submodels with each other or to attach
 * a solver to a submodel.
 *
 * \param cref         [in] Full identifier of a component, e.g. FMI composite model
 * \param conA         [in] Name of connector A
 * \param conB         [in] Name of connector B
 * \return             Error status
 */
oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB);

/**
 * \brief Deletes the connection between connectors A and B.
 * This can also be used to unconnect a solver from a given FMU.
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
 * \param cb               [in] Callback function which is called after every completed step of the composite model
 * \return                 Error status
 */
oms_status_enu_t oms2_simulate_asynchronous(const char* ident, void (*cb)(const char* ident, double time, oms_status_enu_t status));

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
 * \brief Prints a summary of the given composite model to stdout.
 *
 * \param cref   [in] Full identifier of a component.
 * \return       Error status
 */
oms_status_enu_t oms2_describe(const char* cref);

/**
 * \brief Adds an external model to a TLM composite model.
 *
 * \param cref          [in] Identifier of parent TLM composite model.
 * \param name          [in] Identifier of external model
 * \param modelfile     [in] Absolute path to model file of external model.
 * \param startscript   [in] Start script of external model.
 * \return              Error status
 */
oms_status_enu_t oms2_addExternalModel(const char* cref, const char* name, const char* modelfile, const char* startscript);

/**
 * \brief Adds an FMI composite model to a TLM composite model.
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
 * \brief Sets initial value for a TLM interface
 *
 * \param cref  [in] Identifier of TLM composite model.
 * \param from  [in] TLM interface (format: submodel:interface)
 * \param value [in] Initial variable value
 */
oms_status_enu_t oms2_setTLMInitialValues(const char *cref, const char *ifc, const double values[], int nvalues);

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
 * \brief Sets initial position and orientation for a TLM 3D interface
 *
 * \param cref                                [in] Identifier of TLM composite model.
 * \param ifc                                 [in] Identifier of TLM interface (format: submodel:interface)
 * \param x1,x2,x3                            [in] Position vector
 * \param A11,A12,A13,A21,A22,A23,A31,A32,A33 [in] Orientation matrix (3x3)
 * \return             Error status
 */
oms_status_enu_t oms2_setTLMPositionAndOrientation(const char *cref, const char *ifc,
                                                   double x1, double x2, double x3,
                                                   double A11, double A12, double A13,
                                                   double A21, double A22, double A23,
                                                   double A31, double A32, double A33);

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
 * \brief Specifies logging level for OMTLMSimulator
 *
 * \param cref    [in] Identifier of TLM composite model.
 * \param levle   [in] Logging level (0=disabled,1=fatal,2=warning,3=info,4=debug)
 * \return        Error status
 */
oms_status_enu_t oms2_setTLMLoggingLevel(const char* cref, const int level);

/**
 * \brief Set the numer of logging samples to use in the simulation.
 *
 * \param cref              [in] Name of the model instance
 * \param loggingSamples    [in] Logging samples
 * \return                  Error status
 */
oms_status_enu_t oms2_setLoggingSamples(const char* cref, const int loggingSamples);

/**
 * \brief Redirects logging output to file or std streams. The warning/error counters are reset.
 *
 * \param filename   [in] "" to redirect to std streams and proper filename to redirect to file.
 * \return           Error status
 */
oms_status_enu_t oms2_setLogFile(const char* filename);

/**
 * \brief Sets maximum log file size in MB. If the file exceeds this limit, the
 * logging will continue on stdout.
 *
 * \param size   [in] maximum log file size in MB
 * \return       Error status
 */
void oms2_setMaxLogFileSize(const unsigned long size);

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
 * \brief Get the start time from the model.
 *
 * \param cref        [in] Name of the model instance
 * \param startTime   [out] Start time
 * \return            Error status
 */
oms_status_enu_t oms2_getStartTime(const char* cref, double* startTime);

/**
 * \brief Set the start time of the simulation.
 *
 * \param cref        [in] Name of the model instance
 * \param startTime   [in] Start time
 * \return            Error status
 */
oms_status_enu_t oms2_setStartTime(const char* cref, double startTime);

/**
 * \brief Get the stop time from the model.
 *
 * \param cref       [in] Name of the model instance
 * \param stopTime   [out] Stop time
 * \return           Error status
 */
oms_status_enu_t oms2_getStopTime(const char* cref, double* stopTime);

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
 * \brief Set the logging interval of the simulation.
 *
 * \param cref                    [in] Name of the model instance
 * \param loggingInterval         [in] Logging interval
 * \return                        Error status
 */
oms_status_enu_t oms2_setLoggingInterval(const char* cref, double loggingInterval);

/**
 * \brief Set the result file of the simulation.
 *
 * \param cref       [in] Name of the model instance
 * \param filename   [in] Result file
 * \param bufferSize [in] Buffer size to speedup the logging
 * \return           Error status
 */
oms_status_enu_t oms2_setResultFile(const char* cref, const char* filename, int bufferSize);


/**
 * \brief Set master algorithm variant that shall be used (default: "standard").
 *
 * Supported master algorithms: "standard"
 *
 * Experimental master algorithms (no stable API!): "pctpl", "pmrchannela", "pmrchannelcv", "pmrchannelm"
 *
 * \param ident              [in] Name of the model instance
 * \param masterAlgorithm   [in] Master algorithm.
 * \return                  Error status
 */
oms_status_enu_t oms2_setMasterAlgorithm(const char* ident, const char* masterAlgorithm);


/**
 * \brief Experimental feature for setting the activation ratio of FMUs for
 *  experimenting with multi-rate master algorithms.
 *
 * \warning Not part of the stable API, will be changed or removed without notice.
 *
 * \param cref      [in] Full identifier of a component
 * \param k         [in] Activation ratio (k=1 means activate every communicationInterval)
 * \return          Error status
 */
oms_status_enu_t experimental_setActivationRatio(const char* cref, int k);


/**
 * \brief Experimental feature for (soft) real-time simulation.
 *
 * \warning Not part of the stable API, will be changed or removed without notice.
 *
 * \param ident   [in] Name of the model instance
 * \return        Error status
 */
oms_status_enu_t experimental_simulate_realtime(const char* ident);


/**
 * \brief Export the composite structure of a given model to a dot file.
 *
 * \param cref       [in] Name of the model instance
 * \param filename   [in] Path to the dot file; An existing file will be overwritten
 * \return           Error status
 */
oms_status_enu_t oms2_exportCompositeStructure(const char* cref, const char* filename);

/**
 * \brief Export the dependency graphs of a given model to a dot file.
 *
 * \param initialization   [in] Path to the dot file; An existing file will be overwritten
 * \param simulation       [in] Path to the dot file; An existing file will be overwritten
 * \return                 Error status
 */
oms_status_enu_t oms2_exportDependencyGraphs(const char* cref, const char* initialization, const char* simulation);

/**
 * \brief Get current simulation time.
 *
 * \param model [in]  Name of the model instance
 * \param time  [out] Minimal time value that is reached by all instances
 * \return Error status
 */
oms_status_enu_t oms2_getCurrentTime(const char* model, double* time);

/**
 * \brief Add all variables that match the given regex to the result file.
 *
 * \param cref  [in] Name of the model instance
 * \param regex [in] Regular expression
 * \return Error status
 */
oms_status_enu_t oms2_addSignalsToResults(const char* cref, const char* regex);

/**
 * \brief Removes all variables that match the given regex to the result file.
 *
 * \param cref  [in] Name of the model instance
 * \param regex [in] Regular expression
 * \return Error status
 */
oms_status_enu_t oms2_removeSignalsFromResults(const char* cref, const char* regex);

/**
 * \brief Sets special flags.
 *
 * \param cref  [in] Name of the model instance
 * \param flags [in] flags
 * \return Error status
 */
oms_status_enu_t oms2_setFlags(const char* cref, const char* flags);

oms_status_enu_t oms2_addSolver(const char* model, const char* name, const char* solver);

/**
 * \brief Free the memory.
 *
 * \param obj  [in] Pointer to the object.
 * \return Error status
 */
oms_status_enu_t oms2_freeMemory(void* obj);

/**
 * \brief This function returns 1 if a given cref exists in the scope,
 * otherwise 0. It can be used to check for composite models, sub-models such
 * as FMUs, and solver instances.
 */
int oms2_exists(const char* cref);

#ifdef __cplusplus
}
#endif

#endif
