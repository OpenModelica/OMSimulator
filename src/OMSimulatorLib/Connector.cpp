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

oms::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name)
{
  this->causality = causality;
  this->type = type;

  this->name = new char[strlen(name.c_str())+1];
  strcpy(this->name, name.c_str());

  this->geometry = NULL;
}

oms::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name, double height)
{
  this->causality = causality;
  this->type = type;

  std::string str(name);
  this->name = new char[strlen(name.c_str())+1];
  strcpy(this->name, name.c_str());

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
  this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms::ssd::ConnectorGeometry(x, y));
}

oms::Connector::~Connector()
{
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry);
}

oms::Connector* oms::Connector::NewConnector(const pugi::xml_node& node)
{
  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string causalityString = node.attribute("kind").as_string();
  std::string typeString = node.attribute("type").as_string();
  oms_causality_enu_t causality = oms_causality_undefined;
  if (causalityString == "input")
    causality = oms_causality_input;
  else if (causalityString == "output")
    causality = oms_causality_output;
  else if (causalityString == "parameter")
    causality = oms_causality_parameter;
  else
  {
    logError("Failed to import " + std::string(oms::ssd::ssd_connector) + ":causality");
    return NULL;
  }
  oms_signal_type_enu_t type = oms_signal_type_real;
  if (typeString == "Real")
    type = oms_signal_type_real;
  else if (typeString == "Integer")
    type = oms_signal_type_integer;
  else if (typeString == "Boolean")
    type = oms_signal_type_boolean;
  else if (typeString == "Enumeration")
    type = oms_signal_type_enum;
  else
  {
    logError("Failed to import " + std::string(oms::ssd::ssd_connector) + ":type");
    return NULL;
  }

  Connector* connector = new Connector(causality, type, cref);
  if (!connector)
  {
    logError("Failed to import " + std::string(oms::ssd::ssd_connector));
    return NULL;
  }
  else
  {
    // Load connector geometry
    pugi::xml_node connectorGeometryNode = node.child(oms::ssd::ssd_connector_geometry);
    if (connectorGeometryNode)
    {
      oms::ssd::ConnectorGeometry geometry(0.0, 0.0);
      geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
      connector->setGeometry(&geometry);
    }
  }

  return connector;
}

oms_status_enu_t oms::Connector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node = root.append_child(oms::ssd::ssd_connector);
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
    return reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(node);
  }
  return oms_status_ok;
}

oms::Connector::Connector(const oms::Connector& rhs)
{
  this->causality = rhs.causality;
  this->type = rhs.type;

  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms::ssd::ConnectorGeometry(*reinterpret_cast<oms::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;
}

oms::Connector& oms::Connector::operator=(const oms::Connector& rhs)
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

  this->setGeometry(reinterpret_cast<oms::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

void oms::Connector::setName(const oms::ComRef& name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms::Connector::setGeometry(const oms::ssd::ConnectorGeometry *newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms::ssd::ConnectorGeometry(*newGeometry));
}

oms::Connector oms::Connector::addPrefix(const oms::ComRef& prefix) const
{
  oms::Connector c(*this);
  c.setName(prefix + c.getName());
  return c;
}

bool oms::operator==(const oms::Connector& v1, const oms::Connector& v2)
{
  return v1.getName() == v2.getName() && v1.type == v2.type && v1.causality == v2.causality;
}

bool oms::operator!=(const oms::Connector& v1, const oms::Connector& v2)
{
  return !(v1 == v2);
}
