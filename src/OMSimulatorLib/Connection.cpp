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
 * even the implied warranty of MERCHANTABILITY or FITNESS
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


oms::Connection::Connection(const oms::ComRef& conA, const oms::ComRef& conB, oms_connection_type_enu_t type)
{
  std::string str;

  this->type = type;

  str = std::string(conA);
  this->conA = new char[str.size()+1];
  strcpy(this->conA, str.c_str());

  str = std::string(conB);
  this->conB = new char[str.size()+1];
  strcpy(this->conB, str.c_str());

  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(new oms::ssd::ConnectionGeometry());

  tlmparameters = NULL;
}

oms::Connection::~Connection()
{
  if (this->conA) delete[] this->conA;
  if (this->conB) delete[] this->conB;
  if (this->tlmparameters) delete tlmparameters;
  if (this->geometry) delete reinterpret_cast<oms::ssd::ConnectionGeometry*>(this->geometry);
}

oms::Connection::Connection(const oms::Connection& rhs)
{
  this->type = rhs.type;

  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms::ssd::ConnectionGeometry* geometry_ = new oms::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);

  tlmparameters = NULL;
}

oms::Connection& oms::Connection::operator=(const oms::Connection& rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->type != rhs.type)
    logWarning("[oms::Connection::operator=] changing type of connection");
  this->type = rhs.type;

  if (this->conA) delete[] this->conA;
  this->conA = new char[strlen(rhs.conA)+1];
  strcpy(this->conA, rhs.conA);

  if (this->conB) delete[] this->conB;
  this->conB = new char[strlen(rhs.conB)+1];
  strcpy(this->conB, rhs.conB);

  oms::ssd::ConnectionGeometry* geometry_ = new oms::ssd::ConnectionGeometry();
  *geometry_ = *reinterpret_cast<oms::ssd::ConnectionGeometry*>(rhs.geometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);

  setTLMParameters(rhs.tlmparameters);

  return *this;
}

oms_status_enu_t oms::Connection::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node;
  if(type == oms_connection_single)
    node = root.append_child(oms::ssp::Draft20180219::ssd::connection);
  else if(type == oms_connection_bus || type == oms_connection_tlm)
    node = root.append_child(oms::ssp::Draft20180219::bus_connection);

  ComRef startConnectorRef(conA);
  ComRef startElementRef = startConnectorRef.pop_front();
  ComRef endConnectorRef(conB);
  ComRef endElementRef = endConnectorRef.pop_front();

  node.append_attribute("startElement") = startConnectorRef.isEmpty() ? "" : startElementRef.c_str();
  node.append_attribute("startConnector") = startConnectorRef.isEmpty() ? startElementRef.c_str() : startConnectorRef.c_str();
  node.append_attribute("endElement") = endConnectorRef.isEmpty() ? "" : endElementRef.c_str();
  node.append_attribute("endConnector") = endConnectorRef.isEmpty() ? endElementRef.c_str() : endConnectorRef.c_str();

  if(type == oms_connection_tlm)
  {
    node.append_attribute("delay") = std::to_string(tlmparameters->delay).c_str();
    node.append_attribute("alpha") = std::to_string(tlmparameters->alpha).c_str();
    node.append_attribute("linearimpedance") = std::to_string(tlmparameters->linearimpedance).c_str();
    node.append_attribute("angularimpedance") = std::to_string(tlmparameters->angularimpedance).c_str();
  }

  getGeometry()->exportToSSD(node);

  return oms_status_ok;
}

void oms::Connection::setGeometry(const oms::ssd::ConnectionGeometry* newGeometry)
{
  oms::ssd::ConnectionGeometry* geometry_ = reinterpret_cast<oms::ssd::ConnectionGeometry*>(this->geometry);
  if (geometry_)
    delete geometry_;
  geometry_ = new oms::ssd::ConnectionGeometry(*newGeometry);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

void oms::Connection::setTLMParameters(const oms_tlm_connection_parameters_t* parameters)
{
  if (tlmparameters)
  {
    delete tlmparameters;
    tlmparameters = NULL;
  }

  if (parameters)
    setTLMParameters(parameters->delay, parameters->alpha, parameters->linearimpedance, parameters->angularimpedance);
}

void oms::Connection::setTLMParameters(double delay, double alpha, double linearimpedance, double angualrimpedance)
{
  if (!tlmparameters)
    tlmparameters = new oms_tlm_connection_parameters_t;

  tlmparameters->delay = delay;
  tlmparameters->alpha = alpha;
  tlmparameters->linearimpedance = linearimpedance;
  tlmparameters->angularimpedance = angualrimpedance;
}

bool oms::Connection::isEqual(const oms::ComRef& signalA, const oms::ComRef& signalB) const
{
  return (signalA == oms::ComRef(this->conA) && signalB == oms::ComRef(this->conB)) || (signalA == oms::ComRef(this->conB) && signalB == oms::ComRef(this->conA));
}

bool oms::Connection::isEqual(const oms::Connection& connection) const
{
  const oms::ComRef& conA_ = connection.getSignalA();
  const oms::ComRef& conB_ = connection.getSignalB();
  return isEqual(conA_, conB_);
}

bool oms::Connection::containsSignal(const oms::ComRef& signal)
{
  return signal.isRootOf(oms::ComRef(this->conA)) || signal.isRootOf(oms::ComRef(this->conB));
}
