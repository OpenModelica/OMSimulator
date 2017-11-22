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
  strftime(buffer, 64, "%a %b %d %T %Y", timeinfo);
  return std::string(buffer);
}

Log::Log() : filename(""), useStdStream(true), initialized(false)
{
}

void Log::initialize()
{
  {
    std::lock_guard<std::mutex> lock(m);
    if (initialized) {
      return;
    }

    numWarnings = 0;
    numErrors = 0;
    initialized = true;

    if (!useStdStream)
      logFile.open(filename.c_str());
  }

  Info("Initializing logging (" + std::string(oms_git_version) + ")");
}

Log::~Log()
{
  terminate();
}

void Log::terminate()
{
  if (numWarnings + numErrors > 0)
  {
    Info(to_string(numWarnings) + " warnings");
    Info(to_string(numErrors) + " errors");

    if (!useStdStream)
    {
      cout << "info:    " << numWarnings << " warnings" << endl;
      cout << "info:    " << numErrors << " errors" << endl;
    }
  }

  Info("Logging completed properly");

  std::lock_guard<std::mutex> lock(m);

  if (!useStdStream)
  {
    // logFile.close();
    cout << "info:    " << "Logging information has been saved to \"" << filename.c_str() << "\"" << endl;
  }

  // initialized = false;
}

Log& Log::getInstance()
{
  // The only instance
  static Log instance;
  return instance;
}

void Log::Info(const std::string& msg)
{
  if (!initialized)
    initialize();

  std::lock_guard<std::mutex> lock(m);

  if (useStdStream)
    cout << "info:    " << msg << endl;
  else
    logFile << TimeStr() << " | info:    " << msg << endl;
}

void Log::Debug(const std::string& msg)
{
  if (!initialized)
    initialize();

  std::lock_guard<std::mutex> lock(m);

  if (useStdStream)
    cout << "debug:   " << msg << endl;
  else
    logFile << TimeStr() << " | debug:   " << msg << endl;
}

void Log::Warning(const std::string& msg)
{
  if (!initialized)
    initialize();

  std::lock_guard<std::mutex> lock(m);

  numWarnings++;
  if (useStdStream)
    cout << "warning: " << msg << endl;
  else
    logFile << TimeStr() << " | warning: " << msg << endl;
}

void Log::Error(const std::string& msg)
{
  if (!initialized)
    initialize();

  std::lock_guard<std::mutex> lock(m);

  numErrors++;
  if (useStdStream)
    cerr << "error:   " << msg << endl;
  else
    logFile << TimeStr() << " | error:   " << msg << endl;
}

void Log::Fatal(const std::string& msg)
{
  if (!initialized)
    initialize();

  {
    std::lock_guard<std::mutex> lock(m);

    numErrors++;
    if (useStdStream)
      cerr << "fatal:   " << msg << endl;
    else
      logFile << TimeStr() << " | fatal:   " << msg << endl;
  }
  // Triggers the mutex again...
  exit(1);
}

void Log::Trace(const std::string& function, const std::string& file, const long line)
{
  if (!initialized)
    initialize();

  std::lock_guard<std::mutex> lock(m);

  if (useStdStream)
    cout << "trace:   " << function << " (" << file << ":" << line << ")" << endl;
  else
    logFile << TimeStr() << " | trace:   " << function << " (" << file << ":" << line << ")" << endl;
}

void Log::setLogFile(const std::string& filename)
{
  std::lock_guard<std::mutex> lock(m);

  if (initialized)
  {
    Error("Log::setLogFile can only be used before the logging is initialized.");
    return;
  }

  if (filename.empty())
  {
    useStdStream = true;
    this->filename = "";
  }
  else
  {
    useStdStream = false;
    this->filename = filename;
  }
}
