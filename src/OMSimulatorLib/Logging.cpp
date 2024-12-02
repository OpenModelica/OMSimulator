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

#include "Logging.h"
#include "Version.h"
#include "OMSimulator/Types.h"

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <string>

class Logging
{
public:
  Logging() : filename(""), cb(NULL) {}
  ~Logging()
  {
    // close _log file
    oms::Log::SetLogFile("");
  }

private:
  // stop the compiler generating methods copying the object
  Logging(Logging const &copy);            ///< not implemented
  Logging &operator=(Logging const &copy); ///< not implemented

public:
  int logLevel;
  std::string filename;
  std::ofstream logFile;

  unsigned int numWarnings = 0;
  unsigned int numErrors = 0;
  unsigned int numMessages = 0;

  unsigned long limit = 1024 * 1024 * 50;
  unsigned long size = 0;

  bool progress = false;
  int percent;

  void (*cb)(oms_message_type_enu_t type, const char *message);

  std::mutex m;
};

Logging _log;

static std::string TimeStr()
{
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[64];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 64, "%a %b %d %H:%M:%S %Y", timeinfo);
  return std::string(buffer);
}

static void PrintStringToStream(std::ostream& stream, const std::string& type, const std::string& msg)
{
  oms::Log::TerminateBar();

  std::string timeStamp, padding;
  if (_log.logFile.is_open())
  {
    timeStamp = TimeStr();
    padding = std::string(timeStamp.size(), ' ');
    _log.logFile << timeStamp << " | ";
  }

  stream << type << ": " << std::string(7 - type.size(), ' ');
  size_t start = 0, end = 0;
  bool firstLine = true;
  std::string buffer;
  unsigned int nLines = 1;
  while(end < msg.size())
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
  stream << std::endl;

  if (_log.logFile.is_open())
  {
    _log.size += msg.length() + nLines*(12+timeStamp.length());

    if (_log.size > _log.limit)
    {
      _log.numWarnings++;
      stream << timeStamp << " | warning: Log file becomes too big; switching to stdout" << std::endl;
      std::cout << "info:    Partial logging information has been saved to \"" + _log.filename + "\"" << std::endl;
      _log.logFile.close();
      _log.filename = "";
      _log.size = 0;
    }
  }
}

void oms::Log::Info(const std::string &msg)
{
  std::lock_guard<std::mutex> lock(_log.m);

  _log.numMessages++;
  std::ostream &stream = _log.logFile.is_open() ? _log.logFile : std::cout;
  PrintStringToStream(stream, "info", msg);

  if (_log.cb)
    _log.cb(oms_message_info, msg.c_str());
}

oms_status_enu_t oms::Log::Warning(const std::string &msg)
{
  std::lock_guard<std::mutex> lock(_log.m);

  _log.numWarnings++;
  _log.numMessages++;
  std::ostream &stream = _log.logFile.is_open() ? _log.logFile : std::cout;
  PrintStringToStream(stream, "warning", msg);

  if (_log.cb)
    _log.cb(oms_message_warning, msg.c_str());

  return oms_status_warning;
}

oms_status_enu_t oms::Log::Error(const std::string &msg, const std::string &function)
{
  std::lock_guard<std::mutex> lock(_log.m);

  _log.numErrors++;
  _log.numMessages++;
  std::ostream &stream = _log.logFile.is_open() ? _log.logFile : std::cerr;
  std::string fullMessage = "[" + function + "] " + msg;
  PrintStringToStream(stream, "error", fullMessage);

  if (_log.cb)
    _log.cb(oms_message_error, fullMessage.c_str());

  return oms_status_error;
}

bool oms::Log::DebugEnabled()
{
  return _log.logLevel >= 1;
}

void oms::Log::Debug(const std::string &msg)
{
  std::lock_guard<std::mutex> lock(_log.m);

  if (_log.logLevel < 1)
    return;

  _log.numMessages++;
  std::ostream &stream = _log.logFile.is_open() ? _log.logFile : std::cout;
  PrintStringToStream(stream, "debug", msg);

  if (_log.cb)
    _log.cb(oms_message_debug, msg.c_str());
}

bool oms::Log::TraceEnabled()
{
  return _log.logLevel >= 2;
}

void oms::Log::Trace(const std::string &function, const std::string &file, const long line)
{
  std::lock_guard<std::mutex> lock(_log.m);

  if (_log.logLevel < 2)
    return;

  _log.numMessages++;
  std::string msg = function + " (" + file + ":" + std::to_string(line) + ")";

  std::ostream &stream = _log.logFile.is_open() ? _log.logFile : std::cout;
  PrintStringToStream(stream, "trace", msg);

  if (_log.cb)
    _log.cb(oms_message_trace, msg.c_str());
}

oms_status_enu_t oms::Log::SetLogFile(const std::string &filename)
{
  std::lock_guard<std::mutex> lock(_log.m);

  if (_log.logFile.is_open())
  {
    PrintStringToStream(_log.logFile, "info", "Logging completed properly");
    _log.logFile.close();
    PrintStringToStream(std::cout, "info", "Logging information has been saved to \"" + _log.filename + "\"");
  }

  if (_log.numWarnings + _log.numErrors > 0)
  {
    PrintStringToStream(std::cout, "info", std::to_string(_log.numWarnings) + " warnings");
    PrintStringToStream(std::cout, "info", std::to_string(_log.numErrors) + " errors");
  }

  _log.numWarnings = 0;
  _log.numErrors = 0;
  _log.numMessages = 0;
  _log.filename = filename;
  _log.size = 0;

  if (!filename.empty())
  {
    _log.logFile.open(filename.c_str());
    if (_log.logFile.is_open())
      PrintStringToStream(_log.logFile, "info", "Initializing logging (" + std::string(oms_git_version) + ")");
    else
    {
      _log.filename = "";
      return oms_status_error;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Log::SetLoggingLevel(int logLevel)
{
  if (logLevel < 0 || logLevel > 2)
    return logError("Invalid logging level");

  _log.logLevel = logLevel;

#if defined(NDEBUG)
  if (logLevel > 1)
    Warning("debug logging is not available");
#endif

  return oms_status_ok;
}

const int oms::Log::GetLoggingLevel()
{
  return _log.logLevel;
}

void oms::Log::SetMaxLogFileSize(const unsigned long size)
{
  _log.limit = 1024 * 1024 * size;
}

void oms::Log::SetCallback(void (*cb)(oms_message_type_enu_t type, const char *message))
{
  _log.cb = cb;
}

void oms::Log::ProgressBar(double start, double stop, double value)
{
  if (_log.progress)
    printf("\r");
  else
    _log.percent = -1;

  const char *label = "info:    ";

  int width = 72 - strlen(label);
  int pos = ((value - start) * width) / (stop - start);
  int percent = ((value - start) * 100) / (stop - start);

  if (_log.percent == percent)
    return;
  _log.percent = percent;

  printf("%s[", label);

  // fill progress bar with =
  for (int i = 0; i < pos; i++)
    printf("%c", '=');

  // fill progress bar with spaces
  printf("%*c %3d%%", width - pos + 1, ']', percent);
  _log.progress = true;
}

void oms::Log::TerminateBar()
{
  if (_log.progress)
  {
    printf("\n");
    _log.progress = false;
  }
}
