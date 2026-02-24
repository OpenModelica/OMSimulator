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

#include "Connector.h"
#include "Logging.h"
#include "OMSString.h"
#include "ssd/Tags.h"
#include "Variable.h"

#include <cstring>

oms::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name, const oms::ComRef& owner)
{
  this->causality = causality;
  this->type = type;

  this->owner = allocateAndCopyString(owner.c_str());
  this->name = allocateAndCopyString(name.c_str());

  this->geometry = NULL;
}

oms::Connector::Connector(oms_causality_enu_t causality, oms_signal_type_enu_t type, const oms::ComRef& name, const oms::ComRef& owner, double height)
{
  this->causality = causality;
  this->type = type;

  this->owner = allocateAndCopyString(owner.c_str());
  this->name = allocateAndCopyString(name.c_str());

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
  if (this->owner) delete[] this->owner;
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry);
}

oms::Connector::Connector(const oms::Connector& rhs)
{
  this->causality = rhs.causality;
  this->type = rhs.type;

  this->owner = allocateAndCopyString(rhs.owner);
  this->name = allocateAndCopyString(rhs.name);

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

  if (this->owner)
    delete[] this->owner;
  this->owner = allocateAndCopyString(rhs.owner);

  if (this->name)
    delete[] this->name;
  this->name = allocateAndCopyString(rhs.name);

  this->setGeometry(reinterpret_cast<oms::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

oms::Connector* oms::Connector::NewConnector(const pugi::xml_node& node, const std::string& sspVersion, const oms::ComRef& owner)
{
  ComRef cref = ComRef(node.attribute("name").as_string());

  std::string causalityString = node.attribute("kind").as_string();
  oms_causality_enu_t causality = oms_causality_undefined;
  if (causalityString == "input")
    causality = oms_causality_input;
  else if (causalityString == "output")
    causality = oms_causality_output;
  else if (causalityString == "parameter")
    causality = oms_causality_parameter;
  else if (causalityString == "calculatedParameter")
    causality = oms_causality_calculatedParameter;
  else
  {
    logWarning("Failed to import connector \"" + std::string(cref) + "\" with causality \"" + causalityString + "\"");
    return NULL;
  }

  // get the types of connectors
  std::string typeString = getTypeString(node, sspVersion);
  oms_signal_type_enu_t type;
  if (typeString == "Real")
    type = oms_signal_type_real;
  else if (typeString == "Integer")
    type = oms_signal_type_integer;
  else if (typeString == "Boolean")
    type = oms_signal_type_boolean;
  else if (typeString == "Enumeration")
    type = oms_signal_type_enum;
  else if (typeString == "String")
    type = oms_signal_type_string;
  // TODO handle "Binary" type for FMI-2.1 see specification
  else
  {
    logWarning("Failed to import connector \"" + std::string(cref) + "\" with type \"" + typeString + "\"");
    return NULL;
  }

  Connector* connector = new Connector(causality, type, cref, owner);
  if (!connector)
    return NULL;

  // Load connector geometry
  pugi::xml_node connectorGeometryNode = node.child(oms::ssp::Draft20180219::ssd::connector_geometry);
  if (connectorGeometryNode)
  {
    oms::ssd::ConnectorGeometry geometry(0.0, 0.0);
    geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
    connector->setGeometry(&geometry);
  }

  return connector;
}

std::string oms::Connector::getTypeString(const pugi::xml_node& node, const std::string& sspVersion)
{
  if (sspVersion == "Draft20180219")
  {
    return node.attribute("type").as_string();
  }
  else if (sspVersion == "1.0")
  {
    for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
    {
      std::string name = it->name();
      if (name == oms::ssp::Version1_0::ssc::real_type)
        return "Real";
      else if(name == oms::ssp::Version1_0::ssc::integer_type)
        return "Integer";
      else if(name == oms::ssp::Version1_0::ssc::boolean_type)
        return "Boolean";
      else if(name == oms::ssp::Version1_0::ssc::string_type)
        return "String";
      else if(name == oms::ssp::Version1_0::ssc::enumeration_type)
        return "Enumeration";
      else if(name == oms::ssp::Version1_0::ssc::binary_type)
        return "Binary";
    }

    logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connector) + ":type Version-1.0");
    return "";
  }

  logError("Unknown SSP version");
  return "";
}

oms_status_enu_t oms::Connector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node = root.append_child(oms::ssp::Draft20180219::ssd::connector);
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
  case oms_causality_calculatedParameter:
    node.append_attribute("kind") = "calculatedParameter";
    break;
  }
  switch (this->type)
  {
  case oms_signal_type_boolean:
    node.append_child(oms::ssp::Version1_0::ssc::boolean_type);
    break;
  case oms_signal_type_enum:
    node.append_child(oms::ssp::Version1_0::ssc::enumeration_type);
    for (const auto & it : this->enumerationName)
      node.child(oms::ssp::Version1_0::ssc::enumeration_type).append_attribute("name") = it.second.c_str();
    break;
  case oms_signal_type_integer:
    node.append_child(oms::ssp::Version1_0::ssc::integer_type);
    break;
  case oms_signal_type_real:
    node.append_child(oms::ssp::Version1_0::ssc::real_type);
    if (!this->connectorUnits.empty())
      for (const auto & it : this->connectorUnits)
        node.child(oms::ssp::Version1_0::ssc::real_type).append_attribute("unit") = it.first.c_str();
    break;
  case oms_signal_type_string:
    node.append_child(oms::ssp::Version1_0::ssc::string_type);
    break;
  default:
    logError("Failed to export " + std::string(oms::ssp::Draft20180219::ssd::connector) + ":Binary-type");
    break;
  }
  if (this->geometry)
  {
    return reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(node);
  }
  return oms_status_ok;
}

void oms::Connector::setName(const oms::ComRef& name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms::Connector::setOwner(const ComRef& owner)
{
  if (this->owner)
    delete[] this->owner;

  std::string str(owner);
  this->owner = new char[str.size()+1];
  strcpy(this->owner, str.c_str());
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

std::string oms::Connector::getTypeString() const
{
  switch (this->type)
  {
  case oms_signal_type_real:
    return std::string("Real");

  case oms_signal_type_integer:
    return std::string("Integer");

  case oms_signal_type_string:
    return std::string("String");

  case oms_signal_type_enum:
    return std::string("Enumeration");

  case oms_signal_type_boolean:
    return std::string("Bool");

  case oms_signal_type_bus:
    return std::string("Bus");

  default:
    return std::string("unknown");
  }
}

std::string oms::Connector::getCausalityString() const
{
  switch (this->causality)
  {
  case oms_causality_input:
    return "input";

  case oms_causality_output:
    return "output";

  case oms_causality_parameter:
    return "parameter";

  case oms_causality_calculatedParameter:
    return "calculatedParameter";

  default:
    return std::string("unknown");
  }
}
