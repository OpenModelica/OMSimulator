/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "BusConnector.h"
#include "Logging.h"
#include "ssd/Tags.h"
#include <iostream>
#include <cstring>

oms::BusConnector::BusConnector(const oms::ComRef &name)
{
  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->geometry = NULL;
  this->connectors = NULL;
}

oms::BusConnector::~BusConnector()
{
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry);
  if (this->connectors) {
    for (int i=0; connectors[i]; ++i)
      delete[] connectors[i];
    delete[] connectors;
  }
}

oms_status_enu_t oms::BusConnector::exportToSSD(pugi::xml_node& root) const
{
  pugi::xml_node bus_node = root.append_child(oms::ssp::Draft20180219::bus);
  bus_node.append_attribute("name") = name;

  pugi::xml_node signals_node = bus_node.append_child(oms::ssp::Draft20180219::signals);
  for(auto& connector : conrefs)
  {
    pugi::xml_node signal_node = signals_node.append_child(oms::ssp::Draft20180219::signal);
    signal_node.append_attribute("name") = connector.c_str();
  }

  if (this->geometry)
    return reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(bus_node);

  return oms_status_ok;
}

oms::BusConnector::BusConnector(const oms::BusConnector &rhs)
{
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms::ssd::ConnectorGeometry(*reinterpret_cast<oms::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;
}

oms::BusConnector &oms::BusConnector::operator=(const oms::BusConnector &rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->name)
    delete[] this->name;
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  this->setGeometry(reinterpret_cast<oms::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

void oms::BusConnector::setName(const oms::ComRef &name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms::BusConnector::setGeometry(const oms::ssd::ConnectorGeometry *newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms::ssd::ConnectorGeometry(*newGeometry));
}

oms_status_enu_t oms::BusConnector::addConnector(const oms::ComRef &cref)
{
  conrefs.push_back(cref);
  updateConnectors();
  return oms_status_ok;
}

oms_status_enu_t oms::BusConnector::deleteConnector(const oms::ComRef &cref)
{
  for (auto it = conrefs.begin(); it != conrefs.end(); ++it) {
    if (*it == cref) {
      conrefs.erase(it);
      updateConnectors();
      return oms_status_ok;
    }
  }
  return oms_status_error;
}

void oms::BusConnector::updateConnectors()
{
  if (connectors)
  {
    for (int i=0; connectors[i]; ++i)
      delete[] connectors[i];
    delete[] connectors;
  }
  connectors = new char*[conrefs.size()+1];
  connectors[conrefs.size()] = NULL;

  for (int i=0; i<conrefs.size(); ++i) {
    connectors[i] = new char[sizeof(conrefs[i].c_str())+1];
    strcpy(connectors[i], conrefs[i].c_str());
  }
}
