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

#ifndef _OMS_LOGGING_H_
#define _OMS_LOGGING_H_

#include <string>
#include <fstream>
#include <mutex>

//#define OMS_DEBUG_LOGGING

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
  static Log& getInstance();

  void Info(const std::string& msg);
  void Debug(const std::string& msg);
  void Warning(const std::string& msg);
  void Error(const std::string& msg);
  [[noreturn]] void Fatal(const std::string& msg);
  void Trace(const std::string& function, const std::string& file, const long line);

  void setLogFile(const std::string& filename);

private:
  Log();
  ~Log();

  void initialize();
  void terminate();

  // Stop the compiler generating methods of copy the object
  Log(Log const& copy);            // Not Implemented
  Log& operator=(Log const& copy); // Not Implemented

  bool initialized;
  bool useStdStream;
  std::string filename;
  std::ofstream logFile;
  std::mutex m;
  unsigned int numWarnings;
  unsigned int numErrors;
};

#define logInfo(msg)    Log::getInstance().Info(msg)
#define logWarning(msg) Log::getInstance().Warning(msg)
#define logError(msg)   Log::getInstance().Error(msg)
#define logFatal(msg)   Log::getInstance().Fatal(msg)

#ifdef OMS_DEBUG_LOGGING
  #define logDebug(msg) Log::getInstance().Debug(msg)
  #define logTrace()    Log::getInstance().Trace(__FUNCTION_NAME__, __FILE__, __LINE__)
#else
  #define logDebug(msg) ((void)0)
  #define logTrace()    ((void)0)
#endif

#endif
