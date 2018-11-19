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

#include "SystemTLM.h"

#include "Component.h"
#include "Model.h"
#include "SystemWC.h"
#include "Types.h"
#include "OMTLMSimulatorLib.h"
#include "ssd/Tags.h"
#include "OMTLMSimulatorLib.h"

#include <thread>
#include <algorithm>

oms3::SystemTLM::SystemTLM(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms3::System(cref, oms_system_tlm, parentModel, parentSystem)
{
  logTrace();
  model = omtlm_newModel(cref.c_str());
  omtlm_setLogLevel(model, 1);
  omtlm_setNumLogStep(model, 1000);
}

oms3::SystemTLM::~SystemTLM()
{
  omtlm_unloadModel(model);
}

oms3::System* oms3::SystemTLM::NewSystem(const oms3::ComRef& cref, oms3::Model* parentModel, oms3::System* parentSystem)
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

oms_status_enu_t oms3::SystemTLM::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  pugi::xml_node node_simulation_information = node.append_child(oms2::ssd::ssd_simulation_information);

  pugi::xml_node node_annotations = node_simulation_information.append_child(oms2::ssd::ssd_annotations);

  pugi::xml_node node_annotation = node_annotations.append_child(oms2::ssd::ssd_annotation);
  node_annotation.append_attribute("type") = oms::annotation_type;

  pugi::xml_node node_tlm = node_annotation.append_child(oms::tlm_master);
  node_tlm.append_attribute("ip") = address.c_str();
  node_tlm.append_attribute("managerport") = std::to_string(desiredManagerPort).c_str();
  node_tlm.append_attribute("monitorport") = std::to_string(desiredMonitorPort).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::importFromSSD_SimulationInformation(const pugi::xml_node& node)
{
  pugi::xml_node annotationsNode = node.child(oms2::ssd::ssd_annotations);
  if(annotationsNode) {
    pugi::xml_node annotationNode = annotationsNode.child(oms2::ssd::ssd_annotation);
    if(annotationNode && std::string(annotationNode.attribute("type").as_string()) == "org.openmodelica") {
      pugi::xml_node tlmmasterNode = annotationNode.child("oms:TlmMaster");
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
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::instantiate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::initialize()
{
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
    omtlm_addSubModel(model, subsystem.second->getCref().c_str(),"","none");
    for(int i=0; subsystem.second->getTLMBusConnectors()[i]; ++i) {
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

      omtlm_addInterface(model, subsystem.second->getCref().c_str(), tlmbus->getName().c_str(),dimensions, causality.c_str(), tlmbus->getDomain().c_str());
    }
  }

  Connection** connections = this->getConnections(ComRef(""));
  for(int i=0; connections[i]; ++i) {
    oms3_tlm_connection_parameters_t* tlmpars = connections[i]->getTLMParameters();
    omtlm_addConnection(model,connections[i]->getSignalA().c_str(),connections[i]->getSignalB().c_str(),tlmpars->delay,tlmpars->linearimpedance,tlmpars->angularimpedance,tlmpars->alpha);
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::reset()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->reset())
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  omtlm_setStartTime(model, getModel()->getStartTime());
  omtlm_setStopTime(model, stopTime);

  if(getSubSystems().empty() && getComponents().empty())
    logWarning("oms3::TLMCompositeModel::stepUntil: Simulating empty model...");

  logInfo("Starting TLM manager in new thread");

  std::thread *masterThread = new std::thread(&omtlm_simulate, model);

  logInfo("Connecting submodels to manager (threaded)");
  std::string server = address + ":" + std::to_string(actualManagerPort);
  std::vector<std::thread> fmiConnectThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System* subsystem = it->second;
    ComRef syscref = subsystem->getCref();
    fmiConnectThreads.push_back(std::thread(&oms3::SystemTLM::connectToSockets, this, syscref, server));
  }
  for(auto &thread : fmiConnectThreads)
    thread.join();
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    if(find(connectedsubsystems.begin(), connectedsubsystems.end(), it->second->getCref()) == connectedsubsystems.end())
      return logError("Failed to connect TLM subsystem: "+std::string(it->second->getCref()));
  }

  logInfo("Initializing TLM submodels (threaded)");
  std::vector<std::thread> fmiInitializeThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System *subsystem = it->second;
    ComRef syscref = subsystem->getCref();
    fmiInitializeThreads.push_back(std::thread(&oms3::SystemTLM::initializeSubSystem, this, syscref));
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
    fmiSimulateThreads.push_back(std::thread(&oms3::SystemTLM::simulateSubSystem, this, syscref, stopTime));
  }
  for(auto &thread : fmiSimulateThreads)
    thread.join();

  masterThread->join();
  delete masterThread;

  logInfo("Disconnecting submodels from manager (threaded)");
  std::vector<std::thread> fmiDisconnectThreads;
  for(auto it = getSubSystems().begin(); it!=getSubSystems().end(); ++it) {
    System* subsystem = it->second;
    fmiDisconnectThreads.push_back(std::thread(&oms3::SystemTLM::disconnectFromSockets, this, subsystem->getCref()));
  }
  for(auto &thread : fmiDisconnectThreads)
    thread.join();

  logInfo("Simulation of TLM composite model "+std::string(getCref())+" complete.");

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::connectToSockets(const oms3::ComRef cref, std::string server)
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
      if(system->getStepSize() > tlmbuses[i]->getDelay()*0.5) {
        system->setFixedStepSize(tlmbuses[i]->getDelay()*0.5);
        logInfo("Limiting stepSize for "+std::string(getCref())+"."+std::string(tlmbuses[i]->getName())+" to "+std::to_string(system->getStepSize()));
      }
  }

  logInfo("Creating TLM plugin instance for "+std::string(cref));

  TLMPlugin* plugin = TLMPlugin::CreateInstance();
  pluginsMutex.lock();
  plugins[system] = plugin;
  pluginsMutex.unlock();

  logInfo("Initializing plugin for "+std::string(cref));

  if(!plugin->Init(std::string(cref),
                   getModel()->getStartTime(),
                   1, //Unused argument anyway
                   system->getStepSize(),
                   server)) {
    logError("Error initializing the TLM plugin for "+std::string(cref));
    return oms_status_error;
  }

  logInfo("Registering interfaces for "+std::string(cref));

  for (int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
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

void oms3::SystemTLM::disconnectFromSockets(const oms3::ComRef cref)
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

oms_status_enu_t oms3::SystemTLM::setSocketData(const std::string &address, int managerPort, int monitorPort)
{
  if (oms_modelState_terminated != getModel()->getModelState())
    return logError_ModelInWrongState(this);

  this->address = address;
  this->desiredManagerPort = managerPort;
  this->desiredMonitorPort = monitorPort;
  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::setPositionAndOrientation(const oms3::ComRef &cref, std::vector<double> x, std::vector<double> A)
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

oms_status_enu_t oms3::SystemTLM::updateInitialValues(const oms3::ComRef cref)
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

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_input) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      system->getReal(bus->getConnector(tlmrefs.y), value);
      plugin->SetInitialValue(bus->getId(), value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_1d_cg_t tlmrefs;
      double effort,flow;
      system->getReal(bus->getConnector(tlmrefs.c), effort);
      system->getReal(bus->getConnector(tlmrefs.v), flow);
      plugin->SetInitialForce1D(bus->getId(), effort);
      plugin->SetInitialFlow1D(bus->getId(), flow);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_1d_fg_t tlmrefs;
      double effort,flow;
      system->getReal(bus->getConnector(tlmrefs.c[0]), effort);
      system->getReal(bus->getConnector(tlmrefs.v), flow);
      plugin->SetInitialForce1D(bus->getId(), effort);
      plugin->SetInitialFlow1D(bus->getId(), flow);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(3,0);
      std::vector<double> angularflow(3,0);
      system->getReals(bus->getConnectors(tlmrefs.f), effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      system->getReals(bus->getConnectors(flowrefs), flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_3d_cg_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(3,0);
      std::vector<double> angularflow(3,0);
      system->getReals(bus->getConnectors(tlmrefs.c), effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      system->getReals(bus->getConnectors(flowrefs), flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_3d_fg_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(3,0);
      std::vector<double> angularflow(3,0);
      system->getReals(bus->getConnectors(tlmrefs.c[0]), effort);
      std::vector<int> flowrefs = tlmrefs.v;
      flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
      system->getReals(bus->getConnectors(flowrefs), flow);
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::initializeSubSystem(oms3::ComRef cref)
{
  oms_status_enu_t status = getSubSystem(cref)->initialize();
  if(status = oms_status_ok)
    status = updateInitialValues(cref);
  if(status == oms_status_ok) {
    setInitializedMutex.lock();
    initializedsubsystems.push_back(cref);
    setInitializedMutex.unlock();
  }
  return status;
}

oms_status_enu_t oms3::SystemTLM::simulateSubSystem(oms3::ComRef cref, double stopTime)
{
  oms_status_enu_t status = getSubSystem(cref)->stepUntil(stopTime, NULL);
  plugins[getSubSystem(cref)]->AwaitClosePermission();
  return status;
}

void oms3::SystemTLM::writeToSockets(SystemWC *system, double time, Component* component)
{
  socketMutexes[system].lock();

  TLMPlugin *plugin = plugins.find(system)->second;
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for(int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    int id = bus->getId();      //Need socket ID from top-level bus
    bus = bus->getActualBus();  //Communicate with actual bus

    if(component && component != bus->getComponent()) {
      continue; //Ignore FMUs not specified in vector
    }
    component = bus->getComponent();

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_output) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      system->getReal(bus->getConnector(tlmrefs.y), value);
      plugin->SetValueSignal(id, time, value);
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
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir) {

      oms_tlm_sigrefs_3d_t tlmrefs;

      std::vector<double> x(3,0);
      std::vector<double> A(9,0);
      std::vector<double> v(3,0);
      std::vector<double> w(3,0);
      std::vector<double> f(6,0);

      system->getReals(bus->getConnectors(tlmrefs.x), x);
      system->getReals(bus->getConnectors(tlmrefs.A), A);
      system->getReals(bus->getConnectors(tlmrefs.v), v);
      system->getReals(bus->getConnectors(tlmrefs.w), w);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      plugin->GetForce3D(id, time, &x[0], &A[0], &v[0], &w[0], &f[0]);

      //Send the resulting motion back to master
      plugin->SetMotion3D(id, time, &x[0], &A[0], &v[0], &w[0]);
    }
  }

  socketMutexes[system].unlock();
}

void oms3::SystemTLM::readFromSockets(SystemWC* system, double time, Component* component)
{
  socketMutexes[system].lock();

  TLMPlugin *plugin = plugins.find(system)->second;
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for(int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    int id = bus->getId();      //Need socket ID from top-level bus
    bus = bus->getActualBus();  //Communicate with actual bus

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

      if(bus->getDomain() != "hydraulic") {
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
      plugin->GetWaveImpedance1D(id, time+system->getStepSize(), &impedance2, &wave2);

      double dWave = (wave2-wave)/system->getStepSize();

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
        t += system->getStepSize()/9;

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
      plugin->GetWaveImpedance3D(id, time+system->getStepSize(), &Zt2, &Zr2, &waves2[0]);

      std::vector<double> dWaves(6,0);
      for(size_t i=0; i<6; ++i) {
        double dWave = (waves2[i]-waves[i])/system->getStepSize();
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
        t += system->getStepSize()/9;

        bus->setReals(tlmrefs.c[i], waves);
        bus->setReal(tlmrefs.t[i], t);
      }

      bus->setReal(tlmrefs.Zt, Zt);
      bus->setReal(tlmrefs.Zr, Zr);
    }
  }

  socketMutexes[system].unlock();
}
