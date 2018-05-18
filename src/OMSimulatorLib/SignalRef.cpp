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

#include "SignalRef.h"
#include "Logging.h"

oms2::SignalRef::SignalRef(const std::string& signal)
{
  size_t sep = signal.find(":");
  if (std::string::npos != sep)
  {
    this->cref = oms2::ComRef(signal.substr(0, sep));
    this->var = signal.substr(sep+1);
  }
  else
  {
    this->var = signal;
    logError("Invalid SignalRef: " + signal);
  }
}

oms2::SignalRef::SignalRef(const oms2::ComRef& cref, const std::string& var)
{
  this->cref = cref;
  this->var = var;
}

oms2::SignalRef::~SignalRef()
{
}

// methods to copy the signal reference
oms2::SignalRef::SignalRef(oms2::SignalRef const& copy)
{
  this->cref = copy.cref;
  this->var = copy.var;
}

oms2::SignalRef& oms2::SignalRef::operator=(oms2::SignalRef const& copy)
{
  // check for self-assignment
  if(&copy == this)
    return *this;

  this->cref = copy.cref;
  this->var = copy.var;
  return *this;
}

bool oms2::SignalRef::operator<(const oms2::SignalRef& rhs)
{
  return toString() < rhs.toString();
}
