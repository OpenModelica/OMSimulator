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

#include <cstring>

oms2::Connection::Connection(const oms2::ComRef& cref, const std::string& varA, const std::string& varB)
  : conA(cref, varA), conB(cref, varB)
{
  geometry.n = 0;
  geometry.pointsX = NULL;
  geometry.pointsY = NULL;
}

oms2::Connection::Connection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
  : conA(conA), conB(conB)
{
  geometry.n = 0;
  geometry.pointsX = NULL;
  geometry.pointsY = NULL;
}

oms2::Connection::~Connection()
{
  if (geometry.n > 0)
  {
    delete[] geometry.pointsX;
    delete[] geometry.pointsY;
  }
}

oms2::Connection::Connection(const oms2::Connection& rhs)
  : conA(rhs.conA), conB(rhs.conB)
{
  if (rhs.geometry.n > 0)
  {
    this->geometry.n = rhs.geometry.n;
    this->geometry.pointsX = new double[rhs.geometry.n];
    this->geometry.pointsY = new double[rhs.geometry.n];

    memcpy(this->geometry.pointsX, rhs.geometry.pointsX, rhs.geometry.n*sizeof(double));
    memcpy(this->geometry.pointsY, rhs.geometry.pointsY, rhs.geometry.n*sizeof(double));
  }
  else
  {
    this->geometry.n = 0;
    this->geometry.pointsX = NULL;
    this->geometry.pointsY = NULL;
  }
}

oms2::Connection& oms2::Connection::operator=(const oms2::Connection& rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->conA = rhs.conA;
  this->conB = rhs.conB;

  if (rhs.geometry.n > 0)
  {
    this->geometry.n = rhs.geometry.n;
    this->geometry.pointsX = new double[rhs.geometry.n];
    this->geometry.pointsY = new double[rhs.geometry.n];

    memcpy(this->geometry.pointsX, rhs.geometry.pointsX, rhs.geometry.n*sizeof(double));
    memcpy(this->geometry.pointsY, rhs.geometry.pointsY, rhs.geometry.n*sizeof(double));
  }
  else
  {
    this->geometry.n = 0;
    this->geometry.pointsX = NULL;
    this->geometry.pointsY = NULL;
  }

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

void oms2::Connection::setGeometry(const ssd_connection_geometry_t* newGeometry)
{
  if (geometry.n > 0)
  {
    delete[] geometry.pointsX;
    delete[] geometry.pointsY;
  }

  if (newGeometry && newGeometry->n > 0)
  {
    geometry.n = newGeometry->n;
    geometry.pointsX = new double[newGeometry->n];
    geometry.pointsY = new double[newGeometry->n];

    memcpy(geometry.pointsX, newGeometry->pointsX, newGeometry->n*sizeof(double));
    memcpy(geometry.pointsY, newGeometry->pointsY, newGeometry->n*sizeof(double));
  }
  else
  {
    geometry.n = 0;
    geometry.pointsX = NULL;
    geometry.pointsY = NULL;
  }
}
