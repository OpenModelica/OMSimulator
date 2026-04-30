/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "ResultWriter.h"
#include "Flags.h"
#include "Model.h"
#include "Scope.h"

oms::ResultWriter::ResultWriter(unsigned int bufferSize)
  : bufferSize(bufferSize),
    nEmits(0),
    data_2(NULL)
{
}

oms::ResultWriter::~ResultWriter()
{
  if (data_2)
    delete[] data_2;
}

unsigned int oms::ResultWriter::addSignal(const ComRef& name, const std::string& description, SignalType_t type)
{
  Signal signal;
  signal.name = name;
  signal.description = description;
  signal.type = type;

  oms::Model* model = oms::Scope::GetInstance().getModel(name.front());
  if (Flags::StripRoot() || (model && model->isIsolatedFMUModel()))
  {
    signal.name.pop_front();
    signal.name.pop_front();
    if (model && model->isIsolatedFMUModel())
      signal.name.pop_front();
  }

  if (signal.name.isEmpty())
    return 0;

  signals.push_back(signal);
  return (unsigned int) signals.size();
}

void oms::ResultWriter::addParameter(const ComRef& name, const std::string& description, SignalType_t type, SignalValue_t value)
{
  Parameter parameter;
  parameter.signal.name = name;
  parameter.signal.description = description;
  parameter.signal.type = type;
  parameter.value = value;

  oms::Model* model = oms::Scope::GetInstance().getModel(name.front());
  if (Flags::StripRoot() || (model && model->isIsolatedFMUModel()))
  {
    parameter.signal.name.pop_front();
    parameter.signal.name.pop_front();
    if (model && model->isIsolatedFMUModel())
      parameter.signal.name.pop_front();
  }

  if (parameter.signal.name.isEmpty())
    return;

  parameters.push_back(parameter);
}

bool oms::ResultWriter::create(const std::string& filename, double startTime, double stopTime)
{
  if (!createFile(filename, startTime, stopTime))
    return false;

  data_2 = new double[bufferSize*(signals.size() + 1)];
  nEmits = 0;
  return true;
}

void oms::ResultWriter::close()
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

void oms::ResultWriter::updateSignal(unsigned int id, SignalValue_t value)
{
  if (!data_2)
    return;

  switch (signals[id-1].type)
  {
    case SignalType_REAL:
      data_2[nEmits*(signals.size() + 1) + id] = value.realValue;
      break;
    case SignalType_INT:
      data_2[nEmits*(signals.size() + 1) + id] = value.intValue;
      break;
    case SignalType_BOOL:
      data_2[nEmits*(signals.size() + 1) + id] = value.boolValue;
      break;
  }
}

void oms::ResultWriter::emit(double time)
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
