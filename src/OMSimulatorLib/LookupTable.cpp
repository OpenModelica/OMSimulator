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
#include "LookupTable.h"
#include "ResultReader.h"

#include <string>
#include <unordered_map>

LookupTable::LookupTable()
{
  logTrace();
  resultReader = NULL;
}

LookupTable::~LookupTable()
{
  logTrace();
  close();
}

bool LookupTable::open(const std::string& filename)
{
  logTrace();

  if (resultReader)
  {
    logWarning("LookupTable::open: This table is already loaded.");
    return false;
  }

  this->filename = filename;
  resultReader = ResultReader::newReader(filename.c_str());

  if (!resultReader)
  close();

  return (resultReader != NULL);
}

void LookupTable::close()
{
  logTrace();

  filename = "";

  for (auto it=series.begin(); it != series.end(); it++)
    ResultReader::deleteSeries(&it->second);
  series.clear();

  if (resultReader)
  {
    delete resultReader;
    resultReader = NULL;
  }
}

double LookupTable::getValue(const std::string& var, const double time)
{
  logTrace();
  if (!resultReader)
    logFatal("LookupTable::getValue: This table isn't initialized properly.");

  if (series.find(var) == series.end())
    series[var] = resultReader->getSeries(var.c_str());

  for (int i=1; i<series[var]->length; ++i)
  {
    if (series[var]->time[i-1] == time)
      return series[var]->value[i-1];
    else if (series[var]->time[i-1] <= time &&
        series[var]->time[i] >= time)
    {
      double m = (series[var]->value[i] - series[var]->value[i-1]) / (series[var]->time[i] - series[var]->time[i-1]);
      return series[var]->value[i-1] + (time - series[var]->time[i-1]) * m;
    }
  }

  logFatal("LookupTable::getValue: Time out of range.");
  return 0.0;
}
