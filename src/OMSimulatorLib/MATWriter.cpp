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

#include "MATWriter.h"
#include "ResultWriter.h"
#include "MatVer4.h"
#include "oms2/Logging.h"
#include "Util.h"

#include <stdio.h>
#include <string>
#include <cstring>
#include <errno.h>

MATWriter::MATWriter(unsigned int bufferSize)
  : ResultWriter(bufferSize),
    pFile(NULL)
{
}

MATWriter::~MATWriter()
{
  closeFile();
}

bool MATWriter::createFile(const std::string& filename, double startTime, double stopTime)
{
  if (pFile)
  {
    logError("MATWriter::createFile: File is already open");
    return false;
  }

  pFile = fopen(filename.c_str(), "wb+");

  if (!pFile)
  {
    logError("MATWriter::createFile: " + std::string(strerror(errno)));
    return false;
  }

  //       Name: Aclass
  //       Rank: 2
  // Dimensions: 4 x 11
  // Class Type: Character Array
  //  Data Type: 8-bit, unsigned integer
  const char Aclass[] = "A1\0bt.\0ir1\0na\0\0Tj\0\0re\0\0ac\0\0nt\0\0so\0\0\0r\0\0\0y\0\0\0";
  writeMatVer4Matrix(pFile, "Aclass", 4, 11, Aclass, MatVer4Type_CHAR);

  //       Name: name
  //       Rank: 2
  // Dimensions: maxLength x nVars
  // Class Type: Character Array
  //  Data Type: 8-bit, unsigned integer
  size_t maxLength = 5;
  for (int i = 0; i < signals.size(); ++i)
    if (signals[i].name.size() + 1 > maxLength)
      maxLength = signals[i].name.size() + 1;
  for (int i = 0; i < parameters.size(); ++i)
    if (parameters[i].signal.name.size() + 1 > maxLength)
      maxLength = parameters[i].signal.name.size() + 1;
  char* name = new char[maxLength * (1 + signals.size() + parameters.size())]();
  memcpy(name, "time", 4);
  for (int i = 0; i < signals.size(); ++i)
    memcpy(&name[maxLength * (1 + i)], signals[i].name.c_str(), signals[i].name.size());
  for (int i = 0; i < parameters.size(); ++i)
    memcpy(&name[maxLength * (1 + signals.size() + i)], parameters[i].signal.name.c_str(), parameters[i].signal.name.size());
  writeMatVer4Matrix(pFile, "name", maxLength, 1 + signals.size() + parameters.size(), name, MatVer4Type_CHAR);
  delete[] name;
  name = NULL;

  //       Name: description
  //       Rank: 2
  // Dimensions: maxLength x nVars
  // Class Type: Character Array
  //  Data Type: 8-bit, unsigned integer
  maxLength = 10;
  for (int i = 0; i < signals.size(); ++i)
    if (signals[i].description.size() + 1 > maxLength)
      maxLength = signals[i].description.size() + 1;
  for (int i = 0; i < parameters.size(); ++i)
    if (parameters[i].signal.description.size() + 1 > maxLength)
      maxLength = parameters[i].signal.description.size() + 1;
  char* description = new char[maxLength * (1 + signals.size() + parameters.size())]();
  memcpy(description, "Time in s", 9);
  for (int i = 0; i < signals.size(); ++i)
    memcpy(&description[maxLength * (1 + i)], signals[i].description.c_str(), signals[i].description.size());
  for (int i = 0; i < parameters.size(); ++i)
    memcpy(&description[maxLength * (1 + signals.size() + i)], parameters[i].signal.description.c_str(), parameters[i].signal.description.size());
  writeMatVer4Matrix(pFile, "description", maxLength, 1 + signals.size() + parameters.size(), description, MatVer4Type_CHAR);
  delete[] description;
  description = NULL;

  //       Name: dataInfo
  //       Rank: 2
  // Dimensions: 4 x nVars
  // Class Type: 32-bit, signed integer array
  //  Data Type: 32-bit, signed integer
  int32_t* dataInfo = new int32_t[4 * (1 + signals.size() + parameters.size())];
  dataInfo[0] = 0;
  dataInfo[1] = 1;
  dataInfo[2] = 0;
  dataInfo[3] = -1;
  for (int i = 0; i < signals.size(); ++i)
  {
    dataInfo[4 * (1 + i) + 0] = 2;
    dataInfo[4 * (1 + i) + 1] = 2 + i;
    dataInfo[4 * (1 + i) + 2] = 0;
    dataInfo[4 * (1 + i) + 3] = -1;
  }
  for (int i = 0; i < parameters.size(); ++i)
  {
    dataInfo[4 * (1 + signals.size() + i) + 0] = 1;
    dataInfo[4 * (1 + signals.size() + i) + 1] = 2 + i;
    dataInfo[4 * (1 + signals.size() + i) + 2] = 0;
    dataInfo[4 * (1 + signals.size() + i) + 3] = 0;
  }
  writeMatVer4Matrix(pFile, "dataInfo", 4, 1 + signals.size() + parameters.size(), dataInfo, MatVer4Type_INT32);
  delete[] dataInfo;
  dataInfo = NULL;

  //       Name: data_1
  //       Rank: 2
  // Dimensions: (1+nParams) x 2
  // Class Type: Double Precision Array
  //  Data Type: IEEE 754 double-precision
  double* data_1 = new double[(1 + parameters.size()) * 2];
  data_1[0] = startTime;
  data_1[(1 + parameters.size())] = stopTime;
  for (int i = 0; i < parameters.size(); ++i)
  {
    data_1[i + 1] = parameters[i].value.realValue;
    data_1[(1 + parameters.size()) + i + 1] = parameters[i].value.realValue;
  }
  writeMatVer4Matrix(pFile, "data_1", 1 + parameters.size(), 2, data_1, MatVer4Type_DOUBLE);
  delete[] data_1;
  data_1 = NULL;

  //       Name: data_2
  //       Rank: 2
  // Dimensions: nSeries x nPoints
  // Class Type: Double Precision Array
  //  Data Type: IEEE 754 double-precision
  pos_data_2 = ftell(pFile);
  writeMatVer4Matrix(pFile, "data_2", 1 + signals.size(), 0, NULL, MatVer4Type_DOUBLE);

  return true;
}

void MATWriter::closeFile()
{
  if (pFile)
  {
    writeFile();
    fclose(pFile);
    pFile = NULL;
  }
}

void MATWriter::writeFile()
{
  appendMatVer4Matrix(pFile, pos_data_2, "data_2", 1 + signals.size(), nEmits, data_2, MatVer4Type_DOUBLE);
}
