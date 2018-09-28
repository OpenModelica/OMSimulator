#include "TLMBusConnector.h"
#include "Logging.h"
#include <iostream>
#include <cstring>
#include <algorithm>

oms3::TLMBusConnector::TLMBusConnector(const oms3::ComRef &name, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
  this->geometry = NULL;
  this->domain = new char[domain.size()+1];
  strcpy(this->domain, domain.c_str());
  this->dimensions = dimensions;
  this->interpolation = interpolation;

  updateVariableTypes();
}

oms3::TLMBusConnector::~TLMBusConnector()
{
  if (this->name) delete[] this->name;
  if (this->domain) delete[] this->domain;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
}

oms_status_enu_t oms3::TLMBusConnector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node bus_node = root.append_child("OMSimulator:Bus");
  bus_node.append_attribute("name") = name;
  bus_node.append_attribute("type") = "tlm";
  bus_node.append_attribute("domain") = domain;
  bus_node.append_attribute("dimensions") = std::to_string(dimensions).c_str();
  if(interpolation == oms_tlm_no_interpolation)
    bus_node.append_attribute("interpolation") = "none";
  else if(interpolation == oms_tlm_coarse_grained)
    bus_node.append_attribute("interpolation") = "coarsegrained";
  else if(interpolation == oms_tlm_fine_grained)
    bus_node.append_attribute("interpolation") = "finegrained";

  pugi::xml_node signals_node = bus_node.append_child("Signals");
  for(auto& connector : connectors) {
    pugi::xml_node signal_node = signals_node.append_child("Signal");
    signal_node.append_attribute("name") = connector.first.c_str();
    signal_node.append_attribute("type") = connector.second.c_str();
  }

  if (this->geometry)
  {
    return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry)->exportToSSD(bus_node);
  }

  return oms_status_ok;
}

oms3::TLMBusConnector::TLMBusConnector(const oms3::TLMBusConnector &rhs)
{
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);
  this->domain = new char[strlen(rhs.domain)+1];
  strcpy(this->domain, rhs.domain);
  this->dimensions = rhs.dimensions;
  this->interpolation = rhs.interpolation;

  if (rhs.geometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry)));
  else
    this->geometry = NULL;

  updateVariableTypes();
}

oms3::TLMBusConnector &oms3::TLMBusConnector::operator=(const oms3::TLMBusConnector &rhs)
{
  // check for self-assignment
  if(&rhs == this)
    return *this;

  if (this->name)
    delete[] this->name;
  this->name = new char[strlen(rhs.name)+1];
  strcpy(this->name, rhs.name);
  this->domain = new char[strlen(rhs.domain)+1];
  strcpy(this->domain, rhs.domain);
  this->dimensions = rhs.dimensions;
  this->interpolation = rhs.interpolation;

  this->setGeometry(reinterpret_cast<oms2::ssd::ConnectorGeometry*>(rhs.geometry));

  updateVariableTypes();

  return *this;
}

void oms3::TLMBusConnector::setName(const oms3::ComRef &name)
{
  if (this->name)
    delete[] this->name;

  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
}

void oms3::TLMBusConnector::setGeometry(const oms2::ssd::ConnectorGeometry *newGeometry)
{
  if (this->geometry)
  {
    delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
    this->geometry = NULL;
  }

  if (newGeometry)
    this->geometry = reinterpret_cast<ssd_connector_geometry_t*>(new oms2::ssd::ConnectorGeometry(*newGeometry));
}

oms_status_enu_t oms3::TLMBusConnector::addConnector(const oms3::ComRef &cref, std::string vartype)
{
  if(std::find(variableTypes.begin(), variableTypes.end(), vartype) == variableTypes.end())
    return logError("Unknown TLM variable type: "+vartype);

  connectors[cref] = vartype;
  return oms_status_ok;
}

void oms3::TLMBusConnector::updateVariableTypes()
{
  if(std::string(domain) == "input" || std::string(domain) == "output") {
    variableTypes = { "value" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_no_interpolation) {
    variableTypes = { "state", "flow", "effort" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_coarse_grained) {
    variableTypes = { "state", "flow", "wave", "impedance" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_fine_grained) {
    variableTypes = { "state", "flow",
                     "wave1", "wave2", "wave3", "wave4", "wave5", "wave6", "wave7", "wave8", "wave9", "wave10",
                     "time1", "time2", "time3", "time4", "time5", "time6", "time7", "time8", "time9", "time10",
                     "impedance" };
  }
  else if(dimensions == 3 && interpolation == oms_tlm_no_interpolation) {
    variableTypes = { "state1", "state2", "state3",
                     "A11","A12","A13","A21","A22","A23","A31","A32","A33",
                     "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
                     "effort1", "effort2", "effort3", "effort4", "effort5", "effort6"};
  }
  else if(dimensions == 3 && interpolation == oms_tlm_coarse_grained) {
    variableTypes = { "state1", "state2", "state3",
                     "A11","A12","A13","A21","A22","A23","A31","A32","A33",
                     "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
                     "wave1", "wave2", "wave3", "wave4", "wave5", "wave6",
                     "impedance1", "impedance2"};
  }
  else if(dimensions == 3 && interpolation == oms_tlm_fine_grained) {
    variableTypes = { "state1", "state2", "state3",
                     "A11","A12","A13","A21","A22","A23","A31","A32","A33",
                     "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
                     "wave1_1",  "wave1_2",  "wave1_3",  "wave1_4",  "wave1_5",  "wave1_6",
                     "wave2_1",  "wave2_2",  "wave2_3",  "wave2_4",  "wave2_5",  "wave2_6",
                     "wave3_1",  "wave3_2",  "wave3_3",  "wave3_4",  "wave3_5",  "wave3_6",
                     "wave4_1",  "wave4_2",  "wave4_3",  "wave4_4",  "wave4_5",  "wave4_6",
                     "wave5_1",  "wave5_2",  "wave5_3",  "wave5_4",  "wave5_5",  "wave5_6",
                     "wave6_1",  "wave6_2",  "wave6_3",  "wave6_4",  "wave6_5",  "wave6_6",
                     "wave7_1",  "wave7_2",  "wave7_3",  "wave7_4",  "wave7_5",  "wave7_6",
                     "wave8_1",  "wave8_2",  "wave8_3",  "wave8_4",  "wave8_5",  "wave8_6",
                     "wave9_1",  "wave9_2",  "wave9_3",  "wave9_4",  "wave9_5",  "wave9_6",
                     "wave10_1", "wave10_2", "wave10_3", "wave10_4", "wave10_5", "wave10_6",
                     "impedance1", "impedance2"};
  }
}
