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

oms_status_enu_t oms3::SystemTLM::stepUntil(double stopTime)
{
  return logError_NotImplemented;
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
        system->setStepSize(tlmbuses[i]->getDelay()*0.5);
        logInfo("Limiting stepSize for "+std::string(getCref())+"."+std::string(tlmbuses[i]->getName())+" to "+std::to_string(system->getStepSize()));
      }
  }

  logInfo("Creating TLM plugin instance for "+std::string(cref));

  TLMPlugin* plugin = TLMPlugin::CreateInstance();
  plugins[system] = plugin;

  logInfo("Initializing plugin for "+std::string(cref));

  if(!plugin->Init(std::string(cref),
                   system->getTime(),
                   1, //Unused argument anyway
                   system->getStepSize(),
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
