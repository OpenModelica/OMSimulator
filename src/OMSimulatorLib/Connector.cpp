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

#include "Connector.h"
#include "Logging.h"
#include "ssd/Tags.h"

#include <cstring>

oms3::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms3::ComRef& name)
{
  this->causality = causality;
  this->type = type;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->geometry = NULL;
}

oms3::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms3::ComRef &name, double height)
{
  this->causality = causality;
  this->type = type;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  double x, y;
  switch (causality)
  {
    case oms_causality_input:
      // inputs are placed on the left of the component
      x = 0.0;
      y = height;
      break;
    case oms_causality_output:
      // outputs are placed on the right of the component
      x = 1.0;
      y = height;
      break;
    default:
      // parameters are placed on the top of the component
      x = height;
      y = 1.0;
      break;
  }
  this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(x, y));
}

oms3::Connector::~Connector()
{
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
}

oms_status_enu_t oms3::Connector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node = root.append_child(oms2::ssd::ssd_connector);
  node.append_attribute("name") = std::string(getName()).c_str();
  switch (this->causality)
  {
  case oms_causality_input:
    node.append_attribute("kind") = "input";
    break;
  case oms_causality_output:
    node.append_attribute("kind") = "output";
    break;
  case oms_causality_parameter:
    node.append_attribute("kind") = "parameter";
    break;
  }
  switch (this->type)
  {
  case oms_signal_type_boolean:
    node.append_attribute("type") = "Boolean";
    break;
  case oms_signal_type_enum:
    node.append_attribute("type") = "Enumeration";
    break;
  case oms_signal_type_integer:
    node.append_attribute("type") = "Integer";
    break;
  case oms_signal_type_real:
    node.append_attribute("type") = "Real";
    break;
  case oms_signal_type_string:
    node.append_attribute("type") = "String";
    break;
  }
  if (this->geometry)
  {
    return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(node);
  }
  return oms_status_ok;
}

oms3::Connector::Connector(const oms3::Connector &rhs)
{
  this->causality = rhs.causality;
  this->type = rhs.type;

  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;
}

oms3::Connector &oms3::Connector::operator=(const oms3::Connector &rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->causality = rhs.causality;
  this->type = rhs.type;

  if (this->name)
    delete[] this->name;
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  this->setGeometry(reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

void oms3::Connector::setName(const oms3::ComRef &name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms3::Connector::setGeometry(const oms2::ssd::ConnectorGeometry *newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*newGeometry));
}


oms2::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms2::SignalRef& name)
{
  this->causality = causality;
  this->type = type;

  std::string str = name.toString();
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->geometry = NULL;
}

oms2::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms2::SignalRef& name, double height)
{
  this->causality = causality;
  this->type = type;

  std::string str = name.toString();
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  double x, y;
  switch (causality)
  {
    case oms_causality_input:
      // inputs are placed on the left of the component
      x = 0.0;
      y = height;
      break;
    case oms_causality_output:
      // outputs are placed on the right of the component
      x = 1.0;
      y = height;
      break;
    default:
      // parameters are placed on the top of the component
      x = height;
      y = 1.0;
      break;
  }
  this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(x, y));
}

oms2::Connector::~Connector()
{
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
}

oms2::Connector::Connector(const oms2::Connector& rhs)
{
  this->causality = rhs.causality;
  this->type = rhs.type;

  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;
}

oms2::Connector& oms2::Connector::operator=(const oms2::Connector& rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  this->causality = rhs.causality;
  this->type = rhs.type;

  if (this->name)
    delete[] this->name;
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  this->setGeometry(reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

oms_status_enu_t oms2::Connector::exportToSSD(pugi::xml_node& root) const
{
  if (this->geometry)
  {
    pugi::xml_node node = root.append_child(oms2::ssd::ssd_connector);
    node.append_attribute("name") = getName().getVar().c_str();
    switch (this->causality)
    {
    case oms_causality_input:
      node.append_attribute("kind") = "input";
      break;
    case oms_causality_output:
      node.append_attribute("kind") = "output";
      break;
    case oms_causality_parameter:
      node.append_attribute("kind") = "parameter";
      break;
    }
    switch (this->type)
    {
    case oms_signal_type_boolean:
      node.append_attribute("type") = "Boolean";
      break;
    case oms_signal_type_enum:
      node.append_attribute("type") = "Enumeration";
      break;
    case oms_signal_type_integer:
      node.append_attribute("type") = "Integer";
      break;
    case oms_signal_type_real:
      node.append_attribute("type") = "Real";
      break;
    case oms_signal_type_string:
      node.append_attribute("type") = "String";
      break;
    }
    return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(node);
  }
  return oms_status_ok;
}

void oms2::Connector::setName(const oms2::SignalRef& name)
{
  if (this->name)
    delete[] this->name;

  std::string str = name.toString();
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms2::Connector::setGeometry(const oms2::ssd::ConnectorGeometry* newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*newGeometry));
}
