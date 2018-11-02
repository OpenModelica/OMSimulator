#include "TLMBusConnector.h"
#include "System.h"
#include "Logging.h"
#include "ssd/Tags.h"
#include <iostream>
#include <cstring>
#include <algorithm>

oms3::TLMBusConnector::TLMBusConnector(const oms3::ComRef &name, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation, System* parentSystem)
  : parentSystem(parentSystem)
{
  std::string str(name);
  this->name = new char[str.size()+1];
  strcpy(this->name, str.c_str());
  this->geometry = NULL;
  this->domain = new char[domain.size()+1];
  strcpy(this->domain, domain.c_str());
  this->dimensions = dimensions;
  this->interpolation = interpolation;

  if(domain == "input")
    causality = oms_causality_input;
  else if(domain == "output")
    causality = oms_causality_output;
  else
    causality = oms_causality_bidir;

  connectornames = nullptr;
  connectortypes = nullptr;

  updateVariableTypes();
}

oms3::TLMBusConnector::~TLMBusConnector()
{
  if (this->name) delete[] this->name;
  if (this->domain) delete[] this->domain;
  if (this->geometry) delete reinterpret_cast<oms2::ssd::ConnectorGeometry*>(this->geometry);
  if (this->connectornames) {
    for (int i=0; connectornames[i]; ++i)
      delete[] connectornames[i];
    delete[] connectornames;
  }
  if (this->connectortypes) {
    for (int i=0; connectortypes[i]; ++i)
      delete[] connectortypes[i];
    delete[] connectortypes;
  }
}

oms_status_enu_t oms3::TLMBusConnector::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node bus_node = root.append_child(oms::bus);
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

  pugi::xml_node signals_node = bus_node.append_child(oms::signals);
  for(auto& connector : connectors) {
    pugi::xml_node signal_node = signals_node.append_child(oms::signal);
    signal_node.append_attribute("name") = connector.second.c_str();
    signal_node.append_attribute("type") = connector.first.c_str();
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

oms3::ComRef oms3::TLMBusConnector::getConnector(int id) const
{
  return sortedConnectors[id];
}

std::vector<oms3::ComRef> oms3::TLMBusConnector::getConnectors(std::vector<int> ids) const
{
  std::vector<oms3::ComRef> retval;
  for(int id : ids) {
    retval.push_back(sortedConnectors[id]);
  }
  return retval;
}

oms_status_enu_t oms3::TLMBusConnector::addConnector(const oms3::ComRef &cref, std::string vartype)
{
  if(std::find(variableTypes.begin(), variableTypes.end(), vartype) == variableTypes.end())
    return logError("Unknown TLM variable type: "+vartype);

  oms3::ComRef tempRef = cref;
  connectors.insert(std::make_pair(vartype, tempRef));
  updateConnectors();
  sortConnectors();

  return oms_status_ok;
}

oms_status_enu_t oms3::TLMBusConnector::deleteConnector(const oms3::ComRef &cref)
{
  for (auto it = connectors.begin(); it != connectors.end(); ++it) {
    if ((*it).second == cref) {
      connectors.erase(it);
      updateConnectors();
      sortConnectors();
      return oms_status_ok;
    }
  }
  return oms_status_error;
}

void oms3::TLMBusConnector::sortConnectors()
{
  if(variableTypes.size() == connectors.size()) {
    for(const std::string& type : variableTypes) {
      oms3::ComRef name = connectors.find(type)->second;
      sortedConnectors.push_back(std::string(name));
    }
  }
}

oms_status_enu_t oms3::TLMBusConnector::registerToSockets(TLMPlugin *plugin)
{
  if(sortedConnectors.empty())
    return logError("All required connectors not added to TLM bus");

  //OMTLMSimulator uses degrees of freedom as "dimensions",
  //so convert to this:
  int omtlm_dimensions = dimensions;
  if(dimensions == 2) omtlm_dimensions = 3;
  if(dimensions == 3) omtlm_dimensions = 6;

  //Convert causality to string
  std::string omtlm_causality = "Bidirectional";
  if(std::string(domain) == "input")
    omtlm_causality = "Input";
  else if(std::string(domain) == "output")
    omtlm_causality = "Output";

  this->id = plugin->RegisteTLMInterface(name,omtlm_dimensions,omtlm_causality,domain);

  if(this->id < 0) {
    logError("Failed to register TLM interface: "+std::string(name));
    return oms_status_error;
  }

  return oms_status_ok;
}

/**
 * \brief Recurse into specified system and find component connected to conA
 */
oms3::Component *oms3::TLMBusConnector::getComponent(const ComRef& conA, System* system) const
{
  Connection** connections = system->getConnections(ComRef(""));
  for(int i=0; connections[i]; ++i) {
    if(connections[i]->getSignalA() == conA) {
      ComRef conref = connections[i]->getSignalB().front();
      ComRef sysref = conref.pop_front();
      System* subsystem = system->getSubSystem(sysref);
      if(subsystem) {
        return getComponent(conref, system->getSubSystem(sysref));
      }
      return system->getComponent(conref.front());  //Will return NULL if component not found, which is fine
    }
  }

  return NULL;
}

/**
 * \brief Recursively find component (i.e. FMU) connected to the TLM bus. Return NULL if not found.
 */
oms3::Component *oms3::TLMBusConnector::getComponent()
{
  if(!parentSystem)
    return nullptr;

  if(!component)
    component = getComponent(sortedConnectors[0], parentSystem); //Store pointer for performance reasons

  return component;
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
                     "linearimpedance", "angularimpedance"};
  }
  else if(dimensions == 3 && interpolation == oms_tlm_fine_grained) {
    variableTypes = { "state1", "state2", "state3",
                      "A11","A12","A13","A21","A22","A23","A31","A32","A33",
                      "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
                      "wave1_1", "wave2_1", "wave3_1", "wave4_1", "wave5_1", "wave6_1", "wave7_1", "wave8_1", "wave9_1", "wave 10_1",
                      "wave1_2", "wave2_2", "wave3_2", "wave4_2", "wave5_2", "wave6_2", "wave7_2", "wave8_2", "wave9_2", "wave 10_2",
                      "wave1_3", "wave2_3", "wave3_3", "wave4_3", "wave5_3", "wave6_3", "wave7_3", "wave8_3", "wave9_3", "wave 10_3",
                      "wave1_4", "wave2_4", "wave3_4", "wave4_4", "wave5_4", "wave6_4", "wave7_4", "wave8_4", "wave9_4", "wave 10_4",
                      "wave1_5", "wave2_5", "wave3_5", "wave4_5", "wave5_5", "wave6_5", "wave7_5", "wave8_5", "wave9_5", "wave 10_5",
                      "wave1_6", "wave2_6", "wave3_6", "wave4_6", "wave5_6", "wave6_6", "wave7_6", "wave8_6", "wave9_6", "wave 10_6",
                      "time1", "time2", "time3", "time4", "time5", "time6", "time7", "time8", "time9", "time10",
                      "linearimpedance", "angularimpedance"};
  }
}


void oms3::TLMBusConnector::updateConnectors()
{
  if (connectornames)
  {
    for (int i=0; connectornames[i]; ++i)
      delete[] connectornames[i];
    delete[] connectornames;
  }
  if (connectortypes)
  {
    for (int i=0; connectortypes[i]; ++i)
      delete[] connectortypes[i];
    delete[] connectortypes;
  }
  connectornames = new char*[connectors.size()+1];
  connectornames[connectors.size()] = NULL;
  connectortypes = new char*[connectors.size()+1];
  connectortypes[connectors.size()] = NULL;

  int i=0;
  for (const auto connector : connectors) {
    connectornames[i] = new char[sizeof(connector.second.c_str())+1];
    strcpy(connectornames[i], connector.second.c_str());
    connectortypes[i] = new char[sizeof(connector.first.c_str())+1];
    strcpy(connectortypes[i], connector.first.c_str());
    ++i;
  }
}
