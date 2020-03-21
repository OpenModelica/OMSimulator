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

#include "SystemTLM.h"

#include "Component.h"
#include "Flags.h"
#include "Model.h"
#include "OMTLMSimulatorLib/OMTLMSimulatorLib.h"
#include "ssd/Tags.h"
#include "SystemWC.h"
#include "Types.h"

#include <thread>
#include <algorithm>

oms::SystemTLM::SystemTLM(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms::System(cref, oms_system_tlm, parentModel, parentSystem, oms_solver_none)
{
  logTrace();
  model = omtlm_newModel(cref.c_str());

  omtlm_setLogLevel(model, TLMLogLevel::Fatal);
  if(Log::getLoggingLevel() == 1)
    omtlm_setLogLevel(model, TLMLogLevel::Info);
  else if(Log::getLoggingLevel() == 2)
    omtlm_setLogLevel(model, TLMLogLevel::Debug);
}

oms::SystemTLM::~SystemTLM()
{
  omtlm_unloadModel(model);
}

oms::System* oms::SystemTLM::NewSystem(const oms::ComRef& cref, oms::Model* parentModel, oms::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError_InternalError;
    return NULL;
  }

  System* system = new SystemTLM(cref, parentModel, parentSystem);
  return system;
}

oms_status_enu_t oms::SystemTLM::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms::ssp::Draft20180219::ssd::simulation_information);
  //pugi::xml_node node_simulation_information = node.append_child(oms::ssd::ssd_simulation_information);

  pugi::xml_node node_annotations = node_simulation_information.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node node_annotations = node.append_child(oms::ssd::ssd_annotations);

  pugi::xml_node node_annotation = node_annotations.append_child(oms::ssp::Draft20180219::ssd::annotation);
  node_annotation.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;

  pugi::xml_node node_tlm = node_annotation.append_child(oms::ssp::Draft20180219::tlm_master);
  pugi::xml_node node_simulation_information = node_annotation.append_child(oms::simulation_information);

  pugi::xml_node node_tlm = node_simulation_information.append_child(oms::tlm_master);
  node_tlm.append_attribute("ip") = address.c_str();
  node_tlm.append_attribute("managerport") = std::to_string(desiredManagerPort).c_str();
  node_tlm.append_attribute("monitorport") = std::to_string(desiredMonitorPort).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  pugi::xml_node annotationsNode = node.child(oms::ssp::Draft20180219::ssd::annotations);

  /*  To handle version = "Draft20180219"*/
  if(annotationsNode) {
    pugi::xml_node annotationNode = annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation);
    if(annotationNode && std::string(annotationNode.attribute("type").as_string()) == "org.openmodelica") {
      importFromSSD_SimulationInformationHelper(annotationNode);
    }
  }
  else
  {
    // for version = 1.0
    importFromSSD_SimulationInformationHelper(node);
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::importFromSSD_SimulationInformationHelper(const pugi::xml_node & node)
{
  pugi::xml_node tlmmasterNode = node.child("oms:TlmMaster");
  for (auto it = tlmmasterNode.attributes_begin(); it != tlmmasterNode.attributes_end(); ++it)
   {
     std::string name = it->name();
     if (name == "ip")
       this->address = it->value();
     else if(name == "managerport")
       this->desiredManagerPort = tlmmasterNode.attribute("managerport").as_int();
     else if(name == "monitorport")
       this->desiredMonitorPort = tlmmasterNode.attribute("monitorport").as_int();
   }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::instantiate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::initialize()
{
  omtlm_setLogStepSize(model, getModel()->getLoggingInterval());

  actualManagerPort = desiredManagerPort;
  actualMonitorPort = desiredMonitorPort;
#ifndef _WIN32
  omtlm_checkPortAvailability(&actualManagerPort);
  omtlm_checkPortAvailability(&actualMonitorPort);
#endif

  omtlm_setAddress(model, address);
  omtlm_setManagerPort(model, actualManagerPort);
  omtlm_setMonitorPort(model, actualMonitorPort);

  for(const auto& subsystem : this->getSubSystems()) {
    if(!singleModel.empty() && singleModel != subsystem.second->getCref().c_str()) {
        continue;
    }
    omtlm_addSubModel(model, subsystem.second->getCref().c_str(),"","none");
    for(int i=0; subsystem.second->getTLMBusConnectors()[i] && (singleModel==""); ++i) {
      TLMBusConnector *tlmbus = subsystem.second->getTLMBusConnectors()[i];

      std::string causality = "input";
      if(tlmbus->getCausality() == oms_causality_output)
        causality = "output";
      else if(tlmbus->getCausality() == oms_causality_bidir)
        causality = "bidirectional";

      //OMTLMSimulator uses degrees of freedom as "dimensions",
      //so convert to this:
      int dimensions = tlmbus->getDimensions();
      if(dimensions == 2) dimensions = 3;
      if(dimensions == 3) dimensions = 6;

      omtlm_addInterface(model, subsystem.second->getCref().c_str(), tlmbus->getName().c_str(),dimensions, causality.c_str(), tlmbus->getDomainString().c_str());
    }
  }

  for(const auto& component : this->getComponents()) {
    if(component.second->getType() != oms_component_external)
      continue;

    if(!singleModel.empty() && singleModel != component.second->getCref().c_str()) {
        continue;
    }

    ExternalModel* externalmodel = reinterpret_cast<ExternalModel*>(component.second);

    //Copy external model file to temporary directory
    std::string tempModelPath = getModel()->getTempDirectory()+"/"+std::string(externalmodel->getCref());

  #ifdef WIN32
    std::string cmd = "mkdir \""+tempModelPath+"\" 2> NUL";
    system(cmd.c_str());
    std::string externalModelPath = externalmodel->getPath();
    std::replace(externalModelPath.begin(), externalModelPath.end(), '/', '\\');
    cmd = "copy \""+externalModelPath+"\" \""+tempModelPath;
    system(cmd.c_str());
  #else
    std::string cmd = "mkdir -p \""+tempModelPath+"\"";
    system(cmd.c_str());
    cmd = "cp \""+externalmodel->getPath()+"\" \""+tempModelPath+"\"";
    system(cmd.c_str());
  #endif

    //Extract file name from path
    size_t i1 = externalmodel->getPath().rfind('/', externalmodel->getPath().length());
    size_t i2 = externalmodel->getPath().rfind('\\', externalmodel->getPath().length());
    size_t i = i1;
    if(i2>i1) {
      i = i2;   //We cannot use std::max with MSVC for some reason
    }
    if(i1 == std::string::npos) {
      i = i2;
    }
    else if(i2 == std::string::npos) {
      i = i1;
    }
    std::string fileName = externalmodel->getPath().substr(i+1, externalmodel->getPath().length() - i);
    tempModelPath = tempModelPath+"/"+fileName;

    //Registser external model for OMTLMSimulator
    omtlm_addSubModel(model, externalmodel->getCref().c_str(), tempModelPath.c_str(), externalmodel->getStartScript().c_str());

    for(int i=0; component.second->getTLMBusConnectors()[i]; ++i) {
      TLMBusConnector *tlmbus = component.second->getTLMBusConnectors()[i];

      std::string causality = "input";
      if(tlmbus->getCausality() == oms_causality_output)
        causality = "output";
      else if(tlmbus->getCausality() == oms_causality_bidir)
        causality = "bidirectional";

      //OMTLMSimulator uses degrees of freedom as "dimensions",
      //so convert to this:
      int dimensions = tlmbus->getDimensions();
      if(dimensions == 2) dimensions = 3;
      if(dimensions == 3) dimensions = 6;

      omtlm_addInterface(model, component.second->getCref().c_str(), tlmbus->getName().c_str(),dimensions, causality.c_str(), tlmbus->getDomainString().c_str());
    }
  }

  Connection** connections = this->getConnections(ComRef(""));
  for(int i=0; connections[i]; ++i) {
    oms_tlm_connection_parameters_t* tlmpars = connections[i]->getTLMParameters();
    omtlm_addConnection(model,connections[i]->getSignalA().c_str(),connections[i]->getSignalB().c_str(),tlmpars->delay,tlmpars->linearimpedance,tlmpars->angularimpedance,tlmpars->alpha);
  }

  logStep = (getModel()->getLoggingInterval() > 0) ? getModel()->getLoggingInterval() : 1e-3;
  nextLogTime = getModel()->getStartTime()+logStep;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::reset()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->reset())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  omtlm_setStartTime(model, getModel()->getStartTime());
  omtlm_setStopTime(model, stopTime);

  if(getSubSystems().empty() && getComponents().empty())
    logWarning("oms::TLMCompositeModel::stepUntil: Simulating empty model...");

  logInfo("Starting TLM manager in new thread");

  std::thread *masterThread = new std::thread(&omtlm_simulate, model);

  logInfo("Connecting submodels to manager (sequential)");
  std::string server = address + ":" + std::to_string(actualManagerPort);
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System* subsystem = it->second;
    ComRef syscref = subsystem->getCref();
    connectToSockets(syscref, server);
  }
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    if(find(connectedsubsystems.begin(), connectedsubsystems.end(), it->second->getCref()) == connectedsubsystems.end())
      return logError("Failed to connect TLM subsystem: "+std::string(it->second->getCref()));
  }

  logInfo("Initializing TLM submodels (threaded)");
  std::vector<std::thread> fmiInitializeThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System *subsystem = it->second;
    ComRef syscref = subsystem->getCref();
    fmiInitializeThreads.push_back(std::thread(&oms::SystemTLM::initializeSubSystem, this, syscref));
  }
  for(auto &thread : fmiInitializeThreads)
    thread.join();
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    if(find(initializedsubsystems.begin(), initializedsubsystems.end(), it->second->getCref()) == initializedsubsystems.end())
      return logError("Failed to initialize TLM subsystem: "+std::string(it->second->getCref()));
  }

  logInfo("Simulating TLM submodels (threaded).");
  std::vector<std::thread> fmiSimulateThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System* subsystem = it->second;
    ComRef syscref = subsystem->getCref();
    fmiSimulateThreads.push_back(std::thread(&oms::SystemTLM::simulateSubSystem, this, syscref, stopTime));
  }
  for(auto &thread : fmiSimulateThreads)
    thread.join();

  masterThread->join();
  delete masterThread;

  logInfo("Disconnecting submodels from manager (threaded)");
  std::vector<std::thread> fmiDisconnectThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System* subsystem = it->second;
    fmiDisconnectThreads.push_back(std::thread(&oms::SystemTLM::disconnectFromSockets, this, subsystem->getCref()));
  }
  for(auto &thread : fmiDisconnectThreads)
    thread.join();

  logInfo("Simulation of TLM composite model "+std::string(getCref())+" complete.");

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::connectToSockets(const oms::ComRef cref, std::string server)
{
  logInfo("Starting TLM simulation thread for model "+std::string(cref));

  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(cref));
  if(system == nullptr)
    return logError_SubSystemNotInSystem(getCref(),cref);

  //Limit communication interval to half TLM delay
  //This is for avoiding extrapolation when running asynchronously.

  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for (int i=0; tlmbuses[i]; ++i)
  {
      if(system->getMaximumStepSize() > tlmbuses[i]->getDelay()*0.5) {
        system->setFixedStepSize(tlmbuses[i]->getDelay()*0.5);
        logInfo("Limiting stepSize for "+std::string(getCref())+"."+std::string(tlmbuses[i]->getName())+" to "+std::to_string(system->getMaximumStepSize()));
      }
  }

  logInfo("Creating TLM plugin instance for "+std::string(cref));

  TLMPlugin* plugin = TLMPlugin::CreateInstance();
  pluginsMutex.lock();
  plugins[system] = plugin;

  logInfo("Initializing plugin for "+std::string(cref));

  if(!plugin->Init(std::string(cref),
                   getModel()->getStartTime(),
                   1, //Unused argument anyway
                   system->getMaximumStepSize(),
                   server)) {
    logError("Error initializing the TLM plugin for "+std::string(cref));
    pluginsMutex.unlock();
    return oms_status_error;
  }
  pluginsMutex.unlock();

  logInfo("Registering interfaces for "+std::string(cref));

  for (int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i]->getActualBus();
    oms_status_enu_t status = bus->registerToSockets(plugin);
    if(status == oms_status_error) {
      return logError("Failed to register interface "+std::string(bus->getName()));
    }
  }

  setConnectedMutex.lock();
  connectedsubsystems.push_back(cref);
  setConnectedMutex.unlock();

  return oms_status_ok;
}

void oms::SystemTLM::disconnectFromSockets(const oms::ComRef cref)
{
  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(cref));
  if(system != nullptr)
  {
    //Wait for close permission, to prevent socket from being
    //destroyed before master has read all data
    TLMPlugin *plugin = plugins.find(system)->second;
    delete plugin;
    pluginsMutex.lock();
    plugins[system] = nullptr;
    pluginsMutex.unlock();
  }
}

oms_status_enu_t oms::SystemTLM::setSocketData(const std::string &address, int managerPort, int monitorPort)
{
  if (!getModel()->validState(oms_modelState_virgin))
    return logError_ModelInWrongState(this);

  this->address = address;
  this->desiredManagerPort = managerPort;
  this->desiredMonitorPort = monitorPort;
  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::setPositionAndOrientation(const oms::ComRef &cref, std::vector<double> x, std::vector<double> A)
{
  ComRef tail = cref;
  ComRef head = tail.pop_front();

  if(getSubSystems().find(head) == getSubSystems().end() &&
     getComponents().find(head) == getComponents().end()) {
    return logError("Sub-model \""+std::string(head)+"\" not found.");
  }
  std::string ifcname;
  if(tail.isEmpty()) {
    ifcname = std::string(head);   //Apply to component
  }
  else {
    ifcname = std::string(head)+"."+std::string(tail);  //Apply to interface
  }
  omtlm_setInitialPositionAndOrientation(model, ifcname.c_str(), x, A);
  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::fetchInterfaces(const oms::ComRef &cref,
                                                 std::vector<std::string> &names,
                                                 std::vector<int> &dimensions,
                                                 std::vector<std::string> &domains)
{
    if(getComponents().find(cref) == getComponents().end()) {
      return logError("Sub-model \""+std::string(cref)+"\" not found.");
    }
    singleModel = cref.c_str();
    initialize();
    singleModel = "";
    omtlm_fetchInterfaces(model, cref.c_str());

    //Read interface data from interfaceData.xml (generated above)
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("interfaceData.xml");
    if(!result) {
        return logError("reading interface data from interfaceData.xml failed");
    }

    pugi::xml_node modelDataNode = doc.child("ModelData");
    if(!modelDataNode) {
        return logError("\"ModelData\" node not found in interfaceData.xml");
    }

    pugi::xml_node interfacesNode = modelDataNode.child("Interfaces");
    if(!interfacesNode) {
        return logError("\"Interfaces\" node not found in interfaceData.xml");
    }

    for (pugi::xml_node interfaceNode = interfacesNode.child("Interface"); interfaceNode; interfaceNode = interfaceNode.next_sibling("Interface"))
    {
        names.push_back(interfaceNode.attribute("Name").value());
        dimensions.push_back(interfaceNode.attribute("Dimensions").as_int());
        domains.push_back(interfaceNode.attribute("Domain").value());
    }

    return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::updateInitialValues(const oms::ComRef cref)
{
  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(cref));
  if(system == nullptr)
    return logError_SubSystemNotInSystem(getCref(),cref);
  if (std::find(connectedsubsystems.begin(), connectedsubsystems.end(), cref) == connectedsubsystems.end())
    return logError("System not connected to TLM sockets: "+std::string(cref));
  TLMPlugin* plugin = plugins.find(system)->second;

  //Apply initial values for signal and effort
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for (int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    bus = bus->getActualBus();

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_input) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      bus->getReal(tlmrefs.y, value);
      plugin->SetInitialValue(bus->getId(), value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_1d_cg_t tlmrefs;
      double effort,flow;
      bus->getReal(tlmrefs.c, effort);
      bus->getReal(tlmrefs.v, flow);
      plugin->SetInitialForce1D(bus->getId(), effort);
      plugin->SetInitialFlow1D(bus->getId(), flow);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_1d_fg_t tlmrefs;
      double effort,flow;
      bus->getReal(tlmrefs.c[0], effort);
      bus->getReal(tlmrefs.v, flow);
      plugin->SetInitialForce1D(bus->getId(), effort);
      plugin->SetInitialFlow1D(bus->getId(), flow);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      bus->getReals(tlmrefs.f, effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      bus->getReals(flowrefs, flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_3d_cg_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      bus->getReals(tlmrefs.c, effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      bus->getReals(flowrefs, flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_3d_fg_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      bus->getReals(tlmrefs.c[0], effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      bus->getReals(flowrefs, flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemTLM::initializeSubSystem(oms::ComRef cref)
{
  oms_status_enu_t status = getSubSystem(cref)->initialize();
  if(oms_status_ok != status)
    return status;
  status = updateInitialValues(cref);
  if(oms_status_ok == status) {
    setInitializedMutex.lock();
    initializedsubsystems.push_back(cref);
    setInitializedMutex.unlock();
  }
  return status;
}

oms_status_enu_t oms::SystemTLM::simulateSubSystem(oms::ComRef cref, double stopTime)
{
  oms_status_enu_t status = getSubSystem(cref)->stepUntil(stopTime, NULL);
  plugins[getSubSystem(cref)]->AwaitClosePermission();
  return status;
}

void oms::SystemTLM::writeToSockets(SystemWC *system, double time, Component* component)
{
  socketMutexes[system].lock();

  TLMPlugin *plugin = plugins.find(system)->second;
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for(int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    bus = bus->getActualBus();  //Communicate with actual bus
    int id = bus->getId();

    if(component && component != bus->getComponent()) {
      continue; //Ignore FMUs not specified in vector
    }
    component = bus->getComponent();

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_output) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      bus->getReal(tlmrefs.y, value);
      plugin->SetValueSignal(id, time, value);
      sendValueToLogger(busLogIds[bus], time, value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir) {
      oms_tlm_sigrefs_1d_t tlmrefs;
      double state, flow, force;
      bus->getReal(tlmrefs.x, state);
      bus->getReal(tlmrefs.v, flow);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      plugin->GetForce1D(id, time, flow, &force);

      //Send the resulting motion back to master
      plugin->SetMotion1D(id, time, state, flow);
      sendValueToLogger(busLogIds[bus], time, state);
      sendValueToLogger(busLogIds[bus]+1, time, flow);
      sendValueToLogger(busLogIds[bus]+2, time, force);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir) {

      oms_tlm_sigrefs_3d_t tlmrefs;

      std::vector<double> x(3,0);
      std::vector<double> A(9,0);
      std::vector<double> v(3,0);
      std::vector<double> w(3,0);
      std::vector<double> f(6,0);

      bus->getReals(tlmrefs.x, x);
      bus->getReals(tlmrefs.A, A);
      bus->getReals(tlmrefs.v, v);
      bus->getReals(tlmrefs.w, w);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      plugin->GetForce3D(id, time, &x[0], &A[0], &v[0], &w[0], &f[0]);

      //Send the resulting motion back to master
      plugin->SetMotion3D(id, time, &x[0], &A[0], &v[0], &w[0]);

      sendValueToLogger(busLogIds[bus], time, x[0]);
      sendValueToLogger(busLogIds[bus]+1, time, x[1]);
      sendValueToLogger(busLogIds[bus]+2, time, x[2]);
      sendValueToLogger(busLogIds[bus]+3, time, A[0]);
      sendValueToLogger(busLogIds[bus]+4, time, A[1]);
      sendValueToLogger(busLogIds[bus]+5, time, A[2]);
      sendValueToLogger(busLogIds[bus]+6, time, A[3]);
      sendValueToLogger(busLogIds[bus]+7, time, A[4]);
      sendValueToLogger(busLogIds[bus]+8, time, A[5]);
      sendValueToLogger(busLogIds[bus]+9, time, A[6]);
      sendValueToLogger(busLogIds[bus]+10, time, A[7]);
      sendValueToLogger(busLogIds[bus]+11, time, A[8]);
      sendValueToLogger(busLogIds[bus]+12, time, v[0]);
      sendValueToLogger(busLogIds[bus]+13, time, v[1]);
      sendValueToLogger(busLogIds[bus]+14, time, v[2]);
      sendValueToLogger(busLogIds[bus]+15, time, w[0]);
      sendValueToLogger(busLogIds[bus]+16, time, w[1]);
      sendValueToLogger(busLogIds[bus]+17, time, w[2]);
      sendValueToLogger(busLogIds[bus]+18, time, f[0]);
      sendValueToLogger(busLogIds[bus]+19, time, f[1]);
      sendValueToLogger(busLogIds[bus]+20, time, f[2]);
      sendValueToLogger(busLogIds[bus]+21, time, f[3]);
      sendValueToLogger(busLogIds[bus]+22, time, f[4]);
      sendValueToLogger(busLogIds[bus]+23, time, f[5]);
    }
  }

  sendValuesToLogger(system, time);

  socketMutexes[system].unlock();
}

void oms::SystemTLM::readFromSockets(SystemWC* system, double time, Component* component)
{
  socketMutexes[system].lock();

  TLMPlugin *plugin = plugins.find(system)->second;
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for(int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    bus = bus->getActualBus();  //Communicate with actual bus
    int id = bus->getId();

    if(component && component != bus->getComponent()) {
      continue; //Ignore FMUs not specified in vector
    }
    component = bus->getComponent();

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_input) {
      oms_tlm_sigrefs_signal_t tlmrefs;

      double value;
      plugin->GetValueSignal(id, time, &value);
      bus->setReal(tlmrefs.y, value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_1d_t tlmrefs;

      double flow,effort;

      //Read position and speed from FMU
      bus->getReal(tlmrefs.v, flow);

      //Get interpolated force
      plugin->GetForce1D(id, time, flow, &effort);

      if(bus->getDomain() != oms_tlm_domain_hydraulic) {
          effort = -effort;
      }

      //Write force to FMU
      bus->setReal(tlmrefs.f, effort);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_1d_cg_t tlmrefs;

      double impedance, wave;
      plugin->GetWaveImpedance1D(id, time, &impedance, &wave);
      bus->setReal(tlmrefs.c, wave);
      bus->setReal(tlmrefs.Z, impedance);

      double impedance2, wave2;
      plugin->GetWaveImpedance1D(id, time+system->getMaximumStepSize(), &impedance2, &wave2);

      double dWave = (wave2-wave)/system->getMaximumStepSize();

      bus->setRealInputDerivatives(tlmrefs.c, 1, dWave);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_1d_fg_t tlmrefs;

      double wave;
      double impedance;

      double t = time;
      for(size_t i=0; i<10; ++i) {
        plugin->GetWaveImpedance1D(id, t, &impedance, &wave);
        t += system->getMaximumStepSize()/9;

        bus->setReal(tlmrefs.c[i], wave);
        bus->setReal(tlmrefs.t[i], t);
      }

      bus->setReal(tlmrefs.Z, impedance);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {

      oms_tlm_sigrefs_3d_t tlmrefs;

      std::vector<double> x(3,0); //Dummy, GetForce3D needs it but does not use it
      std::vector<double> A(9,0); //Dummy
      std::vector<double> v(3,0);
      std::vector<double> w(3,0);
      std::vector<double> f(6,0);

      //Read position and speed from FMU
      bus->getReals(tlmrefs.v, v);
      bus->getReals(tlmrefs.w, w);

      //Get interpolated force
      plugin->GetForce3D(id, time,&x[0], &A[0], &v[0], &w[0], &f[0]);

      for(size_t i=0; i<6; ++i) {
        f[i] = -f[i];
      }

      //Write force to FMU
      bus->setReals(tlmrefs.f, f);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_3d_cg_t tlmrefs;
      std::vector<double> waves(6,0);
      double Zt, Zr;
      plugin->GetWaveImpedance3D(id, time, &Zt, &Zr, &waves[0]);
      bus->setReals(tlmrefs.c, waves);
      bus->setReal(tlmrefs.Zt, Zt);
      bus->setReal(tlmrefs.Zr, Zr);

      std::vector<double> waves2(6,0);
      double Zt2, Zr2;
      plugin->GetWaveImpedance3D(id, time+system->getMaximumStepSize(), &Zt2, &Zr2, &waves2[0]);

      std::vector<double> dWaves(6,0);
      for(size_t i=0; i<6; ++i) {
        double dWave = (waves2[i]-waves[i])/system->getMaximumStepSize();
        bus->setRealInputDerivatives(tlmrefs.c[i], 1, dWave);
      }
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_3d_fg_t tlmrefs;

      std::vector<double> waves(6,0);
      double Zt,Zr;

      double t = time;

      for(size_t i=0; i<10; ++i) {
        plugin->GetWaveImpedance3D(id, t, &Zt, &Zr, &waves[0]);
        t += system->getMaximumStepSize()/9;

        bus->setReals(tlmrefs.c[i], waves);
        bus->setReal(tlmrefs.t[i], t);
      }

      bus->setReal(tlmrefs.Zt, Zt);
      bus->setReal(tlmrefs.Zr, Zr);
    }
  }

  socketMutexes[system].unlock();
}


void oms::SystemTLM::sendValueToLogger(int varId, double time, double value)
{
  logMutex.lock();
  logBuffer[varId].push_back(std::make_pair(time, value));

  double tmax = 1e20;
  for(auto& varbuffer : logBuffer) {
    if(varbuffer.second.back().first < tmax) {
      tmax = varbuffer.second.back().first;
    }
  }

  if(tmax >= nextLogTime && logBuffer.size() == numLogVars)
  {
    logTime = nextLogTime;
    getModel()->emit(nextLogTime);
    nextLogTime += logStep;
  }

  logMutex.unlock();
}

oms_status_enu_t oms::SystemTLM::registerSignalsForResultFile(ResultWriter& resultFile)
{
  if (Flags::WallTime())
    clock_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$wallTime")), "wall-clock time [s]", SignalType_REAL);
  else
    clock_id = 0;

  for(auto& system : getSubSystems())
  {
    TLMBusConnector** tlmbuses = system.second->getTLMBusConnectors();
    for(int i=0; tlmbuses[i]; ++i)
    {
      TLMBusConnector* bus = tlmbuses[i];
      bus = bus->getActualBus();  //Communicate with actual bus
      if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_output) {
        busLogIds[bus] = registerLogVariable();
        resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName()), "", SignalType_REAL);
      }
      else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir) {
        std::string statename,flowname,effortname;
        if(bus->getDomain() == oms_tlm_domain_mechanical) {
          statename = "x";
          flowname =  "v";
          effortname = "f";
        }
        else if(bus->getDomain() == oms_tlm_domain_electric) {
          statename = "phi";
          flowname =  "w";
          effortname = "T";
        }
        else if(bus->getDomain() == oms_tlm_domain_hydraulic) {
          statename = "V";
          flowname =  "q";
          effortname = "p";
        }
        else if(bus->getDomain() == oms_tlm_domain_electric) {
          statename = "Q";
          flowname =  "i";
          effortname = "U";
        }
        busLogIds[bus] = registerLogVariable();
        resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName())+"."+statename, "", SignalType_REAL);
        registerLogVariable();
        resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName())+"."+flowname, "", SignalType_REAL);
        registerLogVariable();
        resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName())+"."+effortname, "", SignalType_REAL);
      }
      else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir) {
        busLogIds[bus] = registerLogVariable();
        resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName())+".x[1]", "", SignalType_REAL);
        std::vector<std::string> varNames = {"x[2]","x[3]",
                                             "A[1,1]","A[1,2]","A[1,3]","A[2,1]","A[2,2]","A[2,3]","A[3,1]","A[3,2]","A[3,3]",
                                             "v[1]","v[2]","v[3]","w[1]","w[2]","w[3]",
                                             "f[1]","f[2]","f[3]","t[1]","t[2]","t[3]"};
        for(std::string& varName : varNames) {
          registerLogVariable();
          resultFile.addSignal(std::string(system.second->getFullCref()+bus->getName())+"."+varName, "", SignalType_REAL);
        }
      }
    }
    registerLogVariables(system.second, resultFile);
  }

  return oms_status_ok;
}

int oms::SystemTLM::registerLogVariable()
{
  return numLogVars++;
}

void oms::SystemTLM::registerLogVariables(oms::System *system, ResultWriter& resultFile)
{
  for(int i=0; system->getConnectors()[i]; ++i) {
    if(system->getConnectors()[i]->isOutput() && system->getConnectors()[i]->isTypeReal()) {
      connectorLogIds[system->getConnectors()[i]] = registerLogVariable();
      resultFile.addSignal(std::string(system->getFullCref()+system->getConnectors()[i]->getName()), "", SignalType_REAL);
    }
  }

  for(auto& component : system->getComponents()) {
    for(int i=0; component.second->getConnectors()[i]; ++i) {
      if(component.second->getConnectors()[i]->isOutput() && component.second->getConnectors()[i]->isTypeReal()) {
        connectorLogIds[component.second->getConnectors()[i]] = registerLogVariable();
        resultFile.addSignal(std::string(system->getFullCref()+component.second->getCref()+component.second->getConnectors()[i]->getName()), "", SignalType_REAL);
      }
    }
  }

  for(auto& subsystem : system->getSubSystems()) {
    registerLogVariables(subsystem.second, resultFile);
  }
}

void oms::SystemTLM::sendValuesToLogger(oms::System *system, double time)
{
  for(int i=0; system->getConnectors()[i]; ++i) {
    if(system->getConnectors()[i]->isOutput() && system->getConnectors()[i]->isTypeReal()) {
      double value;
      system->getReal(system->getConnectors()[i]->getName(), value);
      sendValueToLogger(connectorLogIds[system->getConnectors()[i]], time, value);
    }
  }

  for(auto& component : system->getComponents()) {
    for(int i=0; component.second->getConnectors()[i]; ++i) {
      if(component.second->getConnectors()[i]->isOutput() && component.second->getConnectors()[i]->isTypeReal()) {
        double value;
        component.second->getReal(component.second->getConnectors()[i]->getName(), value);
        sendValueToLogger(connectorLogIds[component.second->getConnectors()[i]], time, value);
      }
    }
  }

  for(auto& subsystem : system->getSubSystems()) {
    sendValuesToLogger(subsystem.second, time);
  }
}

oms_status_enu_t oms::SystemTLM::updateSignals(oms::ResultWriter &resultFile)
{
  if (logBuffer.size() == 0)
    return oms_status_ok;

  int i = (clock_id) ? 2 : 1;  // TODO: see #596

  if (clock_id)
  {
    SignalValue_t wallTime;
    wallTime.realValue = clock.getElapsedWallTime();
    resultFile.updateSignal(clock_id, wallTime);
  }

  for(auto& varbuffer : logBuffer) {
    auto& databuffer = varbuffer.second;
    double x1,x2,t1,t2;
    t2 = databuffer.back().first;
    x2 = databuffer.back().second;
    t1 = t2;
    x1 = x2;
    for(int i=databuffer.size()-1; i>=0; --i) {
      if(databuffer[i].first < nextLogTime) {
        t1 = databuffer[i].first;
        x1 = databuffer[i].second;
        break;
      }
    }

    SignalValue_t value;
    value.realValue = interpolate(t1,t2,x1,x2,logTime);
    resultFile.updateSignal(i, value);
    ++i;
  }
  return oms_status_ok;
}
