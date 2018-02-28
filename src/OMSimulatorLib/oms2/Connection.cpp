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

oms2::Connection::Connection(const oms2::ComRef& parent, const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  std::string str;

  this->type = oms_connection_fmi;

  str = parent.toString();
  this->parent = new char[str.size()+1];
  strcpy(this->parent, str.c_str());

  str = conA.toString();
  this->conA = new char[str.size()+1];
  strcpy(this->conA, str.c_str());

  str = conB.toString();
  this->conB = new char[str.size()+1];
  strcpy(this->conB, str.c_str());

  oms2::ssd::ConnectionGeometry* geometry_ = new oms2::ssd::ConnectionGeometry();
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

oms2::Connection::~Connection()
{
  if (this->parent) delete[] this->parent;
  if (this->conA) delete[] this->conA;
  if (this->conB) delete[] this->conB;

  oms2::ssd::ConnectionGeometry* geometry_ = reinterpret_cast<oms2::ssd::ConnectionGeometry*>(this->geometry);
  if (geometry_)
    delete geometry_;
}

oms2::Connection::Connection(const oms2::Connection& rhs)
{
  this->type = rhs.type;

  this->parent = new char[strlen(rhs.parent)+1];
  strcpy(this->parent, rhs.parent);

  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms2::ssd::ConnectionGeometry* geometry_ = new oms2::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms2::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
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

void oms2::Connection::setGeometry(const oms2::ssd::ConnectionGeometry* newGeometry)
{
  oms2::ssd::ConnectionGeometry* geometry_ = reinterpret_cast<oms2::ssd::ConnectionGeometry*>(this->geometry);
  if (geometry_)
    delete geometry_;
  geometry_ = new oms2::ssd::ConnectionGeometry(*newGeometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

bool oms2::Connection::isEqual(const oms2::ComRef& parent, const oms2::SignalRef& signalA, const oms2::SignalRef& signalB) const
{
  return parent.isEqual(this->parent) && signalA.isEqual(this->conA) && signalB.isEqual(this->conB);
}
