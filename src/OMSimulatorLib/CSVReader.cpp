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

#include "CSVReader.h"

#include "Logging.h"
#include "Util.h"

#include <string.h>

CSVReader::CSVReader(const char* filename)
  : ResultReader(filename)
{
  std::ifstream file(filename);
  std::string line;

  if (file.fail())
    logFatal("MatReader::MatReader failed opening file \"" + std::string(filename) + "\"");

  // count number of lines
  unsigned int number_of_lines = 0;
  bool usingSepHeader = false;
  while (std::getline(file, line))
  {
    if (!line.empty())
    {
      if(number_of_lines == 0 && line.substr(0, 5) =="\"sep=")
        usingSepHeader = true;
      else
        ++number_of_lines;
    }
  }
  length = number_of_lines-1;

  // read first line
  file.clear();
  file.seekg(0, std::ios::beg);

  // TODO: process sep header properly
  if (usingSepHeader)
    std::getline(file, line);

  std::getline(file, line);
  bool quoteSign=false;
  std::string name;
  for(char& c : line)
  {
    if (!quoteSign && '"' == c)
    quoteSign = true;
    else if (quoteSign && '"' == c)
    {
      quoteSign = false;
      trim(name);
      if (!name.empty())
      {
        names.push_back(name);
        name.clear();
      }
    }
    else if (!quoteSign && ',' == c)
    {
      quoteSign = false;
      trim(name);
      if (!name.empty())
      {
        names.push_back(name);
        name.clear();
      }
    }
    else
      name += c;
  }
  trim(name);
  if (!name.empty())
    names.push_back(name);

  // read data
  data = new double[length * names.size()];
  int row=0;
  int col=0;
  while (std::getline(file, line))
    if (!line.empty())
    {
      quoteSign = false;
      col = 0;
      for(char& c : line)
      {
        if (!quoteSign && '"' == c)
        quoteSign = true;
        else if (quoteSign && '"' == c)
        {
          quoteSign = false;
          trim(name);
          if (!name.empty())
          {
            data[row * names.size() + col] = strtod(name.c_str(), NULL);
            name.clear();
            col++;
          }
        }
        else if (!quoteSign && ',' == c)
        {
          quoteSign = false;
          trim(name);
          if (!name.empty())
          {
            data[row * names.size() + col] = strtod(name.c_str(), NULL);
            name.clear();
            col++;
          }
        }
        else
          name += c;
      }
      trim(name);
      if (!name.empty())
      {
        data[row * names.size() + col] = strtod(name.c_str(), NULL);
        name.clear();
        col++;
      }
      row++;
    }
}

CSVReader::~CSVReader()
{
  delete[] data;
}

ResultReader::Series* CSVReader::getSeries(const char* var)
{
  // find index
  int index = -1;
  for (int i = 0; i < names.size(); ++i)
    if (!strcmp(var, names[i].c_str()))
      index = i;

  if (index == -1)
  {
    logWarning("CSVReader::getSeries: series " + std::string(var) + " not found");
    return NULL;
  }

  Series *series = new Series;
  series->length = length;
  series->time = new double[series->length];
  series->value = new double[series->length];

  for (unsigned int row = 0; row < series->length; ++row)
  {
    series->time[row] = data[row * names.size()];
    series->value[row] = data[row * names.size() + index];
  }

  return series;
}
