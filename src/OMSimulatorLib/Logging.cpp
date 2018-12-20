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
#include "Version.h"
#include "Types.h"

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdlib.h>

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
  numMessages = 0;
  logLevel = 0;
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

void Log::printStringToStream(std::ostream& stream, const std::string& type, const std::string& msg)
{
  TerminateBar();

  std::string timeStamp, padding;
  if (logFile.is_open())
  {
    timeStamp = TimeStr();
    padding = std::string(timeStamp.size(), ' ');
    logFile << timeStamp << " | ";
  }

  stream << type << ": " << std::string(7 - type.size(), ' ');
  size_t start = 0, end = 0;
  bool firstLine = true;
  std::string buffer;
  unsigned int nLines = 1;
  while(msg[end])
  {
    if (msg[end] == '\n' || msg[end] == '\r')
    {
      buffer = msg.substr(start, end-start);
      end++;
      start = end;
      nLines++;
    }
    else if (msg[end+1] == '\0')
    {
      buffer = msg.substr(start);
    }
    end++;

    if (!buffer.empty())
    {
      if (!firstLine)
      {
        stream << "\n";
        if (!timeStamp.empty())
          stream << padding << "   ";
        stream << "         ";
      }
      firstLine = false;
      stream << buffer;
      buffer.clear();
    }
  }
  stream << endl;

  if (logFile.is_open())
  {
    size += msg.length() + nLines*(12+timeStamp.length());

    if (size > limit)
    {
      numWarnings++;
      stream << timeStamp << " | warning: Log file becomes too big; switching to stdout" << endl;
      cout << "info:    Partial logging information has been saved to \"" + filename + "\"" << endl;
      logFile.close();
      filename = "";
      size = 0;
    }
  }
}

void Log::Info(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  log.numMessages++;
  std::ostream& stream = log.logFile.is_open() ? log.logFile : cout;
  log.printStringToStream(stream, "info", msg);

  if (log.cb)
    log.cb(oms_message_info, msg.c_str());
}

oms_status_enu_t Log::Warning(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  log.numWarnings++;
  log.numMessages++;
  std::ostream& stream = log.logFile.is_open() ? log.logFile : cout;
  log.printStringToStream(stream, "warning", msg);

  if (log.cb)
    log.cb(oms_message_warning, msg.c_str());

  return oms_status_warning;
}

oms_status_enu_t Log::Error(const std::string& msg, const std::string& function)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  log.numErrors++;
  log.numMessages++;
  std::ostream& stream = log.logFile.is_open() ? log.logFile : cerr;
  std::string fullMessage = "[" + function + "] " + msg;
  log.printStringToStream(stream, "error", fullMessage);

  if (log.cb)
    log.cb(oms_message_error, fullMessage.c_str());

  return oms_status_error;
}

void Log::Debug(const std::string& msg)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logLevel < 1)
    return;

  log.numMessages++;
  std::ostream& stream = log.logFile.is_open() ? log.logFile : cout;
  log.printStringToStream(stream, "debug", msg);

  if (log.cb)
    log.cb(oms_message_debug, msg.c_str());
}

void Log::Trace(const std::string& function, const std::string& file, const long line)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logLevel < 2)
    return;

  log.numMessages++;
  std::string msg = function + " (" + file + ":" + std::to_string(line) + ")";

  std::ostream& stream = log.logFile.is_open() ? log.logFile : cout;
  log.printStringToStream(stream, "trace", msg);

  if (log.cb)
    log.cb(oms_message_trace, msg.c_str());
}

oms_status_enu_t Log::setLogFile(const std::string& filename)
{
  Log& log = getInstance();
  std::lock_guard<std::mutex> lock(log.m);

  if (log.logFile.is_open())
  {
    log.printStringToStream(log.logFile, "info", "Logging completed properly");
    log.logFile.close();
    log.printStringToStream(cout, "info", "Logging information has been saved to \"" + log.filename + "\"");
  }

  if (log.numWarnings + log.numErrors > 0)
  {
    log.printStringToStream(cout, "info", std::to_string(log.numWarnings) + " warnings");
    log.printStringToStream(cout, "info", std::to_string(log.numErrors) + " errors");
  }

  log.numWarnings = 0;
  log.numErrors = 0;
  log.numMessages = 0;
  log.filename = filename;
  log.size = 0;

  if (!filename.empty())
  {
    log.logFile.open(filename.c_str());
    if (log.logFile.is_open())
      log.printStringToStream(log.logFile, "info", "Initializing logging (" + std::string(oms_git_version) + ")");
    else
    {
      log.filename = "";
      return oms_status_error;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t Log::setLoggingLevel(int logLevel)
{
  if (logLevel < 0 || logLevel > 2)
    return logError("Invalid logging level");

  Log& log = getInstance();
  log.logLevel = logLevel;

#ifndef OMS_DEBUG_LOGGING
  if (logLevel > 1)
    Warning("debug logging is not available");
#endif

  return oms_status_ok;
}

const int Log::getLoggingLevel()
{
  Log& log = getInstance();
  return log.logLevel;
}

void Log::ProgressBar(double start, double stop, double value)
{
  Log& log = getInstance();

  if (log.progress)
    printf("\r");
  else
    log.percent = -1;

  const char* label = "info:    ";

  int width = 72 - strlen(label);
  int pos = ((value - start) * width) / (stop - start) ;
  int percent = ((value - start) * 100) / (stop - start);

  if (log.percent == percent)
    return;
  log.percent = percent;

  printf("%s[", label);

  //fill progress bar with =
  for (int i = 0; i<pos; i++)
    printf("%c", '=');

  //fill progress bar with spaces
  printf("% *s", width - pos + 1, "]");
  printf(" %3d%%", percent);
  log.progress = true;
}

void Log::TerminateBar()
{
  Log& log = getInstance();

  if (log.progress)
  {
    printf("\n");
    log.progress = false;
  }
}
