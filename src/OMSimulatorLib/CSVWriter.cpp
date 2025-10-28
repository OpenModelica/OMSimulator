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

#include "CSVWriter.h"

#include "Flags.h"
#include "Logging.h"
#include "ResultWriter.h"

#include <cstring>  // strerror
#include <string>

oms::CSVWriter::CSVWriter(unsigned int bufferSize)
  : ResultWriter(bufferSize),
    pFile(NULL)
{
}

oms::CSVWriter::~CSVWriter()
{
  closeFile();
}

bool oms::CSVWriter::createFile(const std::string& filename, double startTime, double stopTime)
{
  if (pFile)
    return false;

  pFile = fopen(filename.c_str(), "w");

  if (!pFile)
  {
    logError("CSVWriter::createFile: " + std::string(strerror(errno)));
    return false;
  }

  if (!Flags::SkipCSVHeader())
    fputs("\"sep=,\"\n", pFile);

  // first signal is always 'time'
  fputs("\"time\"", pFile);

  // write signal names to csv file
  for (int i = 0; i < signals.size(); ++i)
    fprintf(pFile, ",\"%s\"", signals[i].name.c_str());

  // write parameter headers to csv file
  if (Flags::AddParametersToCSV())
    for (int i = 0; i < parameters.size(); ++i)
      fprintf(pFile, ",\"%s\"", parameters[i].signal.name.c_str());

  fputs("\n", pFile);
  return true;
}

void oms::CSVWriter::closeFile()
{
  if (!pFile)
    return;

  writeFile();
  fclose(pFile);
  pFile = NULL;
}

void oms::CSVWriter::writeFile()
{
  for (int i = 0; i < nEmits; ++i)
  {
    // first signal is always 'time'
    fprintf(pFile, "%.12g", data_2[i * (signals.size() + 1) + 0]);

    // write signals to csv file
    for (int j = 1; j < signals.size() + 1; ++j)
      fprintf(pFile, ", %.12g", data_2[i * (signals.size() + 1) + j]);

    // write parameters to csv file
    if (Flags::AddParametersToCSV())
    {
      for (int j = 0; j < parameters.size(); ++j)
      {
        switch (parameters[j].signal.type)
        {
          case SignalType_REAL:
            fprintf(pFile, ", %.12g", parameters[j].value.realValue);
            break;
          case SignalType_INT:
            fprintf(pFile, ", %d", parameters[j].value.intValue);
            break;
          case SignalType_BOOL:
            fprintf(pFile, ", %d", parameters[j].value.boolValue);
            break;
        }
      }
    }

    fputs("\n", pFile);
  }
  fflush(pFile);
}
