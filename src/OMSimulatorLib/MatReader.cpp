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

#include "MatReader.h"
#include "MatVer4.h"

#include "oms2_Logging.h"

#include <string.h>

MatReader::MatReader(const char* filename)
  : ResultReader(filename)
{
  FILE *pFile = fopen(filename, "rb");
  if (!pFile)
    logError("MatReader::MatReader failed opening file \"" + std::string(filename) + "\"");
  skipMatVer4Matrix(pFile);
  name = readMatVer4Matrix(pFile);
  skipMatVer4Matrix(pFile);
  dataInfo = readMatVer4Matrix(pFile);
  data_1 = readMatVer4Matrix(pFile);
  data_2 = readMatVer4Matrix(pFile);
  fclose(pFile);
}

MatReader::~MatReader()
{
  freeMatVer4Matrix(&name);
  freeMatVer4Matrix(&dataInfo);
  freeMatVer4Matrix(&data_1);
  freeMatVer4Matrix(&data_2);
}

ResultReader::Series* MatReader::getSeries(const char* var)
{
  // find index
  int index = -1;
  char *var_buffer = new char[name->header.mrows];

  for (int i = 0; i < name->header.ncols; ++i)
  {
    // Fix missing closing \0
    memcpy(var_buffer, (char*)name->data + name->header.mrows*i, name->header.mrows);
    for (int i=name->header.mrows-1; i>0 && var_buffer[i] == ' '; i--)
      var_buffer[i] = '\0';

    if (!strcmp(var, var_buffer))
      index = i;
  }

  delete[] var_buffer;
  var_buffer = NULL;

  if (index == -1)
  {
    logWarning("MatReader::getSeries: series " + std::string(var) + " not found");
    return NULL;
  }

  int32_t info[4];
  memcpy(&info, (int32_t*)dataInfo->data + 4 * index, 4 * sizeof(int32_t));

  if (info[1] < 0)
    return NULL;

  MatVer4Matrix *data = NULL;
  if (info[0] == 1)
    data = data_1;
  else if (info[0] == 2)
    data = data_2;
  else
    return NULL;

  Series *series = new Series;

  series->length = data->header.ncols;
  series->time = new double[series->length];
  series->value = new double[series->length];

  for (int i = 0; i < series->length; ++i)
  {
    series->time[i] = ((double*)data->data)[data->header.mrows * i];
    series->value[i] = ((double*)data->data)[data->header.mrows * i + (info[1] - 1)];
  }

  return series;
}
