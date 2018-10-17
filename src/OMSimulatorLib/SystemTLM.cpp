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
#include "ssd/Tags.h"
#include "OMTLMSimulatorLib.h"

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
    logError("\"" + std::string(cref) + "\" is not a valid ident");
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError("Internal error");
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
  node_tlm.append_attribute("managerport") = std::to_string(managerPort).c_str();
  node_tlm.append_attribute("monitorport") = std::to_string(monitorPort).c_str();

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
          this->managerPort = tlmmasterNode.attribute("managerport").as_int();
        else if(name == "monitorport")
          this->monitorPort = tlmmasterNode.attribute("monitorport").as_int();
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::SystemTLM::instantiate()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::SystemTLM::initialize()
{
#ifndef _WIN32
  omtlm_checkPortAvailability(&managerPort);
  omtlm_checkPortAvailability(&monitorPort);
#endif

  omtlm_setAddress(model, address);
  omtlm_setManagerPort(model, managerPort);
  omtlm_setMonitorPort(model, monitorPort);

  return logError_NotImplemented;
}

oms_status_enu_t oms3::SystemTLM::terminate()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms3::SystemTLM::connectToSockets(const oms3::ComRef cref, std::string server)
{
  logInfo("Starting TLM simulation thread for model "+std::string(cref));

  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(cref));
  if(system == nullptr)
    return logError_SubSystemNotInSystem(getName(),cref);

  ///< Todo: Where obtain time and communication interval?
  double time = 0;    ///< Todo: Remove when implemented
  double communicationInterval = 0.001;   ///< Todo: Remove when implemented

  //Limit communication interval to half TLM delay
  //This is for avoiding extrapolation when running asynchronously.

  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for (int i=0; tlmbuses[i]; ++i)
  {
      if(communicationInterval > tlmbuses[i]->getDelay()*0.5) {
        communicationInterval = tlmbuses[i]->getDelay()*0.5;
        logInfo("Limiting communicationInterval for "+std::string(getName())+"."+std::string(tlmbuses[i]->getName())+" to "+std::to_string(communicationInterval));
      }
  }

  logInfo("Creating TLM plugin instance for "+std::string(cref));

  TLMPlugin* plugin = TLMPlugin::CreateInstance();
  plugins[system] = plugin;

  logInfo("Initializing plugin for "+std::string(cref));

  if(!plugin->Init(std::string(cref),
                   time,
                   1, //Unused argument anyway
                   communicationInterval,
                   server)) {
    logError("Error initializing the TLM plugin for "+std::string(cref));
    return oms_status_error;
  }

  logInfo("Registering interfaces for "+std::string(cref));

  for (int i=0; tlmbuses[i]; ++i)
  {
    oms_status_enu_t status = tlmbuses[i]->registerToSockets(plugin);
    if(status == oms_status_error) {
      return logError("Failed to register interface "+std::string(cref)+"."+std::string(tlmbuses[i]->getName()));
    }
  }

  connectedsubsystems.push_back(cref);

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
    plugin->AwaitClosePermission();
    delete plugin;
    plugins[system] = nullptr;
  }
}

oms_status_enu_t oms3::SystemTLM::setSocketData(const std::string &address, int managerPort, int monitorPort)
{
  this->address = address;
  this->managerPort = managerPort;
  this->monitorPort = monitorPort;
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

oms_status_enu_t oms3::SystemTLM::setInitialValues(ComRef cref, std::vector<double> values)
{
  oms3::ComRef tail(cref);
  oms3::ComRef head = tail.pop_front();
  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(head));

  if(system == nullptr)
    return logError_SubSystemNotInSystem(getName(),cref);

  //Find interface log
  bool found = false;
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for (int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];
    if(bus->getName() == tail) {
      found = true;
      if(bus->getDimensions() == 1 && bus->getCausality() != oms_causality_bidir) {
        if(values.size() < 1) {
          return logError("No initial TLM value specified.");
        }
        initialValues.insert(std::make_pair(cref, values));
      }
      else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir) {
        if(values.size() < 2) {
          return logError("Too few initial TLM values specified for 1D interface (should be 2, effort and flow).");
        }
        initialValues.insert(std::make_pair(cref, values));
      }
      else if(bus->getDimensions() == 3) {
        if(values.size() < 12) {
          return logError("Too few initial TLM values specified for 3D interface (should be 12, 3 forces, 3 torques, 3 velocities and 3 angular velocities).");
        }
        initialValues.insert(std::make_pair(cref, values));
      }
      break;
    }
  }

  if(!found) {
    return logError("TLMBusConnector \""+std::string(tail)+"\" not found in system \""+std::string(head)+"\"");
  }

  return oms_status_ok;
}


oms_status_enu_t oms3::SystemTLM::updateInitialValues(const oms3::ComRef cref)
{
  SystemWC* system = reinterpret_cast<SystemWC*>(getSystem(cref));
  if(system == nullptr)
    return logError_SubSystemNotInSystem(getName(),cref);
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
      if(initialValues.find(bus->getName()) != initialValues.end()) {
        value = initialValues.find(bus->getName())->second[0];
      }
      else {
        system->getReal(bus->getConnector(tlmrefs.y), value);
      }
      plugin->SetInitialValue(bus->getId(), value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_1d_t tlmrefs;
      double effort,flow;
      if(initialValues.find(bus->getName()) != initialValues.end()) {
        effort = initialValues.find(bus->getName())->second[0];
        flow = initialValues.find(bus->getName())->second[1];
      }
      else {
        system->getReal(bus->getConnector(tlmrefs.f), effort);
        system->getReal(bus->getConnector(tlmrefs.v), flow);
      }
      plugin->SetInitialForce1D(bus->getId(), effort);
      plugin->SetInitialFlow1D(bus->getId(), flow);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() != oms_tlm_no_interpolation) {
      if(initialValues.find(bus->getName()) != initialValues.end()) {
        double effort = initialValues.find(bus->getName())->second[0];
        double flow = initialValues.find(bus->getName())->second[1];
        plugin->SetInitialForce1D(bus->getId(), effort);
        plugin->SetInitialFlow1D(bus->getId(), flow);
      }
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      if(initialValues.find(bus->getName()) != initialValues.end()) {
        effort[0] = initialValues.find(bus->getName())->second[0];
        effort[1] = initialValues.find(bus->getName())->second[1];
        effort[2] = initialValues.find(bus->getName())->second[2];
        effort[3] = initialValues.find(bus->getName())->second[3];
        effort[4] = initialValues.find(bus->getName())->second[4];
        effort[5] = initialValues.find(bus->getName())->second[5];
        flow[0] = initialValues.find(bus->getName())->second[6];
        flow[1] = initialValues.find(bus->getName())->second[7];
        flow[2] = initialValues.find(bus->getName())->second[8];
        flow[3] = initialValues.find(bus->getName())->second[9];
        flow[4] = initialValues.find(bus->getName())->second[10];
        flow[5] = initialValues.find(bus->getName())->second[11];
      }
      else {
        system->getReals(bus->getConnectors(tlmrefs.f), effort);
        std::vector<int> flowrefs = tlmrefs.v;
        flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
        system->getReals(bus->getConnectors(flowrefs), flow);
      }
      plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(bus->getDimensions() == 3 && bus->getCausality() == oms_causality_bidir &&
            bus->getInterpolation() != oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      if(initialValues.find(bus->getName()) != initialValues.end()) {
        effort[0] = initialValues.find(bus->getName())->second[0];
        effort[1] = initialValues.find(bus->getName())->second[1];
        effort[2] = initialValues.find(bus->getName())->second[2];
        effort[3] = initialValues.find(bus->getName())->second[3];
        effort[4] = initialValues.find(bus->getName())->second[4];
        effort[5] = initialValues.find(bus->getName())->second[5];
        flow[0] = initialValues.find(bus->getName())->second[6];
        flow[1] = initialValues.find(bus->getName())->second[7];
        flow[2] = initialValues.find(bus->getName())->second[8];
        flow[3] = initialValues.find(bus->getName())->second[9];
        flow[4] = initialValues.find(bus->getName())->second[10];
        flow[5] = initialValues.find(bus->getName())->second[11];
        plugin->SetInitialForce3D(bus->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
        plugin->SetInitialFlow3D(bus->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
      }
    }
  }

  return oms_status_ok;
}

void oms3::SystemTLM::writeToSockets(SystemWC *system, double time, std::string fmu)
{
  TLMBusConnector** tlmbuses = system->getTLMBusConnectors();
  for(int i=0; tlmbuses[i]; ++i)
  {
    TLMBusConnector* bus = tlmbuses[i];

    ///< todo Re-implement this name check somehow
//    if(!fmu.empty() &&
//       fmu != bus->getFMUName().toString()) {
//      continue; //Ignore FMUs not specified in vector
//    }

    if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_output) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      system->getReal(bus->getConnector(tlmrefs.y), value);
      system->getTLMPlugin()->SetValueSignal(bus->getId(), time, value);
    }
    else if(bus->getDimensions() == 1 && bus->getCausality() == oms_causality_bidir) {
      oms_tlm_sigrefs_1d_t tlmrefs;
      double state, flow, force;
      system->getReal(bus->getConnector(tlmrefs.x), state);
      system->getReal(bus->getConnector(tlmrefs.v), flow);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      system->getTLMPlugin()->GetForce1D(bus->getId(), time, flow, &force);

      //Send the resulting motion back to master
      system->getTLMPlugin()->SetMotion1D(bus->getId(), time, state, flow);
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
      system->getTLMPlugin()->GetForce3D(bus->getId(), time, &x[0], &A[0], &v[0], &w[0], &f[0]);

      //Send the resulting motion back to master
      system->getTLMPlugin()->SetMotion3D(bus->getId(), time, &x[0], &A[0], &v[0], &w[0]);
    }
  }
}
