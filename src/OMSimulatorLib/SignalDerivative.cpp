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

#include "Logging.h"
#include <cmath>
#include <cstring>

oms::SignalDerivative::SignalDerivative()
{
  order = 0;
  values = nullptr;
}

oms::SignalDerivative::SignalDerivative(double der)
{
  order = 1;
  values = new double[order];
  values[0] = der;
}

oms::SignalDerivative::SignalDerivative(unsigned int order, fmi2_import_t* fmu, fmi2_value_reference_t vr)
{
  this->order = order;
  if (this->order == 0)
    values = nullptr;
  else
  {
    values = new double[order];
    if (fmi2_status_ok != fmi2_import_get_real_output_derivatives(fmu, &vr, 1, (fmi2_integer_t*)&this->order, values))
      logError("fmi2_import_get_real_output_derivatives failed");
    else
    {
      if (std::isnan(values[0]))
        logError("fmi2_import_get_real_output_derivatives returned NAN");
      if (std::isinf(values[0]))
        logError("fmi2_import_get_real_output_derivatives returned +/-inf");
    }
  }
}

oms::SignalDerivative::~SignalDerivative()
{
  if (values)
    delete[] values;
}

oms::SignalDerivative::SignalDerivative(const oms::SignalDerivative& rhs)
{
  order = rhs.order;
  if (order == 0)
    values = nullptr;
  else
  {
    values = new double[order];
    memcpy(values, rhs.values, order*sizeof(double));
  }
}

oms::SignalDerivative& oms::SignalDerivative::operator=(const oms::SignalDerivative& rhs)
{
  // check for self-assignment
  if (&rhs == this)
    return *this;

  if (order != rhs.order)
  {
    if (values)
      delete[] values;

    order = rhs.order;
    if (order == 0)
      values = nullptr;
    else
      values = new double[order];
  }

  if (values)
    memcpy(values, rhs.values, order*sizeof(double));

  return *this;
}
