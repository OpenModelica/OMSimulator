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

#include "Logging.h"

#include <string.h>

oms3::MatReader::MatReader(const char* filename)
  : ResultReader(filename), transposed(true)
{
  FILE *pFile = fopen(filename, "rb");
  if (!pFile)
    logError("MatReader::MatReader failed opening file \"" + std::string(filename) + "\"");

  AClass = readMatVer4Matrix(pFile);
  name = readMatVer4Matrix(pFile);
  skipMatVer4Matrix(pFile);
  dataInfo = readMatVer4Matrix(pFile);
  data_1 = readMatVer4Matrix(pFile);
  data_2 = readMatVer4Matrix(pFile);
  fclose(pFile);

  // detect if matrices are transposed or not
  char *buffer = new char[AClass->header.ncols+1];
  int i;
  for (i = 0; i < AClass->header.ncols; ++i)
    buffer[i] = *((char*)AClass->data + AClass->header.mrows*i + 3);
  buffer[i] = '\0';
  // Fix missing closing \0
  for (i--; i>0 && buffer[i] == ' '; i--)
    buffer[i] = '\0';

  if (!strcmp(buffer, "binTrans"))
    transposed = true;
  else if (!strcmp(buffer, "binNormal"))
    transposed = false;
  else
  {
    transposed = true;
    logWarning("[MatReader::MatReader] Assuming \"binTrans\" (got: \"" + std::string(buffer) + "\"");
  }
  delete[] buffer;

  // Fix MatVer4Matrix* name
  char *var_buffer = new char[transposed ? (name->header.mrows+1) : (name->header.ncols+1)];
  for (int i = 0; i < (transposed ? name->header.ncols : name->header.mrows); ++i)
  {
    if (transposed)
    {
      memcpy(var_buffer, (char*)name->data + name->header.mrows*i, name->header.mrows);
      // Fix missing closing \0
      for (int j=name->header.mrows-1; j>0 && var_buffer[j] == ' '; j--)
        var_buffer[j] = '\0';
    }
    else
    {
      int j;
      for (j=0; j < name->header.ncols; ++j)
        var_buffer[j] = (*((char*)name->data + name->header.mrows*j + i));
      var_buffer[j] = '\0';
    }

    signals.push_back(var_buffer);
  }
  delete[] var_buffer;
}

oms3::MatReader::~MatReader()
{
  freeMatVer4Matrix(&AClass);
  freeMatVer4Matrix(&name);
  freeMatVer4Matrix(&dataInfo);
  freeMatVer4Matrix(&data_1);
  freeMatVer4Matrix(&data_2);
}

oms3::ResultReader::Series* oms3::MatReader::getSeries(const char* var)
{
  // find index
  int index = -1;

  for (int i = 0; i < signals.size(); ++i)
    if (signals[i] == std::string(var))
      index = i;

  if (index == -1)
  {
    logWarning("MatReader::getSeries: series " + std::string(var) + " not found");
    return NULL;
  }

  int32_t info[4];
  if (transposed)
    memcpy(&info, (int32_t*)dataInfo->data + 4 * index, 4 * sizeof(int32_t));
  else
  {
    for (int i=0; i < dataInfo->header.ncols; ++i)
      info[i] = *((int32_t*)dataInfo->data + dataInfo->header.mrows*i + index);
  }

  MatVer4Matrix *data = NULL;
  if (info[0] == 1)
    data = data_1;
  else if (info[0] == 2)
    data = data_2;
  else
    return NULL;

  Series *series = new Series;

  series->length = transposed ? data->header.ncols : data->header.mrows;
  series->time = new double[series->length];
  series->value = new double[series->length];

  double sign = info[1] > 0 ? 1.0 : -1.0;
  int info_1 = info[1] > 0 ? info[1] : -info[1];
  for (int i = 0; i < series->length; ++i)
  {
    if (transposed)
    {
      series->time[i] = ((double*)data->data)[data->header.mrows * i];
      series->value[i] = sign*((double*)data->data)[data->header.mrows * i + (info_1 - 1)];
    }
    else
    {
      series->time[i] = ((double*)data->data)[data->header.ncols * 0 + i];
      series->value[i] = sign*((double*)data->data)[data->header.mrows * (info_1 - 1) + i];
    }
  }

  return series;
}
