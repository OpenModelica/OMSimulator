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

#ifndef _OMS2_LOGGING_H_
#define _OMS2_LOGGING_H_

#include "Types.h"

#include <string>
#include <fstream>
#include <mutex>

#define OMS_DEBUG_LOGGING

#ifndef __FUNCTION_NAME__
  #ifdef WIN32   //WINDOWS
    #define __FUNCTION_NAME__ __FUNCTION__
  #else          //*NIX
    #define __FUNCTION_NAME__ __func__
  #endif
#endif

class Log
{
public:
  static void Info(const std::string& msg);
  static oms_status_enu_t Warning(const std::string& msg);
  static oms_status_enu_t Error(const std::string& msg, const std::string& function);
  static void Debug(const std::string& msg);
  static void Trace(const std::string& function, const std::string& file, const long line);

  static void ProgressBar(double start, double stop, double value);
  static void TerminateBar();

  static oms_status_enu_t setLogFile(const std::string& filename);
  static void setMaxLogFileSize(const unsigned long size) {getInstance().limit=1024*1024*size;}

  static void setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message)) {getInstance().cb = cb;}
  static oms_status_enu_t setLoggingLevel(int logLevel);

private:
  Log();
  ~Log();

  static Log& getInstance();
  void printStringToStream(std::ostream& stream, const std::string& type, const std::string& msg);

  // stop the compiler generating methods copying the object
  Log(Log const& copy);            ///< not implemented
  Log& operator=(Log const& copy); ///< not implemented

private:
  int logLevel;
  std::string filename;
  std::ofstream logFile;
  std::mutex m;
  unsigned int numWarnings;
  unsigned int numErrors;
  unsigned int numMessages;

  unsigned long limit = 1024*1024*50;
  unsigned long size = 0;

  bool progress = false;
  int percent;

  void (*cb)(oms_message_type_enu_t type, const char* message);
};

#define logInfo(msg)    Log::Info(msg)
#define logWarning(msg) Log::Warning(msg)
#define logError(msg)   Log::Error(msg, __func__)

#ifdef OMS_DEBUG_LOGGING
  #define logDebug(msg) Log::Debug(msg)
  #define logTrace()    Log::Trace(__FUNCTION_NAME__, __FILE__, __LINE__)
#else
  #define logDebug(msg) ((void)0)
  #define logTrace()    ((void)0)
#endif

// Common error messages
#define logError_AlreadyInScope(cref)                        logError("\"" + std::string(cref) + "\" already exists in the scope")
#define logError_ComponentNotInSystem(system, component)     logError("System \"" + std::string(system->getFullCref()) + "\" does not contain component \"" + std::string(component) + "\"")
#define logError_ConnectionExistsAlready(crefA, crefB, system) logError("Connection <\"" + std::string(crefA) + "\", \"" + std::string(crefB) + "\"> exists already in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_ConnectionNotInSystem(crefA, crefB, system) logError("Connection <\"" + std::string(crefA) + "\", \"" + std::string(crefB) + "\"> not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_ConnectorNotInSystem(cref, system)          logError("Connector \"" + std::string(cref) + "\" not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_FMUCall(call, fmu)                          logError(std::string(call) + " failed for FMU \"" + std::string(fmu->getFullCref()) + "\"")
#define logError_Initialization(system)                      logError("Initialization of system \"" + std::string(system) + "\" failed")
#define logError_InternalError                               logError("internal error")
#define logError_InvalidIdent(cref)                          logError("\"" + std::string(cref) + "\" is not a valid ident")
#define logError_ModelInWrongState(model)                    logError("Model \"" + std::string(model->getCref()) + "\" is in wrong model state")
#define logError_ModelNotInScope(cref)                       logError("Model \"" + std::string(cref) + "\" does not exist in the scope")
#define logError_NotForScSystem                              logError("Not available for strongly coupled systems")
#define logError_NotForTlmSystem                             logError("Not available for TLM systems")
#define logError_NotImplemented                              logError("Not implemented")
#define logError_OnlyForExternalModels                       logError("Only available for TLM sub models (aka external models)")
#define logError_OnlyForModel                                logError("Only implemented for model identifiers")
#define logError_OnlyForTlmSystem                            logError("Only available for TLM systems")
#define logError_SubSystemNotInSystem(system, subsystem)     logError("System \"" + std::string(system) + "\" does not contain subsystem \"" + std::string(subsystem) + "\"")
#define logError_SystemNotInModel(model, system)             logError("Model \"" + std::string(model) + "\" does not contain system \"" + std::string(system) + "\"")
#define logError_Termination(system)                         logError("Termination of system \"" + std::string(system) + "\" failed")
#define logError_ResetFailed(system)                         logError("failed to reset system \"" + std::string(system) + "\" to instantiation mode")
#define logError_WrongSchema(name)                           logError("Wrong xml schema detected. Unexpected tag \"" + name + "\"")
#define logError_InvalidIdent(cref)                          logError("\"" + std::string(cref) + "\" is not a valid ident")
#define logError_AlreadyInScope(cref)                        logError("\"" + std::string(cref) + "\" already exists in the scope")
#define logError_BusAndConnectorNotSameModel(bus, connector) logError("Bus \"" + std::string(bus) + "\" and connector \"" + std::string(connector) + "\" do not belong to same model")
#define logError_BusAndConnectorNotSameSystem(bus, connector) logError("Bus \"" + std::string(bus) + "\" and connector \"" + std::string(connector) + "\" do not belong to same system")
#define logError_NotForExternalModels                        logError("Not available for external models")
#define logError_ConnectorNotInComponent(cref, component)    logError("Connector \"" + std::string(cref) + "\" not found in component \"" + std::string(component->getFullCref()) + "\"")
#define logError_NoConnectorsInTLMBus(cref)                  logError("No connectors in TLM bus: \"" + std::string(cref) + "\"")
#define logError_BusNotInSystem(cref, system)                logError("Bus connector \"" + std::string(cref) + "\" not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_TlmBusNotInSystem(cref, system)             logError("TLM bus connector \"" + std::string(cref) + "\" not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_BusNotInComponent(cref, component)          logError("Bus connector \"" + std::string(cref) + "\" not found in component \"" + std::string(component->getFullCref()) + "\"")
#define logError_TlmBusNotInComponent(cref, component)       logError("TLM bus connector \"" + std::string(cref) + "\" not found in component \"" + std::string(component->getFullCref()) + "\"")
#define logError_UnknownTLMVariableType(vartype)             logError("Unknown TLM variable type: \""+vartype+"\"")
#define logError_VariableTypeAlreadyInTLMBus(cref,vartype)   logError("TLM bus connector \"" + std::string(cref) + "\" already contains a variable with type \"" + vartype + "\"")
#endif
