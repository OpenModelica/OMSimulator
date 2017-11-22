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

#include "ResultWriter.h"

ResultWriter::ResultWriter(unsigned int bufferSize)
  : bufferSize(bufferSize),
    nEmits(0),
    data_2(NULL)
{
}

ResultWriter::~ResultWriter()
{
  if (data_2)
    delete[] data_2;
}

unsigned int ResultWriter::addSignal(const std::string& name, const std::string& description, SignalType_t type)
{
  Signal signal;
  signal.name = name;
  signal.description = description;
  signal.type = type;

  signals.push_back(signal);
  return (unsigned int) signals.size();
}

void ResultWriter::addParameter(const std::string& name, const std::string& description, SignalType_t type, SignalValue_t value)
{
  Parameter parameter;
  parameter.signal.name = name;
  parameter.signal.description = description;
  parameter.signal.type = type;
  parameter.value = value;

  parameters.push_back(parameter);
}

bool ResultWriter::create(const std::string& filename, double startTime, double stopTime)
{
  if (!createFile(filename, startTime, stopTime))
    return false;

  data_2 = new double[bufferSize*(signals.size() + 1)];
  nEmits = 0;
  return true;
}

void ResultWriter::close()
{
  closeFile();

  if (data_2)
  {
    delete[] data_2;
    data_2 = NULL;
  }

  signals.clear();
  parameters.clear();
}

void ResultWriter::updateSignal(unsigned int id, SignalValue_t value)
{
  if (!data_2)
    return;

  data_2[nEmits*(signals.size() + 1) + id] = value.realValue;
}

void ResultWriter::emit(double time)
{
  if (!data_2)
    return;

  data_2[nEmits*(signals.size() + 1) + 0] = time;
  nEmits++;

  if (nEmits >= bufferSize)
  {
    writeFile();
    nEmits = 0;
  }
}
