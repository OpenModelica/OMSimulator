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

#include "CSVWriter.h"
#include "ResultWriter.h"

#include <stdio.h>
#include <string>

CSVWriter::CSVWriter(unsigned int bufferSize)
  : ResultWriter(bufferSize),
    pFile(NULL)
{
}

CSVWriter::~CSVWriter()
{
  closeFile();
}

bool CSVWriter::createFile(const std::string& filename, double startTime, double stopTime)
{
  if (pFile)
    return false;

  pFile = fopen(filename.c_str(), "w");
  fprintf(pFile, "\"sep=,\"\n");
  fprintf(pFile, "\"time\"");

  for (int i = 0; i < signals.size(); ++i)
  {
    fprintf(pFile, ", \"%s\"", signals[i].name.c_str());
  }
  fprintf(pFile, "\n");

  return true;
}

void CSVWriter::closeFile()
{
  if (pFile)
  {
    writeFile();
    fclose(pFile);
    pFile = NULL;
  }
}

void CSVWriter::writeFile()
{
  for (int i = 0; i < nEmits; ++i)
  {
    fprintf(pFile, "%.12g", data_2[i * (signals.size() + 1) + 0]);

    for (int j = 1; j < signals.size() + 1; ++j)
      fprintf(pFile, ", %.12g", data_2[i * (signals.size() + 1) + j]);

    fprintf(pFile, "\n");
  }
}
