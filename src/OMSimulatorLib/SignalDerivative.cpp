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

#include "SignalDerivative.h"

#include "Logging.h"
#include <cmath>
#include <cstring>
#include <vector>

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

void oms::SignalDerivative::resize(unsigned int order)
{
  if (this->order == order)
    return;

  if (values)
    delete[] values;

  this->order = order;
  values = order == 0 ? nullptr : new double[order];
}

void oms::SignalDerivative::replaceNonFinite(const char* function)
{
  for (unsigned int i=0; i<order; ++i)
  {
    if (std::isnan(values[i]))
    {
      logWarning(std::string(function) + " returned NAN");
      values[i] = 0.0;
    }
    if (std::isinf(values[i]))
    {
      logWarning(std::string(function) + " returned +/-inf");
      values[i] = 0.0;
    }
  }
}

oms_status_enu_t oms::SignalDerivative::getRealOutputDerivatives(unsigned int order, fmi2InstanceHandle* instance, fmi2ValueReference vr)
{
  resize(order);
  if (order == 0)
    return oms_status_ok;

  if (fmi2OK != fmi2_getRealOutputDerivatives(instance, &vr, 1, (fmi2Integer*)&this->order, values))
  {
    resize(0);
    return logError("fmi2_getRealOutputDerivatives failed");
  }

  replaceNonFinite("fmi2_getRealOutputDerivatives");
  return oms_status_ok;
}

oms_status_enu_t oms::SignalDerivative::getRealOutputDerivatives(unsigned int order, fmi3InstanceHandle* instance, fmi3ValueReference vr)
{
  resize(order);
  if (order == 0)
    return oms_status_ok;

  // fmi3GetOutputDerivatives takes one derivative order per value reference,
  // so the same value reference is requested once for each order 1..order
  std::vector<fmi3ValueReference> vrs(order, vr);
  std::vector<fmi3Int32> orders(order);
  for (unsigned int i=0; i<order; ++i)
    orders[i] = i+1;

  if (fmi3OK != fmi3_getOutputDerivatives(instance, vrs.data(), order, orders.data(), values, order))
  {
    resize(0);
    return logError("fmi3_getOutputDerivatives failed");
  }

  replaceNonFinite("fmi3_getOutputDerivatives");
  return oms_status_ok;
}

oms_status_enu_t oms::SignalDerivative::setRealInputDerivatives(fmi2InstanceHandle* instance, fmi2ValueReference vr) const
{
  if (order > 0 && values)
  {
    if (fmi2OK != fmi2_setRealInputDerivatives(instance, &vr, 1, (fmi2Integer*)&order, (fmi2Real*)values))
      return oms_status_error;
  }
  return oms_status_ok;
}

oms::SignalDerivative::operator std::string() const
{
  std::string str = "[" + std::to_string(order) + ": ";
  if (values)
  {
    if (order > 0)
      str += std::to_string(values[0]);
    for (int i=1; i<order; ++i)
      str += "; " + std::to_string(values[i]);
  }
  else
    str += "NULL";

  return str + "]";
}
