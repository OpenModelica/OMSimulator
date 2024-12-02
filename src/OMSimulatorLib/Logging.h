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

#pragma once

#include "OMSimulator/Types.h"

#include <string>

#ifndef __FUNCTION_NAME__
  #ifdef WIN32   //WINDOWS
    #define __FUNCTION_NAME__ __FUNCTION__
  #else          //*NIX
    #define __FUNCTION_NAME__ __func__
  #endif
#endif

namespace oms
{
  namespace Log
  {
    void Info(const std::string &msg);
    oms_status_enu_t Warning(const std::string &msg);
    oms_status_enu_t Error(const std::string &msg, const std::string &function);

    bool DebugEnabled();
    bool TraceEnabled();

    void Debug(const std::string &msg);
    void Trace(const std::string &function, const std::string &file, const long line);

    void ProgressBar(double start, double stop, double value);
    void TerminateBar();

    void SetCallback(void (*cb)(oms_message_type_enu_t type, const char *message));
    oms_status_enu_t SetLogFile(const std::string &filename);

    oms_status_enu_t SetLoggingLevel(int logLevel);
    const int GetLoggingLevel();

    void SetMaxLogFileSize(const unsigned long size);
  }
}

#define logInfo(msg) oms::Log::Info(msg)
#define logWarning(msg) oms::Log::Warning(msg)
#define logError(msg) oms::Log::Error(msg, __func__)

#if !defined(NDEBUG)
  #define logDebugEnabled() oms::Log::DebugEnabled()
  #define logTraceEnabled() oms::Log::TraceEnabled()

  #define logDebug(msg) oms::Log::Debug(msg)
  #define logTrace() oms::Log::Trace(__FUNCTION_NAME__, __FILE__, __LINE__)
#else
  #define logDebugEnabled() (0)
  #define logTraceEnabled() (0)

  #define logDebug(msg) ((void)0)
  #define logTrace() ((void)0)
#endif

// common error messages
#define logError_AlreadyInScope(cref)                          logError("\"" + std::string(cref) + "\" already exists in the scope")
#define logError_BusAndConnectorNotSameModel(bus, connector)   logError("Bus \"" + std::string(bus) + "\" and connector \"" + std::string(connector) + "\" do not belong to same model")
#define logError_BusAndConnectorNotSameSystem(bus, connector)  logError("Bus \"" + std::string(bus) + "\" and connector \"" + std::string(connector) + "\" do not belong to same system")
#define logError_BusNotInComponent(cref, component)            logError("Bus connector \"" + std::string(cref) + "\" not found in component \"" + std::string(component->getFullCref()) + "\"")
#define logError_BusNotInSystem(cref, system)                  logError("Bus connector \"" + std::string(cref) + "\" not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_ComponentNotInSystem(system, component)       logError("System \"" + std::string(system->getFullCref()) + "\" does not contain component \"" + std::string(component) + "\"")
#define logError_ConnectionExistsAlready(crefA, crefB, system) logError("Connection <\"" + std::string(crefA) + "\", \"" + std::string(crefB) + "\"> exists already in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_ConnectionNotInSystem(crefA, crefB, system)   logError("Connection <\"" + std::string(crefA) + "\", \"" + std::string(crefB) + "\"> not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_ConnectorNotInComponent(cref, component)      logError("Connector \"" + std::string(cref) + "\" not found in component \"" + std::string(component->getFullCref()) + "\"")
#define logError_ConnectorNotInSystem(cref, system)            logError("Connector \"" + std::string(cref) + "\" not found in system \"" + std::string(system->getFullCref()) + "\"")
#define logError_FMUCall(call, fmu)                            logError(std::string(call) + " failed for FMU \"" + std::string(fmu->getFullCref()) + "\"")
#define logError_Initialization(system)                        logError("Initialization of system \"" + std::string(system) + "\" failed")
#define logError_InternalError                                 logError("internal error")
#define logError_InvalidIdent(cref)                            logError("\"" + std::string(cref) + "\" is not a valid ident")
#define logError_InvalidIdent(cref)                            logError("\"" + std::string(cref) + "\" is not a valid ident")
#define logError_ModelInWrongState(cref)                       logError("Model \"" + std::string(cref) + "\" is in wrong model state")
#define logError_ModelNotInScope(cref)                         logError("Model \"" + std::string(cref) + "\" does not exist in the scope")
#define logError_NotForScSystem                                logError("Not available for strongly coupled systems")
#define logError_NotImplemented                                logError("Not implemented")
#define logError_OnlyForModel                                  logError("Only implemented for model identifiers")
#define logError_OnlyForRealInputs(cref)                       logError("Signal \"" + std::string(cref) + "\" is not a real input signal")
#define logError_OnlyForSystemWC                               logError("Only available for WC systems")
#define logError_ResetFailed(system)                           logError("failed to reset system \"" + std::string(system) + "\" to instantiation mode")
#define logError_SubSystemNotInSystem(system, subsystem)       logError("System \"" + std::string(system) + "\" does not contain subsystem \"" + std::string(subsystem) + "\"")
#define logError_SystemNotInModel(model, system)               logError("Model \"" + std::string(model) + "\" does not contain system \"" + std::string(system) + "\"")
#define logError_Termination(system)                           logError("Termination of system \"" + std::string(system) + "\" failed")
#define logError_UnknownSignal(cref)                           logError("Unknown signal \"" + std::string(cref) + "\"")
#define logError_WrongSchema(name)                             logError("Wrong xml schema detected. Unexpected tag \"" + name + "\"")
#define logWarning_deprecated                                  logWarning("Wrong/deprecated content detected but successfully loaded. Please re-export the SSP file to avoid this message.")
