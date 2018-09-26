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
#include "Logging.h"
#include "ssd/Tags.h"

#include <cstring>
#include <iostream>


oms3::Connection::Connection(const oms3::ComRef& conA, const oms3::ComRef& conB, oms3_connection_type_enu_t type)
{
  std::string str;

  this->type = type;

  str = std::string(conA);
  this->conA = new char[str.size()+1];
  strcpy(this->conA, str.c_str());

  str = std::string(conB);
  this->conB = new char[str.size()+1];
  strcpy(this->conB, str.c_str());

  tlmparameters = NULL;
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(new oms2::ssd::ConnectionGeometry());
}

oms3::Connection::~Connection()
{
  if (this->conA) delete[] this->conA;
  if (this->conB) delete[] this->conB;
  if (this->tlmparameters) delete tlmparameters;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectionGeometry*>(this->geometry);
}

oms3::Connection::Connection(const oms3::Connection& rhs)
{
  this->type = rhs.type;

  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms2::ssd::ConnectionGeometry* geometry_ = new oms2::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms2::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

oms3::Connection& oms3::Connection::operator=(const oms3::Connection& rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->type != rhs.type)
    logWarning("[oms2::Connection::operator=] changing type of connection");
  this->type = rhs.type;

  if (this->conA) delete[] this->conA;
  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  if (this->conB) delete[] this->conB;
  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms2::ssd::ConnectionGeometry* geometry_ = new oms2::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms2::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);

  return *this;
}

oms_status_enu_t oms3::Connection::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node;
  if(type == oms3_connection_single) {
    node = root.append_child(oms2::ssd::ssd_connection);
  }
  else if(type == oms3_connection_bus) {
    node = root.append_child("OMSimulator:BusConnection");
  }
  else if(type == oms3_connection_tlm) {
    node = root.append_child("OMSimulator:TLMBusConnection");
  }

  ComRef startConnectorRef(conA);
  ComRef startElementRef = startConnectorRef.pop_front();
  ComRef endConnectorRef(conB);
  ComRef endElementRef = endConnectorRef.pop_front();

  node.append_attribute("startElement") = startElementRef.c_str();
  node.append_attribute("startConnector") = startConnectorRef.c_str();
  node.append_attribute("endElement") = endElementRef.c_str();
  node.append_attribute("endConnector") = endConnectorRef.c_str();

  if(type == oms3_connection_tlm) {
    node.append_attribute("delay") = std::to_string(tlmparameters->delay).c_str();
    node.append_attribute("alpha") = std::to_string(tlmparameters->alpha).c_str();
    node.append_attribute("impedance") = std::to_string(tlmparameters->impedance).c_str();
    node.append_attribute("impedancerot") = std::to_string(tlmparameters->impedancerot).c_str();
  }

  getGeometry()->exportToSSD(node);

  return oms_status_ok;
}

void oms3::Connection::setGeometry(const oms2::ssd::ConnectionGeometry* newGeometry)
{
  oms2::ssd::ConnectionGeometry* geometry_ = reinterpret_cast<oms2::ssd::ConnectionGeometry*>(this->geometry);
  if (geometry_)
    delete geometry_;
  geometry_ = new oms2::ssd::ConnectionGeometry(*newGeometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

void oms3::Connection::setTLMParameters(double delay, double alpha, double impedance, double impedancerot)
{
  if(!tlmparameters)
    tlmparameters = new oms3_tlm_connection_parameters_t;

  tlmparameters->delay = delay;
  tlmparameters->alpha = alpha;
  tlmparameters->impedance = impedance;
  tlmparameters->impedancerot = impedancerot;
}


bool oms3::Connection::isEqual(const oms3::ComRef& signalA, const oms3::ComRef& signalB) const
{
  return (signalA == oms3::ComRef(this->conA) && signalB == oms3::ComRef(this->conB)) || (signalA == oms3::ComRef(this->conB) && signalB == oms3::ComRef(this->conA));
}

bool oms3::Connection::isEqual(const oms3::Connection& connection) const
{
  const oms3::ComRef& conA_ = connection.getSignalA();
  const oms3::ComRef& conB_ = connection.getSignalB();
  return isEqual(conA_, conB_);
}



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

  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(new oms2::ssd::ConnectionGeometry());
}

oms2::Connection::~Connection()
{
  if (this->parent) delete[] this->parent;
  if (this->conA) delete[] this->conA;
  if (this->conB) delete[] this->conB;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectionGeometry*>(this->geometry);
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

  if (this->type != rhs.type)
    logWarning("[oms2::Connection::operator=] changing type of connection");
  this->type = rhs.type;

  if (this->parent) delete[] this->parent;
  this->parent = new char[strlen(rhs.parent)+1];
  strcpy(this->parent, rhs.parent);

  if (this->conA) delete[] this->conA;
  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  if (this->conB) delete[] this->conB;
  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms2::ssd::ConnectionGeometry* geometry_ = new oms2::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms2::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);

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
  return parent.isEqual(this->parent) && ((signalA.isEqual(this->conA) && signalB.isEqual(this->conB)) || (signalA.isEqual(this->conB) && signalB.isEqual(this->conA)));
}

bool oms2::Connection::isEqual(const oms2::Connection& connection) const
{
  const oms2::ComRef& parent_ = connection.getParent();
  const oms2::SignalRef& conA_ = connection.getSignalA();
  const oms2::SignalRef& conB_ = connection.getSignalB();
  return isEqual(parent_, conA_, conB_);
}

void oms2::Connection::describe()
{
  std::cout << getParent().toString() << ": " << getSignalA().toString() << " -> " << getSignalB().toString() << std::endl;
}

void oms2::Connection::setParent(const oms2::ComRef& parent)
{
  std::string str = parent.toString();

  if (this->parent) delete[] this->parent;
  this->parent = new char[str.size()+1];
  strcpy(this->parent, str.c_str());
}
