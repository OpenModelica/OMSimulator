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

  static oms_status_enu_t setLogFile(const std::string& filename);
  static void setMaxLogFileSize(const unsigned long size) {getInstance().limit=1024*1024*size;}

  static void setLoggingCallback(void (*cb)(oms_message_type_enu_t type, const char* message)) {getInstance().cb = cb;}
  static void setLoggingLevel(int logLevel);

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
#define logError_OnlyForModel                      logError("Only implemented for model identifiers")
#define logError_ModelNotInScope(cref)             logError("Model \"" + std::string(cref) + "\" does not exist in the scope")
#define logError_NotImplemented                    logError("Not implemented")
#define logError_SystemNotInModel(model, system)   logError("Model \"" + std::string(model) + "\" does not contain system \"" + std::string(system) + "\"")

#endif
