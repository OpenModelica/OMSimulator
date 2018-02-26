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

#include "Connection.h"
#include "ssd/ConnectionGeometry.h"

#include <cstring>

oms2::Connection::Connection(const oms2::ComRef& cref, const std::string& varA, const std::string& varB)
  : conA(cref, varA), conB(cref, varB), geometry()
{
}

oms2::Connection::Connection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
  : conA(conA), conB(conB), geometry()
{
}

oms2::Connection::~Connection()
{
}

oms2::Connection::Connection(const oms2::Connection& rhs)
  : conA(rhs.conA), conB(rhs.conB), geometry(rhs.geometry)
{
}

oms2::Connection& oms2::Connection::operator=(const oms2::Connection& rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->conA = rhs.conA;
  this->conB = rhs.conB;
  this->geometry = rhs.geometry;

  return *this;
}

oms2::Connection oms2::Connection::FromStrings(const std::string& conA, const std::string& conB)
{
  oms2::ComRef A = oms2::ComRef(conA);
  oms2::ComRef B = oms2::ComRef(conB);
  std::string varA = A.last().toString();
  std::string varB = B.last().toString();
  A.popLast();
  B.popLast();
  return oms2::Connection(oms2::SignalRef(A, varA), oms2::SignalRef(B, varB));
}

oms2::Connection oms2::Connection::FromStrings(const oms2::ComRef& cref, const std::string& conA, const std::string& conB)
{
  oms2::ComRef A(cref);
  A.append(oms2::ComRef(conA));
  oms2::ComRef B(cref);
  B.append(oms2::ComRef(conB));
  return oms2::Connection::FromStrings(A.toString(), B.toString());
}

void oms2::Connection::setGeometry(const oms2::ssd::ConnectionGeometry* newGeometry)
{
  this->geometry = *newGeometry;
}
