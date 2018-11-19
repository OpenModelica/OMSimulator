#include "TLMBusConnector.h"
#include "System.h"
#include "Logging.h"
#include "ssd/Tags.h"
#include <iostream>
#include <cstring>
#include <algorithm>

oms3::TLMBusConnector::TLMBusConnector(const oms3::ComRef &name, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation, System* parentSystem, Component *component)
  : parentSystem(parentSystem), component(component)
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

  variableTypes = TLMBusConnector::getVariableTypes(domain, dimensions, interpolation);
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

  variableTypes = TLMBusConnector::getVariableTypes(domain, dimensions, interpolation);
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

  variableTypes = TLMBusConnector::getVariableTypes(domain, dimensions, interpolation);

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

void oms3::TLMBusConnector::setReal(int i, double value)
{
  if(parentSystem)
    parentSystem->setReal(getConnector(i), value);
  else
    getComponent()->setReal(getConnector(i), value);
}

void oms3::TLMBusConnector::getReal(int i, double &value)
{
  if(parentSystem)
    parentSystem->getReal(getConnector(i), value);
  else
    getComponent()->getReal(getConnector(i), value);
}

void oms3::TLMBusConnector::setReals(std::vector<int> i, std::vector<double> values)
{
  if(parentSystem) {
    for(int j=0; j<i.size(); ++j)
      parentSystem->setReal(getConnector(i[j]),values[j]);
  }
  else {
    for(int j=0; j<i.size(); ++j)
      getComponent()->setReal(getConnector(i[j]),values[j]);
  }
}

void oms3::TLMBusConnector::getReals(std::vector<int> i, std::vector<double> &values)
{
  if(parentSystem) {
    for(int j=0; j<i.size(); ++j)
      parentSystem->getReal(getConnector(i[j]),values[j]);
  }
  else {
    for(int j=0; j<i.size(); ++j)
      getComponent()->getReal(getConnector(i[j]),values[j]);
  }
}

void oms3::TLMBusConnector::setRealInputDerivatives(int i, int order, double value)
{
  logError_NotImplemented;
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
  if(sortedConnectors.empty() && (getActualBus() == this))
    return logError_NoConnectorsInTLMBus(getName());

  //OMTLMSimulator uses degrees of freedom as "dimensions",
  //so convert to this:
  int omtlm_dimensions = dimensions;
  if(dimensions == 2) omtlm_dimensions = 3;
  if(dimensions == 3) omtlm_dimensions = 6;

  //Convert causality to string
  std::string omtlm_causality = "bidirectional";
  if(std::string(domain) == "input")
    omtlm_causality = "input";
  else if(std::string(domain) == "output")
    omtlm_causality = "output";

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
oms3::Component* oms3::TLMBusConnector::getComponent(const ComRef& con, System* system) const
{
  Connection** connections = system->getConnections(ComRef(""));
  for(int i=0; connections[i]; ++i) {
    if(connections[i]->getSignalA() == con) {
      ComRef conref = connections[i]->getSignalB();
      ComRef front = conref.pop_front();
      System* subsystem = system->getSubSystem(front);
      if(subsystem) {
        return getComponent(conref, system->getSubSystem(front));
      }
      return system->getComponent(front);  //Will return NULL if component not found, which is fine
    }
    if(connections[i]->getSignalB() == con) {
      ComRef conref = connections[i]->getSignalA();
      ComRef front = conref.pop_front();
      System* subsystem = system->getSubSystem(front);
      if(subsystem) {
        return getComponent(conref, system->getSubSystem(front));
      }
      return system->getComponent(front);  //Will return NULL if component not found, which is fine
    }
  }

  return nullptr;
}

/**
 * \brief Recursively find component (i.e. FMU) connected to the TLM bus. Return NULL if not found.
 */
oms3::Component* oms3::TLMBusConnector::getComponent()
{
  if(!component && !parentSystem)
    return nullptr;

  if(!component)
    component = getComponent(sortedConnectors[0], parentSystem); //Store pointer for performance reasons

  return component;
}

oms3::TLMBusConnector* oms3::TLMBusConnector::getActualBus()
{
  if(!actualBus && !parentSystem)
    actualBus = this;
  else if(!actualBus)
    actualBus = getActualBus(getName(), parentSystem);

  return actualBus;
}

oms3::TLMBusConnector* oms3::TLMBusConnector::getActualBus(ComRef cref, System *system)
{
  Connection** connections = system->getConnections(ComRef(""));
  for(int i=0; connections[i]; ++i) {
    ComRef subcref = ComRef("");
    if(connections[i]->getSignalA() == cref) {
      subcref = connections[i]->getSignalB();
    }
    else if(connections[i]->getSignalB() == cref) {
      subcref = connections[i]->getSignalA();
    }
    else {
      return this;
    }

    TLMBusConnector* subBus = system->getTLMBusConnector(subcref);
    if(subBus) {
      return subBus->getActualBus();
    }
    ComRef subhead = subcref.pop_front();
    System *subSystem = system->getSubSystem(subhead);
    if(subSystem)
      return subSystem->getTLMBusConnector(subcref)->getActualBus();
    Component *component = system->getComponent(subhead);
    if(component) {
      return component->getTLMBusConnector(subcref)->getActualBus();
    }
  }

  return nullptr; //Should never happen
}

std::vector<std::string> oms3::TLMBusConnector::getVariableTypes(std::string domain, int dimensions, oms_tlm_interpolation_t interpolation)
{
  std::vector<std::string> types;
  if(std::string(domain) == "input" || std::string(domain) == "output") {
    types = { "value" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_no_interpolation) {
    types = { "state", "flow", "effort" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_coarse_grained) {
    types = { "state", "flow", "wave", "impedance" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_fine_grained) {
    types = { "state", "flow",
              "wave1", "wave2", "wave3", "wave4", "wave5", "wave6", "wave7", "wave8", "wave9", "wave10",
              "time1", "time2", "time3", "time4", "time5", "time6", "time7", "time8", "time9", "time10",
              "impedance" };
  }
  else if(dimensions == 3 && interpolation == oms_tlm_no_interpolation) {
    types = { "state1", "state2", "state3",
              "A11","A12","A13","A21","A22","A23","A31","A32","A33",
              "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
              "effort1", "effort2", "effort3", "effort4", "effort5", "effort6"};
  }
  else if(dimensions == 3 && interpolation == oms_tlm_coarse_grained) {
    types = { "state1", "state2", "state3",
              "A11","A12","A13","A21","A22","A23","A31","A32","A33",
              "flow1", "flow2", "flow3", "flow4", "flow5", "flow6",
              "wave1", "wave2", "wave3", "wave4", "wave5", "wave6",
              "linearimpedance", "angularimpedance"};
  }
  else if(dimensions == 3 && interpolation == oms_tlm_fine_grained) {
    types = { "state1", "state2", "state3",
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

  return types;
}

std::vector<std::string> oms3::TLMBusConnector::getVariableDescriptions(std::string domain, int dimensions, oms_tlm_interpolation_t interpolation)
{
  std::string value = "variable";
  std::string state = "state";
  std::string flow = "flow";
  std::string rotflow = "flow";
  std::string effort = "effort";
  std::string roteffort = "effort";
  std::string wave = "wave variable";
  std::string rotwave = "rotational wave";
  std::string impedance = "impedance";
  std::string rotimpedance = "rotational impedance";
  std::string time = "time sample";
  if(domain == "input") {
    value = "input variable";
  }
  else if(domain == "output") {
    value = "output variable";
  }
  else if(domain == "mechanical") {
    state = "position";
    flow = "speed";
    rotflow = "angular speed";
    effort = "force";
    roteffort = "torque";
    wave = "wave";
    rotwave = "rotational wave";
    impedance = "impedance";
    rotimpedance = "rotational impedance";
    time = "time sample";
  }
  else if(domain == "rotational") {
    state = "angle";
    flow = "angular speed";
    effort = "torque";
    wave = "wave";
    impedance = "impedance";
    time = "time sample";
  }
  else if(domain == "hydraulic") {
    state = "volume";
    flow = "volume flow";
    effort = "pressure";
    wave = "wave";
    impedance = "impedance";
    time = "time sample";
  }
  else if(domain == "electric") {
    state = "charge";
    flow = "current";
    effort = "potential";
    wave = "wave";
    impedance = "impedance";
    time = "time sample";
  }

  std::vector<std::string> types;
  if(std::string(domain) == "input" || std::string(domain) == "output") {
    types = { "Value variable" };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_no_interpolation) {
    types = { state, flow, effort };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_coarse_grained) {
    types = { state, flow, wave, impedance };
  }
  else if(dimensions == 1 && interpolation == oms_tlm_fine_grained) {
    types = { state, flow };
    for(int i=1; i<=10; ++i) {
      types.push_back(wave + " at t = time("+std::to_string(i)+")");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(time + " for interpolation");
    }
    types.push_back(impedance);
  }
  else if(dimensions == 3 && interpolation == oms_tlm_no_interpolation) {
    types = { state+" (x-axis)", state+" (y-axis)", state+" (z-axis)"};
    for(int i=1; i<=3; ++i)
      for(int j=1;j<=3; ++j)
        types.push_back("My cool Orientation matrix ["+std::to_string(i)+","+std::to_string(j)+"]");
    types.push_back(flow+" (x-axis)");
    types.push_back(flow+" (y-axis)");
    types.push_back(flow+" (z-axis)");
    types.push_back(rotflow+" (x-axis)");
    types.push_back(rotflow+" (y-axis)");
    types.push_back(rotflow+" (z-axis)");
    types.push_back(effort+" (x-axis)");
    types.push_back(effort+" (y-axis)");
    types.push_back(effort+" (z-axis)");
    types.push_back(roteffort+" (x-axis)");
    types.push_back(roteffort+" (y-axis)");
    types.push_back(roteffort+" (z-axis)");
  }
  else if(dimensions == 3 && interpolation == oms_tlm_coarse_grained) {
    types = { state+" (x-axis)", state+" (y-axis)", state+" (z-axis)"};
    for(int i=1; i<=3; ++i)
      for(int j=1;j<=3; ++j)
        types.push_back("Orientation matrix ["+std::to_string(i)+","+std::to_string(j)+"]");
    types.push_back(flow+" (x-axis)");
    types.push_back(flow+" (y-axis)");
    types.push_back(flow+" (z-axis)");
    types.push_back(rotflow+" (x-axis)");
    types.push_back(rotflow+" (y-axis)");
    types.push_back(rotflow+" (z-axis)");
    types.push_back(wave+" (x-axis)");
    types.push_back(wave+" (y-axis)");
    types.push_back(wave+" (z-axis)");
    types.push_back(rotwave+" (x-axis)");
    types.push_back(rotwave+" (y-axis)");
    types.push_back(rotwave+" (z-axis)");
    types.push_back(impedance);
    types.push_back(rotimpedance);
  }
  else if(dimensions == 3 && interpolation == oms_tlm_fine_grained) {
    types = { state+" (x-axis)", state+" (y-axis)", state+" (z-axis)"};
    for(int i=1; i<=3; ++i)
      for(int j=1;j<=3; ++j)
        types.push_back("Orientation matrix ["+std::to_string(i)+","+std::to_string(j)+"]");
    types.push_back(flow+" (x-axis)");
    types.push_back(flow+" (y-axis)");
    types.push_back(flow+" (z-axis)");
    types.push_back(rotflow+" (x-axis)");
    types.push_back(rotflow+" (y-axis)");
    types.push_back(rotflow+" (z-axis)");
    for(int i=1; i<=10; ++i) {
      types.push_back(wave + " at t = time"+std::to_string(i)+" (x-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(wave + " at t = time"+std::to_string(i)+" (y-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(wave + " at t = time"+std::to_string(i)+" (z-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(rotwave + " at t = time"+std::to_string(i)+" (x-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(rotwave + " at t = time"+std::to_string(i)+" (y-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(rotwave + " at t = time"+std::to_string(i)+" (z-axis)");
    }
    for(int i=1; i<=10; ++i) {
      types.push_back(time + " for interpolation");
    }
    types.push_back(impedance);
    types.push_back(rotimpedance);
  }

  return types;
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
    connectornames[i] = new char[strlen(connector.second.c_str())+1];
    strcpy(connectornames[i], connector.second.c_str());
    connectortypes[i] = new char[strlen(connector.first.c_str())+1];
    strcpy(connectortypes[i], connector.first.c_str());
    ++i;
  }
}
