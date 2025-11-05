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
#include "Logging.h"
#include "ssd/ConnectionGeometry.h"
#include "ssd/Tags.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <string>

oms::Connection::Connection(const oms::ComRef& conA, const oms::ComRef& conB, bool suppressUnitConversion, oms_connection_type_enu_t type)
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

  this->suppressUnitConversion = suppressUnitConversion;
}

oms::Connection::~Connection()
{
  if (this->conA) delete[] this->conA;
  if (this->conB) delete[] this->conB;
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

  this->suppressUnitConversion = rhs.suppressUnitConversion;
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

  this->suppressUnitConversion = rhs.suppressUnitConversion;

  return *this;
}

oms_status_enu_t oms::Connection::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node node;
  if(type == oms_connection_single)
    node = root.append_child(oms::ssp::Draft20180219::ssd::connection);
  else if(type == oms_connection_bus)
    node = root.append_child(oms::ssp::Draft20180219::bus_connection);

  ComRef startConnectorRef(conA);
  ComRef startElementRef = startConnectorRef.pop_front();
  ComRef endConnectorRef(conB);
  ComRef endElementRef = endConnectorRef.pop_front();

  node.append_attribute("startElement") = startConnectorRef.isEmpty() ? "" : startElementRef.c_str();
  node.append_attribute("startConnector") = startConnectorRef.isEmpty() ? startElementRef.c_str() : startConnectorRef.c_str();
  node.append_attribute("endElement") = endConnectorRef.isEmpty() ? "" : endElementRef.c_str();
  node.append_attribute("endConnector") = endConnectorRef.isEmpty() ? endElementRef.c_str() : endConnectorRef.c_str();

  if (suppressUnitConversion)
    node.append_attribute("suppressUnitConversion") = suppressUnitConversion;

  getGeometry()->exportToSSD(node);

  return oms_status_ok;
}

void oms::Connection::setGeometry(const oms::ssd::ConnectionGeometry* newGeometry, bool inverse)
{
  oms::ssd::ConnectionGeometry* geometry_ = reinterpret_cast<oms::ssd::ConnectionGeometry*>(this->geometry);
  if (geometry_)
    delete geometry_;
  geometry_ = new oms::ssd::ConnectionGeometry(*newGeometry, inverse);
  this->geometry = reinterpret_cast<ssd_connection_geometry_t*>(geometry_);
}

bool oms::Connection::isStrictEqual(const oms::ComRef& signalA, const oms::ComRef& signalB) const
{
  return signalA == oms::ComRef(this->conA) && signalB == oms::ComRef(this->conB);
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

bool oms::Connection::containsSignal(const oms::ComRef& signal) const
{
  return signal.isRootOf(oms::ComRef(this->conA)) || signal.isRootOf(oms::ComRef(this->conB));
}

bool oms::Connection::containsSignalB(const oms::ComRef& signal) const
{
  return signal.isRootOf(oms::ComRef(this->conB));
}

bool oms::Connection::isValid(const ComRef& crefA, const ComRef& crefB, const Connector& conA, const Connector& conB)
{
  bool connectorA, connectorB;

  // Check connector A
  if (crefA.isValidIdent()) // this is a system
  {
    // Connector A of a systems must be input or parameter
    connectorA = conA.isInput() || conA.isParameter();
  }
  else // this is an element
  {
    // Connector A of an element must be output, calculated parameter, or inout
    // TODO: check for inout, neither FMI-1.0 nor FMI-2.0 do support inout
    connectorA = conA.isOutput() || conA.isCalculatedParameter();
  }

  // Check connector B
  if (crefB.isValidIdent()) // this is a system
  {
    // Connector B of a systems must be output or calculated parameter
    connectorB = conB.isOutput() || conB.isCalculatedParameter();
  }
  else // this is an element
  {
    // Connector A of an element must be input, parameter, or inout
    // TODO: check for inout, neither FMI-1.0 nor FMI-2.0 do support inout
    connectorB = conB.isParameter() || conB.isInput();
  }

  // both connectors must be valid in order to make the connection valid
  return connectorA && connectorB;
}

/*! Checks the validity of connections based on export connector names it is possible that connections
    are valid even though the causality check based on SSP-1.0 fails. This is the case when connecting
    system connectors to element connectors based on export names and how the decomposition of the top
    level system is done.
*/
bool oms::Connection::isValidExportConnectorName(const Connector& conA, const Connector& conB)
{
  bool connectorA, connectorB;
  // Check connector A
  if (ComRef(conA.getExportName()).isValidIdent()) // this is a system
  {
    // Connector A of a systems must be input or parameter
    connectorA = conA.isInput() || conA.isParameter();
  }
  else // this is an element
  {
    // Connector A of an element must be output, calculated parameter, or inout
    // TODO: check for inout, neither FMI-1.0 nor FMI-2.0 do support inout
    connectorA = conA.isOutput() || conA.isCalculatedParameter();
  }

  // Check connector B
  if (ComRef(conB.getExportName()).isValidIdent()) // this is a system
  {
    // Connector B of a systems must be output or calculated parameter
    connectorB = conB.isOutput() || conB.isCalculatedParameter();
  }
  else // this is an element
  {
    // Connector A of an element must be input, parameter, or inout
    // TODO: check for inout, neither FMI-1.0 nor FMI-2.0 do support inout
    connectorB = conB.isParameter() || conB.isInput();
  }

  // both connectors must be valid in order to make the connection valid
  return connectorA && connectorB;
}

bool oms::Connection::isValidUnits(const ComRef& crefA, const ComRef& crefB, const Connector& conA, const Connector& conB)
{
  if (conA.connectorUnits.empty() || conB.connectorUnits.empty())
    return true;

  std::map<std::string, std::string> connectorAUnits, connectorBUnits;

  getSIUnits(conA, connectorAUnits);
  getSIUnits(conB, connectorBUnits);

  if (connectorAUnits != connectorBUnits)
    return false;

  return true;
}

void oms::Connection::getSIUnits(const Connector& connector, std::map<std::string, std::string>& baseUnits)
{
  std::vector<std::string> SIunits = {"kg", "m", "s", "A", "K", "mol", "cd", "rad"};
  for (const auto & unit : SIunits)
  {
    for (const auto & it: connector.connectorUnits)
    {
      auto baseUnit = it.second.find(unit);
      if (baseUnit != it.second.end())
        baseUnits[baseUnit->first] = baseUnit->second;
      else
        baseUnits[unit] = "0";
    }
  }
}

oms_status_enu_t oms::Connection::rename(const ComRef& cref, const ComRef& newCref)
{
  oms::ComRef tailA(getSignalA());
  const oms::ComRef frontA = tailA.pop_front();

  if (frontA == cref)
  {
    std::string str(newCref + tailA);
    delete [] this->conA;
    this->conA = new char[str.size()+1];
    strcpy(this->conA, str.c_str());
  }

  oms::ComRef tailB(getSignalB());
  const oms::ComRef frontB = tailB.pop_front();

  if (frontB == cref)
  {
    std::string str(newCref + tailB);
    delete [] this->conB;
    this->conB = new char[str.size()+1];
    strcpy(this->conB, str.c_str());
  }

  return oms_status_ok;
}
