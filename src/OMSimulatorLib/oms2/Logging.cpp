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

#include "Logging.h"
#include "../Version.h"
#include "../Types.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

using namespace std;

std::string TimeStr()
{
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[64];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 64, "%a %b %d %H:%M:%S %Y", timeinfo);
  return std::string(buffer);
}

Log::Log() : filename(""), cb(NULL)
{
  numWarnings = 0;
  numErrors = 0;
  logLevel = false;
}

Log::~Log()
{
  // close log file
  setLogFile("");
}

Log& Log::getInstance()
{
  // The only instance
  static Log instance;
  return instance;
}

void Log::Info(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (!log.logFile.is_open())
    cout << "info:    " << msg << endl;
  else
    log.logFile << TimeStr() << " | info:    " << msg << endl;

  if (log.cb)
    log.cb(oms_message_info, msg.c_str());
}

void Log::Warning(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  log.numWarnings++;
  if (!log.logFile.is_open())
    cout << "warning: " << msg << endl;
  else
    log.logFile << TimeStr() << " | warning: " << msg << endl;

  if (log.cb)
    log.cb(oms_message_warning, msg.c_str());
}

void Log::Error(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  log.numErrors++;
  if (!log.logFile.is_open())
    cerr << "error:   " << msg << endl;
  else
    log.logFile << TimeStr() << " | error:   " << msg << endl;

  if (log.cb)
    log.cb(oms_message_error, msg.c_str());
}

void Log::Debug(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logLevel < 1)
    return;

  if (!log.logFile.is_open())
    cout << "debug:   " << msg << endl;
  else
    log.logFile << TimeStr() << " | debug:   " << msg << endl;

  if (log.cb)
    log.cb(oms_message_debug, msg.c_str());
}

void Log::Trace(const std::string& function, const std::string& file, const long line)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logLevel < 2)
    return;

  std::string msg = function + " (" + file + ":" + std::to_string(line) + ")";

  if (!log.logFile.is_open())
    cout << "trace:   " << msg << endl;
  else
    log.logFile << TimeStr() << " | trace:   " << msg << endl;

  if (log.cb)
    log.cb(oms_message_trace, msg.c_str());
}

oms_status_t Log::setLogFile(const std::string& filename)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logFile.is_open())
  {
    log.logFile << TimeStr() << " | info:    " << "Logging completed properly" << endl;
    log.logFile.close();

    if (log.numWarnings + log.numErrors > 0)
    {
      cout << "info:    " << log.numWarnings << " warnings" << endl;
      cout << "info:    " << log.numErrors << " errors" << endl;
    }
    cout << "info:    " << "Logging information has been saved to \"" << log.filename.c_str() << "\"" << endl;
  }

  log.numWarnings = 0;
  log.numErrors = 0;
  log.filename = filename;

  if (!filename.empty())
  {
    log.logFile.open(filename.c_str());
    if (log.logFile.is_open())
      log.logFile << TimeStr() << " | info:    " << "Initializing logging (" << std::string(oms_git_version) << ")" << endl;
    else
    {
      log.filename = "";
      return oms_status_error;
    }
  }

  return oms_status_ok;
}

void Log::setLoggingLevel(int logLevel)
{
#ifdef OMS_DEBUG_LOGGING
  Log& log = getInstance();
  log.logLevel = logLevel;
#else
  Warning("Log::setDebugLogging is not available.")
#endif
}
