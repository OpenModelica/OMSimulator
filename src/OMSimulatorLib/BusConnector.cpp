#include "BusConnector.h"
#include "Logging.h"
#include <iostream>
#include <cstring>

oms3::BusConnector::BusConnector(const oms3::ComRef &name)
{
  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());

  this->geometry = NULL;
}

oms3::BusConnector::~BusConnector()
{
  if (this->name) delete[] this->name;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
}

oms_status_enu_t oms3::BusConnector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node bus_node = root.append_child("OMSimulator:Bus");
  bus_node.append_attribute("name") = name;

  pugi::xml_node signals_node = bus_node.append_child("Signals");
  for(auto& connector : connectors) {
    pugi::xml_node signal_node = signals_node.append_child("Signal");
    signal_node.append_attribute("name") = connector.c_str();
  }

  return oms_status_ok;
}

oms3::BusConnector::BusConnector(const oms3::BusConnector &rhs)
{
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;
}

oms3::BusConnector &oms3::BusConnector::operator=(const oms3::BusConnector &rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->name)
    delete[] this->name;
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);

  this->setGeometry(reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry));

  return *this;
}

void oms3::BusConnector::setName(const oms3::ComRef &name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms3::BusConnector::setGeometry(const oms2::ssd::ConnectorGeometry *newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*newGeometry));
}

oms_status_enu_t oms3::BusConnector::addConnector(const oms3::ComRef &cref)
{
  connectors.push_back(cref);
  return oms_status_ok;
}
