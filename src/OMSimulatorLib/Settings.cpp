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

#include "Settings.h"
#include "oms2/Logging.h"

#include <cstring>
#include <string>

Settings::Settings()
{
  startTime = 0.0;
  stopTime = 1.0;
  tolerance = 1e-4;
  communicationInterval = 1e-1;
  resultFile = NULL;
}

Settings::~Settings()
{
  ClearResultFile();
}

void Settings::SetStartTime(double startTime)
{
  this->startTime = startTime;
}

void Settings::SetStopTime(double stopTime)
{
  this->stopTime = stopTime;
}

void Settings::SetTolerance(double tolerance)
{
  this->tolerance = tolerance;
}

void Settings::SetCommunicationInterval(double communicationInterval)
{
  this->communicationInterval = communicationInterval;
}

void Settings::SetResultFile(const char* resultFile)
{
  ClearResultFile();
  this->resultFile = new char[std::strlen(resultFile) + 1];
  std::strcpy(this->resultFile, resultFile);
}

void Settings::ClearResultFile()
{
  if (resultFile)
  {
    delete[] resultFile;
    resultFile = NULL;
  }
}
