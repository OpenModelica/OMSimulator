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

#include "SignalDerivative.h"

#include <cstring>

oms::SignalDerivative::SignalDerivative() :SignalDerivative(0, nullptr)
{
}

oms::SignalDerivative::SignalDerivative(unsigned int order)
{
  this->order = order;
  this->values = new double[order];
  memset(this->values, 0, order*sizeof(double));
}

oms::SignalDerivative::SignalDerivative(unsigned int order, double* values)
{
  this->order = order;
  if (order == 0)
    this->values = nullptr;
  else
  {
    this->values = new double[order];
    memcpy(this->values, values, order*sizeof(double));
  }
}

oms::SignalDerivative::~SignalDerivative()
{
  if (values)
    delete[] values;
}

oms::SignalDerivative::SignalDerivative(const oms::SignalDerivative& copy)
{
  order = copy.order;
  if (order == 0)
    values = nullptr;
  else
  {
    values = new double[order];
    memcpy(values, copy.values, order*sizeof(double));
  }
}

oms::SignalDerivative& oms::SignalDerivative::operator=(const oms::SignalDerivative& copy)
{
  // check for self-assignment
  if (&copy == this)
    return *this;

  if (order != copy.order)
  {
    if (values)
      delete[] values;

    order = copy.order;
    if (order == 0)
      values = nullptr;
    else
      values = new double[order];
  }

  if (values)
    memcpy(values, copy.values, order*sizeof(double));

  return *this;
}
