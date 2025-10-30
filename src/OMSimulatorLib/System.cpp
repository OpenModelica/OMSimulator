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

#include "System.h"

#include "Component.h"
#include "ComponentFMUCS.h"
#include "ComponentFMUME.h"
#include "ComponentTable.h"
#include "Flags.h"
#include "Model.h"
#include "OMSFileSystem.h"
#include "OMSString.h"
#include "ssd/Tags.h"
#include "SystemSC.h"
#include "SystemTLM.h"
#include "SystemWC.h"
#include "Variable.h"

#include <regex>

oms::System::System(const oms::ComRef& cref, oms_system_enu_t type, oms::Model* parentModel, oms::System* parentSystem, oms_solver_enu_t solverMethod)
  : element(oms_element_system, cref), cref(cref), type(type), parentModel(parentModel), parentSystem(parentSystem), solverMethod(solverMethod)
{
  minimumStepSize = Flags::MinimumStepSize();
  maximumStepSize = Flags::MaximumStepSize();
  initialStepSize = Flags::InitialStepSize();

  connections.push_back(NULL);

  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);

#if !defined(NO_TLM)
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
#endif

  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);
}

oms::System::~System()
{
  if (system_geometry)
    delete system_geometry;

  for (const auto& connector : connectors)
    if (connector)
      delete connector;

  for (const auto& connection : connections)
    if (connection)
      delete connection;

  for (const auto& component : components)
    delete component.second;

  for (const auto& subsystem : subsystems)
    delete subsystem.second;

  for (const auto& busconnector : busconnectors)
    if (busconnector)
      delete busconnector;

#if !defined(NO_TLM)
  for (const auto tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector)
      delete tlmbusconnector;
#endif
}

oms::System* oms::System::NewSystem(const oms::ComRef& cref, oms_system_enu_t type, oms::Model* parentModel, oms::System* parentSystem)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return nullptr;
  }

  if (parentSystem && parentSystem->getSystem(cref))
  {
    logError("Name is already in use");
    return nullptr;
  }

  if (parentSystem && parentSystem->getComponent(cref))
  {
    logError("Name is already in use");
    return nullptr;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError_InternalError;
    return nullptr;
  }

  switch (type)
  {
  case oms_system_tlm:
    if (parentSystem)
    {
      logError("A TLM system must be the the root system of a model.");
      return nullptr;
    }
#if !defined(NO_TLM)
    return SystemTLM::NewSystem(cref, parentModel, parentSystem);
#else
    return nullptr;
#endif

  case oms_system_wc:
    if (parentSystem && oms_system_tlm != parentSystem->getType())
    {
      logError("A WC system must be the root system or a subsystem of a TLM system.");
      return nullptr;
    }
    return SystemWC::NewSystem(cref, parentModel, parentSystem);

  case oms_system_sc:
    if (parentSystem && oms_system_wc != parentSystem->getType())
    {
      logError("A SC system must be the root system or a subsystem of a WC system.");
      return nullptr;
    }
    return SystemSC::NewSystem(cref, parentModel, parentSystem);
  default:
      logError_InternalError;
  return nullptr;
  }
}

oms::ComRef oms::System::getFullCref() const
{
  if (parentSystem)
    return parentSystem->getFullCref() + this->getCref();
  if (parentModel)
    return ComRef(parentModel->getCref()) + this->getCref();

  logError_InternalError;
  return this->getCref();
}

oms::System* oms::System::getSystem(const oms::ComRef& cref)
{
  if (cref.isEmpty())
    return this;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->getSystem(tail);

  return nullptr;
}

oms::Component* oms::System::getComponent(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->getComponent(tail);

  auto component = components.find(cref);
  if (component != components.end())
    return component->second;

  return nullptr;
}

oms::Variable* oms::System::getVariable(const ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getVariable(tail);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getVariable(tail);

  //for (auto& connector : connectors)
  //  if (connector && connector->getName() == cref)
  //    return connector->getVariable();

  logError_UnknownSignal(getFullCref() + cref);
  return nullptr;
}

bool oms::System::validCref(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
    return false;

  if (getSystem(cref))
    return false;

  if (getComponent(cref))
    return false;

  if (getConnector(cref))
    return false;

  if (getBusConnector(cref))
    return false;

#if !defined(NO_TLM)
  if (getTLMBusConnector(cref))
    return false;
#endif

  return true;
}

oms_status_enu_t oms::System::addSubSystem(const oms::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isEmpty())
    return logError_AlreadyInScope(getFullCref());

  if (cref.isValidIdent())
  {
    if (this->type == oms_system_sc)
      return logError_NotForScSystem;

    System* system = System::NewSystem(cref, type, NULL, this);
    if (system)
    {
      subsystems[cref] = system;
      subelements.back() = reinterpret_cast<oms_element_t*>(system->getElement());
      subelements.push_back(NULL);
      element.setSubElements(&subelements[0]);
      return oms_status_ok;
    }
    return oms_status_error;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullCref()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubSystem(tail, type);
}

oms_status_enu_t oms::System::addSubModel(const oms::ComRef& cref, const std::string& path)
{
  if (cref.isValidIdent())
  {
    if (!validCref(cref))
      return logError_AlreadyInScope(getFullCref() + cref);

    filesystem::path path_ = oms_canonical(path);
    if (!filesystem::exists(path_))
      return logError("file does not exist: \"" + path + "\"");

    Component* component = NULL;

    std::string extension = "";
    if (path.length() > 4)
      extension = path.substr(path.length() - 4);

    if (extension == ".fmu" && oms_system_wc == type)
      component = ComponentFMUCS::NewComponent(cref, this, path_.string());
    else if (extension == ".fmu" && oms_system_sc == type)
      component = ComponentFMUME::NewComponent(cref, this, path_.string());
    else if (extension == ".csv" || extension == ".mat")
      component = ComponentTable::NewComponent(cref, this, path_.string());
    else
      return logError("supported sub-model formats are \".fmu\", \".csv\", \".mat\"");

    if (!component)
      return oms_status_error;

    components[cref] = component;
    subelements.back() = reinterpret_cast<oms_element_t*>(component->getElement());
    subelements.push_back(NULL);
    element.setSubElements(&subelements[0]);
    return oms_status_ok;
  }

  ComRef tail(cref);
  ComRef front = tail.pop_front();

  System* system = this->getSystem(front);
  if (!system)
    return logError("System \"" + std::string(getFullCref()) + "\" does not contain system \"" + std::string(front) + "\"");

  return system->addSubModel(tail, path);
}

oms_status_enu_t oms::System::replaceSubModel(const oms::ComRef& cref, const std::string& path, bool dryRun, int& warningCount)
{
  /*
    take the snapshot of entire ssd before replacing,
    if (dryRun==true)
      showwarnings, reimport the snapshot and replacing is not done
    else
      show warnings and replace is done
  */

  // get full snapshot
  char* fullsnapshot = NULL;
  getModel().exportSnapshot("", &fullsnapshot);

  if (cref.isValidIdent())
  {
    if (validCref(cref))
      return logError("\"" + std::string(getFullCref() + cref) + "\" does not exists in the scope and it cannot be replaced, use oms_addSubModel to add new component");

    filesystem::path path_ = oms_canonical(path);
    if (!filesystem::exists(path_))
      return logError("file does not exist: \"" + path + "\"");

    Component* replaceComponent = NULL;

    std::string extension = "";
    if (path.length() > 4)
      extension = path.substr(path.length() - 4);

    if (extension == ".fmu" && oms_system_wc == type)
      replaceComponent = ComponentFMUCS::NewComponent(cref, this, path_.string(), "replace");
    else if (extension == ".fmu" && oms_system_sc == type)
      replaceComponent = ComponentFMUME::NewComponent(cref, this, path_.string(), "replace");
    else if (extension == ".csv" || extension == ".mat")
      replaceComponent = ComponentTable::NewComponent(cref, this, path_.string());
    else
      return logError("supported sub-model formats are \".fmu\", \".csv\", \".mat\"");

    if (!replaceComponent)
      return oms_status_error;

    std::vector<std::string> warningList;

    auto component = components.find(cref);
    if (component != components.end())
    {
      for (auto &connection : connections)
      {
        if (connection)
        {
          oms::ComRef headA(connection->getSignalA());
          oms::ComRef tailA = headA.pop_front();

          oms::ComRef headB(connection->getSignalB());
          oms::ComRef tailB = headB.pop_front();
          // check the replacing variable is a valid ScalarVariable
          bool signalA = isValidScalarVariable(component->second, replaceComponent, connection, cref, connection->getSignalA().front(), headA, path, warningList);
          bool signalB = isValidScalarVariable(component->second, replaceComponent, connection, cref, connection->getSignalB().front(), headB, path, warningList);

          // delete the connection, due to scalarVariable mismatch in the replaced submodel
          if (signalA || signalB)
          {
            delete connection;
            connections.pop_back(); // last element is always NULL
            connection = connections.back();
            connections.back() = NULL;
          }
        }
      }

      //copy the element geometry position of old component to the replacing component which will be used in OMEdit
      replaceComponent->getElement()->setGeometry(component->second->getElement()->getGeometry());

      // copy all the resources from old component to replacing component
      replaceComponent->setValuesResources(component->second->getValues());

      // update or delete the start value in ssv of with the replaced component
      replaceComponent->updateOrDeleteStartValueInReplacedComponent(warningList);

      // update the warning count
      warningCount = warningList.size();

      if (dryRun)
      {
        char * newCref = NULL;
        getModel().importSnapshot(fullsnapshot, &newCref);
        return oms_status_ok;
      }

      //delete component
      delete component->second;
      components.erase(component);

      // update the replaced component
      components[cref] = replaceComponent;
      subelements.back() = reinterpret_cast<oms_element_t*>(replaceComponent->getElement());
      subelements.push_back(NULL);
      element.setSubElements(&subelements[0]);
    }
  }

  return oms_status_ok;
}

/*
* function which checks a variable has the same scalarVariable attributes from two fmus
* (e.g) ScalarVariable_A.type = ScalarVariable_B.type
*       ScalarVariable_A.causality = ScalarVariable_B.causality
*/
bool oms::System::isValidScalarVariable(Component* referenceComponent, Component* replacingComponent, Connection* connection, const ComRef& crefA, const ComRef& crefB, const ComRef& signalName, const std::string& path, std::vector<std::string>& warningList)
{
  if (crefA == crefB)
  {
    Variable *oldVar = referenceComponent->getVariable(signalName);
    Variable *replaceVar = replacingComponent->getVariable(signalName);
    std::string errorMsg="";
    if (!oldVar || !replaceVar)
    {
      errorMsg = "deleting connection \"" + std::string(connection->getSignalA()) + " ==> " + std::string(connection->getSignalB()) + "\"" + ", as signal \"" + std::string(signalName) + "\"" + " couldn't be resolved to any signal in the replaced submodel \"" + path + "\"";
      logWarning(errorMsg);
      warningList.push_back(errorMsg);
      return true;
    }
    if (oldVar->getCausality() != replaceVar->getCausality())
    {
      errorMsg = "deleting connection \"" + std::string(connection->getSignalA()) + " ==> " + std::string(connection->getSignalB()) + "\"" + ", as signal \"" + std::string(signalName) + "\"" + " has causality mismatch in the replaced submodel \"" + path + "\"";
      logWarning(errorMsg);
      warningList.push_back(errorMsg);
      return true;
    }
    if (oldVar->getType() != replaceVar->getType())
    {
      errorMsg = "deleting connection \"" + std::string(connection->getSignalA()) + " ==> " + std::string(connection->getSignalB()) + "\"" + ", as signal \"" + std::string(signalName) + "\"" + " has type mismatch in the replaced submodel \"" + path + "\"";
      logWarning(errorMsg);
      warningList.push_back(errorMsg);
      return true;
    }
  }
  return false;
}

oms_status_enu_t oms::System::newResources(const ComRef& cref, const std::string& ssvFilename, const std::string& ssmFilename, bool externalResources)
{
  ComRef tail(cref);
  ComRef front = tail.pop_front();

  if (tail.isEmpty())
  {
    // top level system and subsystems
    Values resources;
    if (externalResources) // check of external resources and override the start values with new references
    {
      Snapshot snapshot;
      snapshot.importResourceFile(ssvFilename, getModel().getTempDirectory() + "/resources");

      // import ssm file, if provided
      if (!ssmFilename.empty())
        snapshot.importResourceFile(ssmFilename, getModel().getTempDirectory() + "/resources");

      //snapshot.debugPrintAll();
      if (oms_status_ok != resources.importFromSnapshot(snapshot, ssvFilename, ssmFilename))
        return logError("oms_referenceResources failed for \"" + std::string(getFullCref()) + ":" + ssvFilename + "\"");
    }

    if (!values.hasResources())
    {
      if (!ssmFilename.empty())
        resources.ssmFile = "resources/"+ ssmFilename;
      // copy modeldescriptionVariableUnits to ssv resources which will be used to export units
      resources.modelDescriptionVariableUnits = values.modelDescriptionVariableUnits;
      resources.allresources["resources/" + ssvFilename] = resources;
      values.parameterResources.push_back(resources);
    }
    else
    {
      // generate empty ssv file, if more resources are added to same level
      if (!ssmFilename.empty())
        resources.ssmFile = "resources/"+ ssmFilename;
      // copy modeldescriptionVariableUnits to ssv resources which will be used to export units
      resources.modelDescriptionVariableUnits = values.modelDescriptionVariableUnits;
      values.parameterResources[0].allresources["resources/" + ssvFilename] = resources;
    }
    return oms_status_ok;
  }

  auto subsystem = subsystems.find(tail);
  if (subsystem != subsystems.end())
    return subsystem->second->newResources(tail, ssvFilename, ssmFilename, externalResources);

  auto component = components.find(tail);
  if (component != components.end())
    return component->second->newResources(ssvFilename, ssmFilename, externalResources);

  /*check for adding resources to components in subsystems
    e.g root.system1.add
    oms_newResources("root.system1.add:add.ssv")
  */
  ComRef tailA(tail);
  ComRef frontA = tailA.pop_front();

  System* system = this->getSystem(frontA);
  if (!system)
    return logError("System \"" + std::string(getFullCref()) + "\" does not contain subsystem or component \"" + std::string(frontA) + "\"");

  auto componentA = system->components.find(tailA);
  if (componentA != components.end())
    return componentA->second->newResources(ssvFilename, ssmFilename, externalResources);

  return logError("failed for \"" + std::string(getModel().getCref() + cref) + ":" + ssvFilename + "\""  + " as the identifier could not be resolved to a system or subsystem or component");
}

oms_status_enu_t oms::System::deleteReferencesInSSD(const ComRef& cref, const std::string& filename)
{
  ComRef tail(cref);
  ComRef front = tail.pop_front();

  if (tail.isEmpty())
  {
    if (values.hasResources())
      if (oms_status_ok == values.deleteReferencesInSSD(filename))
        return oms_status_ok;
  }

  auto subsystem = subsystems.find(tail);
  if (subsystem != subsystems.end())
    return subsystem->second->deleteReferencesInSSD(tail, filename);

  auto component = components.find(tail);
  if (component != components.end())
    if (oms_status_ok == component->second->deleteReferencesInSSD(filename))
      return oms_status_ok;

  return logError("failed to delete references in ssd, as the reference file \"" + std::string(getModel().getCref() + cref) + ":" + filename + "\""  + " could not be resolved to a system or subsystem or component");
}

oms_status_enu_t oms::System::deleteResourcesInSSP(const std::string& filename)
{
  // search in top level system
  if (values.hasResources())
  {
    if (oms_status_ok == values.deleteResourcesInSSP(filename))
      return oms_status_ok;
  }
  // search in subsytems
  for (const auto &subsystem : subsystems)
  {
    if (subsystem.second->values.hasResources())
    {
      if (oms_status_ok == subsystem.second->values.deleteResourcesInSSP(filename))
        return oms_status_ok;
    }
  }
  // search in component
  for (const auto& component : components)
  {
    if (oms_status_ok == component.second->deleteResourcesInSSP(filename))
      return oms_status_ok;
  }

  return logError("failed to delete resources in ssp, as the reference file \"" + std::string(getModel().getCref()) + ":" + filename + "\""  + " could not be resolved to a system or subsystem or component");
}


oms_status_enu_t oms::System::listUnconnectedConnectors(char** contents) const
{
  if (!contents)
    return oms_status_error;

  std::vector<ComRef> unconnectedInputs;
  std::vector<ComRef> unconnectedOutputs;
  std::vector<ComRef> unconnectedBuses;

  for (const auto& component : components)
  {
    Element* element = component.second->getElement();
    Connector** connectors = element->getConnectors();
    for (int i=0; connectors[i]; ++i)
    {
      ComRef cref(component.second->getCref() + connectors[i]->getName());
      if (!isConnected(cref))
      {
        if (connectors[i]->isInput())
          unconnectedInputs.push_back(getFullCref() + cref);
        if (connectors[i]->isOutput())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connectors[i]->isParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connectors[i]->isCalculatedParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
      }
    }
  }

  for (const auto& connector : connectors)
  {
    if (connector && !isConnected(connector->getName()))
    {
      const ComRef& cref = connector->getName();
      if (!isConnected(cref))
      {
        if (connector->isInput())
          unconnectedInputs.push_back(getFullCref() + cref);
        if (connector->isOutput())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connector->isParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
        if (connector->isCalculatedParameter())
          unconnectedOutputs.push_back(getFullCref() + cref);
      }
    }
  }

  for (const auto& busconnector : busconnectors)
    if (busconnector && !isConnected(busconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + busconnector->getName());

#if !defined(NO_TLM)
  for (const auto& tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector && !isConnected(tlmbusconnector->getName()))
      unconnectedBuses.push_back(getFullCref() + tlmbusconnector->getName());
#endif

  std::string msg;
  for (const auto& unconnectedInput : unconnectedInputs)
    msg += "input " + std::string(unconnectedInput) + "\n";
  for (const auto& unconnectedOutput : unconnectedOutputs)
    msg += "output " + std::string(unconnectedOutput) + "\n";
  for (const auto& unconnectedBus : unconnectedBuses)
    msg += "bus " + std::string(unconnectedBus) + "\n";

  if (!msg.empty())
  {
    *contents = mallocAndCopyString(msg);
    if (!*contents)
      return oms_status_fatal;
  }
  return oms_status_ok;
}

oms_status_enu_t oms::System::exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const
{
  node.append_attribute("name") = this->getCref().c_str();

  if (oms_status_ok != element.getGeometry()->exportToSSD(node))
    return logError("export of system ElementGeometry failed");

  if (system_geometry)
  system_geometry->exportToSSD(node);

  // export top level system connectors
  if (connectors.size() > 1)
  {
    pugi::xml_node connectors_node = node.append_child(oms::ssp::Draft20180219::ssd::connectors);
    for(const auto& connector : connectors)
    {
      if (connector)
        connector->exportToSSD(connectors_node);
    }
  }

  // // get variant name
  // std::string variantName = "";
  // if (parentModel)
  //   variantName = parentModel->getVariantName();
  // else
  //   variantName = parentSystem->parentModel->getVariantName();

  values.exportParameterBindings(node, snapshot, variantName);

  if (subelements.size() > 1)
  {
    pugi::xml_node elements_node = node.append_child(oms::ssp::Draft20180219::ssd::elements);
    for (const auto& subsystem : subsystems)
    {
      pugi::xml_node system_node = elements_node.append_child(oms::ssp::Draft20180219::ssd::system);
      if (oms_status_ok != subsystem.second->exportToSSD(system_node, snapshot, variantName))
        return logError("export of system failed");
    }
    for (const auto& component : components)
    {
      pugi::xml_node component_node = elements_node.append_child(oms::ssp::Draft20180219::ssd::component);
      if (oms_status_ok != component.second->exportToSSD(component_node, snapshot, variantName))
        return logError("export of component failed");
    }
  }

  std::vector<oms::Connection*> busconnections, ssdconnections;

  for (const auto& connection : connections)
  {
    if (connection && connection->getType() == oms_connection_single)
      ssdconnections.push_back(connection);
    else if (connection)
      busconnections.push_back(connection);
  }

  if (!ssdconnections.empty())
  {
    pugi::xml_node connections_node = node.append_child(oms::ssp::Draft20180219::ssd::connections);
    for (const auto& ssdconnection : ssdconnections)
    {
      ssdconnection->exportToSSD(connections_node);
    }
  }

  pugi::xml_node annotations_node = node.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node annotation_node = annotations_node.append_child(oms::ssp::Version1_0::ssc::annotation);
  annotation_node.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;
  pugi::xml_node oms_annotation_node = annotation_node.append_child(oms::ssp::Version1_0::oms_annotations);

#if !defined(NO_TLM)
  if (busconnectors[0] || tlmbusconnectors[0] || !busconnections.empty())
#else
  if (busconnectors[0] || !busconnections.empty())
#endif
  {
    if (busconnectors.size() > 1)
    {
      pugi::xml_node oms_buses_node = oms_annotation_node.append_child(oms::ssp::Version1_0::oms_buses);
      for (const auto& busconnector : busconnectors)
      {
        if (busconnector)
          busconnector->exportToSSD(oms_buses_node);
      }
    }
#if !defined(NO_TLM)
    if (tlmbusconnectors.size() > 1)
    {
      pugi::xml_node oms_buses_node = oms_annotation_node.append_child(oms::ssp::Version1_0::oms_buses);
      for (const auto& tlmbusconnector : tlmbusconnectors)
      {
        if (tlmbusconnector)
          tlmbusconnector->exportToSSD(oms_buses_node);
      }
    }
#endif
    if (!busconnections.empty())
    {
      pugi::xml_node busconnections_node = oms_annotation_node.append_child(oms::ssp::Draft20180219::bus_connections);
      for (const auto& busconnection : busconnections)
        busconnection->exportToSSD(busconnections_node);
    }
  }

  //export ssd:SimulationInformation to end, in order to make it valid with easy-ssp
  if (oms_status_ok != this->exportToSSD_SimulationInformation(oms_annotation_node))
    return logError("export of system SimulationInformation failed");

  return oms_status_ok;
}

oms_status_enu_t oms::System::exportToSSV(Snapshot& snapshot) const
{
  pugi::xml_node ssvNode = snapshot.getTemplateResourceNodeSSV("resources/" + std::string(parentModel->getCref()) + ".ssv", "parameters");

  values.exportToSSV(ssvNode);

  for (const auto& subsystem : subsystems)
    subsystem.second->values.exportToSSV(ssvNode);

  for (const auto& component : components)
    component.second->exportToSSV(ssvNode);

  return oms_status_ok;
}

oms_status_enu_t oms::System::importFromSnapshot(const pugi::xml_node& node, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms::ssp::Draft20180219::ssd::simulation_information && sspVersion == "Draft20180219")
    {
      if (oms_status_ok != importFromSSD_SimulationInformation(*it, sspVersion))
        return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::simulation_information));
    }
    else if (name == oms::ssp::Draft20180219::ssd::element_geometry)
    {
      oms::ssd::ElementGeometry geometry;
      geometry.importFromSSD(*it);
      setGeometry(geometry);
    }
    else if (name == oms::ssp::Draft20180219::ssd::system_geometry)
    {
      if (!system_geometry)
        system_geometry = new oms::ssd::SystemGeometry();
      system_geometry->importFromSSD(*it);
    }
    else if (name == oms::ssp::Version1_0::ssd::parameter_bindings) // parameter bindings provided either as inline or .ssv files
    {
      Values resources; // create a list of <ssd:ParameterBindings>
      if (oms_status_ok != resources.importFromSnapshot(*it, sspVersion, snapshot, variantName))
        return logError("Failed to import " + std::string(oms::ssp::Version1_0::ssd::parameter_bindings));

      // add the list of <parameterBindings>
      values.parameterResources.push_back(resources);
      // TODO generate warning for signals in ssv files that do not belong to system, subsystem or fmus
    }
    else if (name == oms::ssp::Draft20180219::ssd::connections)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        ComRef startElement = ComRef(itConnectors->attribute("startElement").as_string());
        ComRef startConnector = ComRef(itConnectors->attribute("startConnector").as_string());
        ComRef endElement = ComRef(itConnectors->attribute("endElement").as_string());
        ComRef endConnector = ComRef(itConnectors->attribute("endConnector").as_string());
        ComRef crefA = startConnector;
        if (!startElement.isEmpty())
          crefA = startElement + startConnector;
        ComRef crefB = endConnector;
        if (!endElement.isEmpty())
          crefB = endElement + endConnector;

        bool suppressUnitConversion = false;
        if (itConnectors->attribute("suppressUnitConversion").as_bool())
          suppressUnitConversion = itConnectors->attribute("suppressUnitConversion").as_bool();

        if (oms_status_ok != addConnection(crefA, crefB, suppressUnitConversion))
          return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection));
        else
        {
          // Load connection geometry
          if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnectors, crefA, crefB))
            return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection_geometry));
        }
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::connectors)
    {
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
      {
        connectors.back() = oms::Connector::NewConnector(*itConnectors, sspVersion, this->getFullCref());
        if (connectors.back())
        {
          //save the connectors with full cref
          exportConnectors[getFullCref() + connectors.back()->getName()] = true;
          connectors.push_back(NULL);
        }
        else
          return logError("Failed to import ssd:connector");
        element.setConnectors(&connectors[0]);
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::elements)
    {
      for(pugi::xml_node_iterator itElements = (*it).begin(); itElements != (*it).end(); ++itElements)
      {
        name = itElements->name();
        if (name == oms::ssp::Draft20180219::ssd::system)
        {
          ComRef systemCref = ComRef(itElements->attribute("name").as_string());

          // lochel: I guess that can somehow be improved
          oms_system_enu_t systemType = getModel().getSystemType(*itElements, sspVersion);

          if (oms_status_ok != addSubSystem(systemCref, systemType))
            return oms_status_error;

          System* system = getSystem(systemCref);
          if (!system)
            return oms_status_error;

          if (oms_status_ok != system->importFromSnapshot(*itElements, sspVersion, snapshot, variantName))
            return oms_status_error;
        }
        else if (name == oms::ssp::Draft20180219::ssd::component)
        {
          Component* component = NULL;
          std::string type = itElements->attribute("type").as_string();
          // allow component type to be empty, as type is optional according to SSP-1.0 and default type is application/x-fmu-sharedlibrary
          if ("application/x-fmu-sharedlibrary" == type || type.empty() && getType() != oms_system_tlm)
          {
            if (getType() == oms_system_wc)
              component = ComponentFMUCS::NewComponent(*itElements, this, sspVersion, snapshot, variantName);
            else if (getType() == oms_system_sc)
              component = ComponentFMUME::NewComponent(*itElements, this, sspVersion, snapshot, variantName);
            else
              return logError("wrong xml schema detected: " + name);
          }
          else if ("application/table" == type || "text/csv" == type)
            component = ComponentTable::NewComponent(*itElements, this, sspVersion, snapshot);
#if !defined(NO_TLM)
          else if (itElements->attribute("type") == nullptr && getType() == oms_system_tlm) {
            std::string name = itElements->attribute("name").as_string();
            std::string source = itElements->attribute("source").as_string();

            // parse older <ssd:SimulationInformation> <ssd:annotations> </ssd:annotations> </ssd:SimulationInformation>
            if (sspVersion == "Draft20180219")
            {
              pugi::xml_node simulationInformationNode = itElements->child(oms::ssp::Draft20180219::ssd::simulation_information);
              if (simulationInformationNode && sspVersion == "Draft20180219")
              {
                pugi::xml_node annotationsNode = simulationInformationNode.child(oms::ssp::Draft20180219::ssd::annotations);
                if (annotationsNode)
                {
                  if (annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation))
                  {
                    logWarning_deprecated;
                  }
                  for (pugi::xml_node annotationNode = annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation); annotationNode; annotationNode = annotationsNode.next_sibling(oms::ssp::Draft20180219::ssd::annotation))
                  {
                    std::string type = annotationNode.attribute("type").as_string();
                    if (oms::ssp::Draft20180219::annotation_type == type)
                    {
                      pugi::xml_node externalModelNode = annotationNode.child(oms::ssp::Draft20180219::external_model);
                      if (externalModelNode)
                      {
                        std::string startScript = externalModelNode.attribute("startscript").as_string();
                        component = oms::ExternalModel::NewComponent(name, this, source, startScript);
                      }
                    }
                  }
                }
              }
            }

            // sspVersion-1.0
            pugi::xml_node annotationsNode = itElements->child(oms::ssp::Draft20180219::ssd::annotations);
            pugi::xml_node annotation_node = annotationsNode.child(oms::ssp::Version1_0::ssc::annotation);

            // check for ssd:annotation to support older version, which is a bug
            if(!annotation_node)
            {
              annotation_node = annotationsNode.child(oms::ssp::Draft20180219::ssd::annotation);
              logWarning_deprecated;
            }

            if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
            {
              pugi::xml_node oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);
              // support older <ssc:annotation>
              if (!oms_annotation_node)
              {
                oms_annotation_node = annotation_node;
                logWarning_deprecated;
              }

              if (oms_annotation_node)
              {
                pugi::xml_node oms_simulation_information = oms_annotation_node.child(oms::ssp::Version1_0::simulation_information);
                if (oms_simulation_information)
                {
                  pugi::xml_node externalModelNode = oms_simulation_information.child(oms::ssp::Draft20180219::external_model);
                  if (externalModelNode)
                  {
                    std::string startScript = externalModelNode.attribute("startscript").as_string();
                    component = oms::ExternalModel::NewComponent(name, this, source, startScript);
                  }
                }
                // parse <oms:Buses>
                for (pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
                {
                  std::string nodeName = itAnnotations->name();
                  // support older <oms:bus>
                  if (nodeName == oms::ssp::Draft20180219::bus)
                  {
                    importTLMBus(*itAnnotations, component);
                  }
                  // <oms:buses>
                  if (nodeName == oms::ssp::Version1_0::oms_buses)
                  {
                    for (pugi::xml_node_iterator itbuses = itAnnotations->begin(); itbuses != itAnnotations->end(); ++itbuses)
                    {
                      if (std::string(itbuses->name()) == oms::ssp::Draft20180219::bus)
                      {
                        importTLMBus(*itbuses, component);
                      }
                    }
                  }
                }
              }
            }
          }
#endif
          if (component)
          {
            components[component->getCref()] = component;
            subelements.back() = reinterpret_cast<oms_element_t*>(component->getElement());
            subelements.push_back(NULL);
            element.setSubElements(&subelements[0]);
          }
          else
            return oms_status_error;
        }
        else
          return logError("wrong xml schema detected: " + name);
      }
    }
    else if (name == oms::ssp::Draft20180219::ssd::annotations)
    {
      pugi::xml_node annotation_node = it->child(oms::ssp::Version1_0::ssc::annotation);

      // check for ssd:annotation to support older version, which is a bug
      if(!annotation_node)
      {
        annotation_node = it->child(oms::ssp::Draft20180219::ssd::annotation);
        logWarning_deprecated;
      }

      if (annotation_node && std::string(annotation_node.attribute("type").as_string()) == oms::ssp::Draft20180219::annotation_type)
      {
        pugi::xml_node oms_annotation_node = annotation_node.child(oms::ssp::Version1_0::oms_annotations);

        // support older <ssc:annotation>
        if (!oms_annotation_node)
        {
          oms_annotation_node = annotation_node;
          logWarning_deprecated;
        }

        // parse <oms:annotations>
        for(pugi::xml_node_iterator itAnnotations = oms_annotation_node.begin(); itAnnotations != oms_annotation_node.end(); ++itAnnotations)
        {
          name = itAnnotations->name();

          // check for oms:simulationInformation from version 1.0
          if (std::string(name) == oms::ssp::Version1_0::simulation_information && sspVersion == "1.0")
          {
            if (oms_status_ok != importFromSSD_SimulationInformation(*itAnnotations, sspVersion))
              return logError("Failed to import " + std::string(oms::ssp::Version1_0::simulation_information));
          }

          // support older <ssc:annotation> <oms:bus> </oms:bus> </ssc:annotation>
          if (std::string(name) == oms::ssp::Draft20180219::bus)
          {
            //Load bus connector
            std::string busname = itAnnotations->attribute("name").as_string();
            if (std::string(itAnnotations->attribute("type").as_string()) == "tlm")
            {
              importTLMBus(*itAnnotations, NULL);
            }
            else
            {
              if (oms_status_ok != addBus(busname))
                return oms_status_error;
            }
            //Load bus connector signals
            importBusConnectorSignals (*itAnnotations);

            // Load bus connector geometry
            importBusConnectorGeometry(*itAnnotations);
          }

          // support <ssc:annotation> <oms:buses> <oms:bus> </oms:bus> </oms:buses></ssc:annotation>
          if (std::string(name) == oms::ssp::Version1_0::oms_buses)
          {
            for(pugi::xml_node_iterator itbuses = itAnnotations->begin(); itbuses != itAnnotations->end(); ++itbuses)
            {
              if (std::string(itbuses->name()) == oms::ssp::Draft20180219::bus)
              {
                //Load bus connector
                std::string busname = itbuses->attribute("name").as_string();
                if (std::string(itbuses->attribute("type").as_string()) == "tlm")
                {
                  importTLMBus(*itbuses, NULL);
                }
                else
                {
                  if (oms_status_ok != addBus(busname))
                    return oms_status_error;
                }
                //Load bus connector signals
                importBusConnectorSignals(*itbuses);

                // Load bus connector geometry
                importBusConnectorGeometry(*itbuses);
              }
            }
          }
          else if (std::string(name) == oms::ssp::Draft20180219::bus_connections)
          {
            //Load bus connections
            for(pugi::xml_node_iterator itConnection = itAnnotations->begin(); itConnection != itAnnotations->end(); ++itConnection)
            {
              //Load bus connection
              oms::ComRef element1(itConnection->attribute("startElement").as_string());
              oms::ComRef connector1(itConnection->attribute("startConnector").as_string());
              oms::ComRef element2(itConnection->attribute("endElement").as_string());
              oms::ComRef connector2(itConnection->attribute("endConnector").as_string());

              oms_status_enu_t status;
              // Load TLM bus connection
              if (itConnection->attribute("delay") || itConnection->attribute("alpha")
                  || itConnection->attribute("linearimpedance") || itConnection->attribute("angularimpedance"))
              {
#if !defined(NO_TLM)
                double delay = itConnection->attribute("delay").as_double();
                double alpha = itConnection->attribute("alpha").as_double();
                double linearimpedance = itConnection->attribute("linearimpedance").as_double();
                double angularimpedance = itConnection->attribute("angularimpedance").as_double();

                status = addTLMConnection(element1+connector1, element2+connector2, delay, alpha, linearimpedance, angularimpedance);
#else
                return LOG_NO_TLM();
#endif
              }
              else  // Load bus connection
              {
                status = addConnection(element1+connector1, element2+connector2);
              }
              // Load the bus connection geometry
              if (oms_status_ok != status)
                return logError("Failed to import " + std::string(oms::ssp::Draft20180219::bus_connection));
              else
              {
                // Load connection geometry
                if (oms_status_ok != importFromSSD_ConnectionGeometry(*itConnection, element1+connector1, element2+connector2))
                  return logError("Failed to import " + std::string(oms::ssp::Draft20180219::ssd::connection_geometry));
              }
            }
          }
        }
      }
    }
    else
      return logError_WrongSchema(name);
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::addConnector(const oms::ComRef& cref, oms_causality_enu_t causality, oms_signal_type_enu_t type)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addConnector(tail, causality, type);

  if (this->type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  connectors.back() = new oms::Connector(causality, type, cref, this->getFullCref());
  exportConnectors[getFullCref() + connectors.back()->getName()] = true;
  connectors.push_back(NULL);
  element.setConnectors(&connectors[0]);

  return oms_status_ok;
}

oms::Connector* oms::System::getConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getConnector(tail);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getConnector(tail);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref)
      return connector;

  return nullptr;
}

std::string oms::System::getConnectorOwner(const oms::ComRef& cref) const
{
  return cref.isValidIdent() ? "System" : "Element";
}

oms::BusConnector* oms::System::getBusConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getBusConnector(tail);

  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return nullptr;
  }

  for(auto& busconnector : busconnectors)
    if (busconnector && busconnector->getName() == cref)
      return busconnector;

  return nullptr;
}

#if !defined(NO_TLM)
oms::TLMBusConnector* oms::System::getTLMBusConnector(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getTLMBusConnector(tail);
  auto component = components.find(head);
  if (component != components.end())
    return component->second->getTLMBusConnector(tail);

  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return nullptr;
  }

  for(auto& tlmbusconnector : tlmbusconnectors)
    if (tlmbusconnector && tlmbusconnector->getName() == cref)
      return tlmbusconnector;

  return nullptr;
}
#endif

oms::Connection** oms::System::getConnections(const oms::ComRef& cref)
{
  if (!cref.isEmpty())
  {
    oms::ComRef tail(cref);
    oms::ComRef head = tail.pop_front();
    auto subsystem = subsystems.find(head);
    if (subsystem != subsystems.end())
      return subsystem->second->getConnections(tail);
  }

  return &connections[0];
}

oms_status_enu_t oms::System::addConnection(const oms::ComRef& crefA, const oms::ComRef& crefB, bool suppressUnitConversion)
{
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  // if both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnection(tailA, tailB, suppressUnitConversion);
  }

#if !defined(NO_TLM)
  // check if it is an internal connection between TLM buses
  TLMBusConnector* tlmA = getTLMBusConnector(crefA);
  TLMBusConnector* tlmB = getTLMBusConnector(crefB);
  if(tlmA && tlmB)
  {
    if (getConnection(crefA, crefB))
      return logError_ConnectionExistsAlready(crefA, crefB, this);

    // create connection between TLM buses (NOT a TLM connection)
    connections.back() = new oms::Connection(crefA, crefB, suppressUnitConversion, oms_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }
#endif

  // first check if it is a bus connection
  BusConnector* busA = NULL;
  auto subsystemA = subsystems.find(headA);
  if (subsystemA != subsystems.end())
    busA = subsystemA->second->getBusConnector(tailA);

  BusConnector* busB = NULL;
  auto subsystemB = subsystems.find(headB);
  if (subsystemB != subsystems.end())
    busB = subsystemB->second->getBusConnector(tailB);

  if (busA && busB)
  {
    if (getConnection(crefA, crefB))
      return logError_ConnectionExistsAlready(crefA, crefB, this);

    // create bus connection
    connections.back() = new oms::Connection(crefA, crefB, suppressUnitConversion, oms_connection_bus);
    connections.push_back(NULL);
    return oms_status_ok;
  }

  // not a bus connection, attempt normal connection
  oms::Connector* conA = this->getConnector(crefA);
  if (!conA) return logError_ConnectorNotInSystem(crefA, this);
  oms::Connector* conB = this->getConnector(crefB);
  if (!conB) return logError_ConnectorNotInSystem(crefB, this);

  if (getConnection(crefA, crefB))
    return logError_ConnectionExistsAlready(crefA, crefB, this);

  auto componentA = getComponent(headA);
  if(componentA && componentA->getType() == oms_component_table)
  {
    // allow non-real connections to tables
  }
  else if((conA->getType() == oms_signal_type_integer && conB->getType() == oms_signal_type_enum) || (conA->getType() == oms_signal_type_enum && conB->getType() == oms_signal_type_integer))
  {
    // allow integer connection to enum types
  }
  else if (conA->getType() != conB->getType())
  {
    return logError("Type mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));
  }

  // Do not allow multiple connections to same 'input' connector
  // (signal B). The 'input' connector (signal B) can actually be an
  // output connector, e.g. if connecting a component to a system
  // connector.
  for (auto& connection : connections)
  {
    if (connection && connection->containsSignalB(crefB))
      return logError("Connector " + std::string(crefB) + " is already connected to " + std::string(connection->getSignalA()));
  }

  // check units in connection
  if (!oms::Connection::isValidUnits(crefA, crefB, *conA, *conB))
  {
    return logError("Unit mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));
  }

  // check if the connections are valid, according to the SSP-1.0 allowed connection table
  if (oms::Connection::isValid(crefA, crefB, *conA, *conB))
  {
    connections.back() = new oms::Connection(crefA, crefB, suppressUnitConversion);
    connections.push_back(NULL);
  }
  // flipped causality check
  else if (oms::Connection::isValid(crefB, crefA, *conB, *conA))
  {
    // ! Flipped connection checks !
    // Do not allow multiple connections to same 'input' connector
    // (signal A!). The 'input' connector (signal A!) can actually be
    // an output connector, e.g. if connecting a component to a system
    // connector.
    for (auto &connection : connections)
    {
      if (connection && connection->containsSignalB(crefA))
        return logError("Connector " + std::string(crefA) + " is already connected to " + std::string(connection->getSignalA()));
    }
    connections.back() = new oms::Connection(crefB, crefA, suppressUnitConversion);
    connections.push_back(NULL);
  }
  else
    return logError("Causality mismatch in connection: " + std::string(crefA) + " -> " + std::string(crefB));

  return oms_status_ok;
}

oms_status_enu_t oms::System::deleteConnection(const oms::ComRef& crefA, const oms::ComRef& crefB)
{
  for (auto& connection : connections)
  {
    if (connection && connection->isEqual(crefA, crefB))
    {
      delete connection;

      connections.pop_back();   // last element is always NULL
      connection = connections.back();
      connections.back() = NULL;

      return oms_status_ok;
    }
  }

  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->deleteConnection(tailA, tailB);
  }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
}

oms_status_enu_t oms::System::addTLMConnection(const oms::ComRef& crefA, const oms::ComRef& crefB, double delay, double alpha, double linearimpedance, double angularimpedance)
{
#if !defined(NO_TLM)
  if (type != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->addTLMConnection(tailA,tailB,delay,alpha,linearimpedance,angularimpedance);
  }

  TLMBusConnector *busA=0, *busB=0;
  auto subsystemA = getSystem(headA);
  if(subsystemA)
    busA = subsystemA->getTLMBusConnector(tailA);
  if(!busA) {
    auto componentA = getComponent(headA);
    if(componentA)
      busA = componentA->getTLMBusConnector(tailA);
  }

  auto subsystemB = getSystem(headB);
  if(subsystemB)
    busB = subsystemB->getTLMBusConnector(tailB);
  if(!busB) {
    auto componentB = getComponent(headB);
    if(componentB)
      busB = componentB->getTLMBusConnector(tailB);
  }

  if (busA && busB)
  {
    //Create bus connection
    connections.back() = new oms::Connection(crefA, crefB, false, oms_connection_tlm);
    connections.back()->setTLMParameters(delay, alpha, linearimpedance, angularimpedance);
    connections.push_back(NULL);
    busA->setDelay(delay);
    busB->setDelay(delay);
    return oms_status_ok;
  }

  return logError("TLM bus connector(s) not found in system");
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addBus(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addBus(tail);

  auto component = components.find(head);
  if (component != components.end())
    return logError("Bus connectors cannot be added to components"); ///< TODO: Implement bus connectors for components

  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  oms::BusConnector* bus = new oms::BusConnector(cref);
  busconnectors.back() = bus;
  busconnectors.push_back(NULL);
  element.setBusConnectors(&busconnectors[0]);
  return oms_status_ok;
}

oms_status_enu_t oms::System::addTLMBus(const oms::ComRef& cref, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->addTLMBus(tail, domain, dimensions, interpolation);

  auto component = components.find(head);
  if (component != components.end())
  {
    return component->second->addTLMBus(tail, domain, dimensions, interpolation);
  }

  if (type == oms_system_tlm)
    return logError_NotForTlmSystem;
  if (!cref.isValidIdent())
    return logError_InvalidIdent(cref);
  if (!validCref(cref))
    return logError_AlreadyInScope(getFullCref() + cref);

  oms::TLMBusConnector* bus = new oms::TLMBusConnector(cref, domain, dimensions, interpolation, this);
  tlmbusconnectors.back() = bus;
  tlmbusconnectors.push_back(NULL);
  element.setTLMBusConnectors(&tlmbusconnectors[0]);
  return oms_status_ok;
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addConnectorToBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToBus(busTail,connectorTail);
  }

  if (!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);

  for(auto& bus : busconnectors)
    if (bus && bus->getName() == busCref)
      return bus->addConnector(connectorCref);

  return logError_BusNotInSystem(busCref, this);
}

oms_status_enu_t oms::System::deleteConnectorFromBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);

  for(auto& bus : busconnectors)
    if(bus && bus->getName() == busCref)
      return bus->deleteConnector(connectorCref);

  return logError_BusNotInSystem(busCref, this);
}

oms_status_enu_t oms::System::addConnectorToTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref, const std::string type)
{
#if !defined(NO_TLM)
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if (busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if (subsystem != subsystems.end())
      return subsystem->second->addConnectorToTLMBus(busTail,connectorTail,type);

    auto component = components.find(busHead);
    if (component != components.end())
      return component->second->addConnectorToTLMBus(busTail,connectorTail,type);
  }

  //Check that connector exists in system
  bool found = false;
  for(auto& connector : connectors)
    if (connector && connector->getName() == connectorCref)
      found = true;
  if (!found)
    return logError_ConnectorNotInSystem(connectorCref, this);

  for(auto& bus : tlmbusconnectors)
  {
    if (bus && bus->getName() == busCref)
      return bus->addConnector(connectorCref,type);
  }

  return logError_TlmBusNotInSystem(busCref, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::deleteConnectorFromTLMBus(const oms::ComRef& busCref, const oms::ComRef& connectorCref)
{
#if !defined(NO_TLM)
  oms::ComRef busTail(busCref);
  oms::ComRef busHead = busTail.pop_front();
  oms::ComRef connectorTail(connectorCref);
  oms::ComRef connectorHead = connectorTail.pop_front();
  //If both bus and connector references the same subsystem, recurse into that subsystem
  if(busHead == connectorHead)
  {
    auto subsystem = subsystems.find(busHead);
    if(subsystem != subsystems.end())
      return subsystem->second->deleteConnectorFromTLMBus(busTail,connectorTail);

    auto component = components.find(busHead);
    if(component != components.end())
      return component->second->deleteConnectorFromTLMBus(busTail,connectorTail);
  }

  if(!busTail.isEmpty() && !connectorTail.isEmpty() && busHead != connectorHead)
    return logError_BusAndConnectorNotSameSystem(busCref, connectorCref);
  for(auto& bus : tlmbusconnectors)
    if(bus && bus->getName() == busCref)
      return bus->deleteConnector(connectorCref);

  return logError_TlmBusNotInSystem(busCref, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::addExternalModel(const oms::ComRef& cref, std::string path, std::string startscript)
{
#if !defined(NO_TLM)
  if (type != oms_system_tlm)
    return logError_OnlyForSystemTLM;

  if (!cref.isValidIdent())
    return oms_status_error;

  oms::ExternalModel* externalmodel = oms::ExternalModel::NewComponent(cref, this, path, startscript);
  if (!externalmodel)
    return oms_status_error;

  components[cref] = externalmodel;
  subelements.back() = reinterpret_cast<oms_element_t*>(externalmodel->getElement());
  subelements.push_back(NULL);
  element.setSubElements(&subelements[0]);

  return oms_status_ok;
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::setConnectorGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setConnectorGeometry(tail,geometry);

  auto component = components.find(head);
  if (component != components.end())
  {
    oms::Connector *connector = component->second->getConnector(tail);
    if (connector)
    {
      connector->setGeometry(geometry);
      return oms_status_ok;
    }
    else {
      return logError("Connector " + std::string(tail) + " not found in component " + std::string(head));
    }
  }

  oms::Connector* connector = this->getConnector(cref);
  if (connector)
  {
    connector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Connector " + std::string(cref) + " not found in system " + std::string(getCref()));
}

oms_status_enu_t oms::System::setConnectionGeometry(const oms::ComRef& crefA, const oms::ComRef& crefB, const oms::ssd::ConnectionGeometry *geometry)
{
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->setConnectionGeometry(tailA,tailB,geometry);
  }

  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
    {
      bool inverse = connection->isStrictEqual(crefB, crefA);
      connection->setGeometry(geometry, inverse);
      return oms_status_ok;
    }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
}


oms_status_enu_t oms::System::setTLMConnectionParameters(const ComRef &crefA, const ComRef &crefB, const oms_tlm_connection_parameters_t* parameters)
{
#if !defined(NO_TLM)
  oms::ComRef tailA(crefA);
  oms::ComRef headA = tailA.pop_front();

  oms::ComRef tailB(crefB);
  oms::ComRef headB = tailB.pop_front();

  //If both A and B references the same subsystem, recurse into that subsystem
  if (headA == headB)
  {
    auto subsystem = subsystems.find(headA);
    if (subsystem != subsystems.end())
      return subsystem->second->setTLMConnectionParameters(tailA, tailB, parameters);
  }

  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
    {
      connection->setTLMParameters(parameters);
      return oms_status_ok;
    }

  return logError_ConnectionNotInSystem(crefA, crefB, this);
#else
    return LOG_NO_TLM();
#endif
}

oms_status_enu_t oms::System::setBusGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBusGeometry(tail,geometry);

  oms::BusConnector* busConnector = this->getBusConnector(cref);
  if (busConnector)
  {
    busConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
}

oms_status_enu_t oms::System::setTLMBusGeometry(const oms::ComRef& cref, const oms::ssd::ConnectorGeometry *geometry)
{
#if !defined(NO_TLM)
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();
  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setTLMBusGeometry(tail,geometry);

  oms::TLMBusConnector* tlmBusConnector = this->getTLMBusConnector(cref);
  if (tlmBusConnector)
  {
    tlmBusConnector->setGeometry(geometry);
    return oms_status_ok;
  }
  return logError("TLM Bus " + std::string(cref)+" not found in system " + std::string(getCref()));
#else
    return LOG_NO_TLM();
#endif
}

oms::Connection* oms::System::getConnection(const oms::ComRef& crefA, const oms::ComRef& crefB)
{
  for (auto& connection : connections)
    if (connection && connection->isEqual(crefA, crefB))
      return connection;
  return nullptr;
}

oms::Model& oms::System::getModel()
{
  if (parentSystem)
    return parentSystem->getModel();
  return *parentModel;
}

oms_status_enu_t oms::System::deleteAllConectionsTo(const oms::ComRef& cref)
{
  for (int i=0; i<connections.size(); ++i)
  {
    while (connections[i] && connections[i]->containsSignal(cref))
    {
      delete connections[i];

      connections.pop_back();   // last element is always NULL
      connections[i] = connections.back();
      connections.back() = NULL;
    }
  }

  return oms_status_ok;
}

bool oms::System::isConnected(const ComRef& cref) const
{
  for (int i=0; i<connections.size(); ++i)
    if (connections[i] && connections[i]->containsSignal(cref))
      return true;

  return false;
}

oms_status_enu_t oms::System::delete_(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (tail.isEmpty())
  {
    // check cref has ":start" suffix at the end to delete only start values
    if (front.hasSuffix("start"))
    {
      // check for local resources
      if (values.hasResources())
      {
        if (oms_status_ok != values.deleteStartValueInResources(front))
          return logWarning("failed to delete start value \"" + std::string(getFullCref()+front) + "\"" + " because the identifier couldn't be resolved to any system signal");
      }
      // check from top level resources
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        if (oms_status_ok != getParentSystem()->values.deleteStartValueInResources(getCref()+front))
          return logWarning("failed to delete start value \"" + std::string(getFullCref()+front) + "\"" + " because the identifier couldn't be resolved to any system signal");
      }
      // inline
      else
      {
        if (oms_status_ok != values.deleteStartValue(front))
          return logWarning("failed to delete start value \"" + std::string(getFullCref()+front) + "\"" + " because the identifier couldn't be resolved to any system signal");
      }

      return oms_status_ok;
    }

    auto subsystem = subsystems.find(front);
    if (subsystem != subsystems.end())
    {
      deleteAllConectionsTo(front);
      delete subsystem->second;
      subsystems.erase(subsystem);
      return oms_status_ok;
    }

    auto component = components.find(front);
    if (component != components.end())
    {
      //logInfo("Delete " + std::string(front));
      deleteAllConectionsTo(front);
      delete component->second;
      components.erase(component);
      return oms_status_ok;
    }

    for (int i=0; i<connectors.size()-1; ++i)
      if (connectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        if (values.hasResources()) // check for local resources
        {
          values.deleteStartValueInResources(front);
        }
        else if (getParentSystem() && getParentSystem()->values.hasResources()) // check from top level resources
        {
          getParentSystem()->values.deleteStartValueInResources(getCref() + front);
        }
        else // inline
        {
          values.deleteStartValue(front);
        }
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete connectors[i];
        connectors.pop_back();   // last element is always NULL
        connectors[i] = connectors.back();
        connectors.back() = NULL;
        return oms_status_ok;
      }

    for (int i=0; i<busconnectors.size()-1; ++i)
      if (busconnectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        if (values.hasResources()) // check for local resources
        {
          values.deleteStartValueInResources(front);
        }
        else if (getParentSystem() && getParentSystem()->values.hasResources()) // check from top level resources
        {
          getParentSystem()->values.deleteStartValueInResources(getCref() + front);
        }
        else // inline
        {
          values.deleteStartValue(front);
        }
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete busconnectors[i];
        busconnectors.pop_back();   // last element is always NULL
        busconnectors[i] = busconnectors.back();
        busconnectors.back() = NULL;
        return oms_status_ok;
      }
#if !defined(NO_TLM)
    for (int i=0; i<tlmbusconnectors.size()-1; ++i)
      if (tlmbusconnectors[i]->getName() == front)
      {
        // delete startValues associated with the Connector
        if (values.hasResources()) // check for local resources
        {
          values.deleteStartValueInResources(front);
        }
        else if (getParentSystem() && getParentSystem()->values.hasResources()) // check from top level resources
        {
          getParentSystem()->values.deleteStartValueInResources(getCref() + front);
        }
        else // inline
        {
          values.deleteStartValue(front);
        }
        deleteAllConectionsTo(front);
        exportConnectors.erase(front);
        delete tlmbusconnectors[i];
        tlmbusconnectors.pop_back();   // last element is always NULL
        tlmbusconnectors[i] = tlmbusconnectors.back();
        tlmbusconnectors.back() = NULL;
        return oms_status_ok;
      }
#endif
  }
  else
  {
    auto subsystem = subsystems.find(front);
    if (subsystem != subsystems.end())
    {
      deleteAllConectionsTo(cref);
      return subsystem->second->delete_(tail);
    }

    auto component = components.find(front);
    if (component != components.end())
    {
      // check cref has ":start" suffix at the end to delete only start values
      if (tail.hasSuffix("start"))
      {
        if (oms_status_ok != component->second->deleteStartValue(tail))
          return logWarning("failed to delete start value \"" + std::string(getFullCref()+cref) + "\"" + " because the identifier couldn't be resolved to any component signal");
        return oms_status_ok;
      }

      deleteAllConectionsTo(cref);
      return component->second->deleteConnector(tail);
    }
  }
  logWarning("failed to delete object \"" + std::string(getFullCref()+cref) + "\"" + " because the identifier couldn't be resolved to any connector, component, system, or model");
  return oms_status_error;
}

bool oms::System::copyResources()
{
  if (parentSystem)
    return parentSystem->copyResources();
  return parentModel->copyResources();
}

void oms::System::getAllResources(std::vector<std::string>& resources) const
{
  for (const auto& component : components)
    component.second->getAllResources(resources);

  for (const auto& subsystem : subsystems)
    subsystem.second->getAllResources(resources);
}

void oms::System::getFilteredSignals(std::vector<Connector>& filteredSignals) const
{
  for (const auto &connector : connectors)
  {
    if (connector && exportConnectors.at(getFullCref() + connector->getName()))
      filteredSignals.push_back(*connector);
  }

  for (const auto& component : components)
    component.second->getFilteredSignals(filteredSignals);

  for (const auto& subsystem : subsystems)
    subsystem.second->getFilteredSignals(filteredSignals);
}

oms_status_enu_t oms::System::exportDependencyGraphs(const std::string& pathInitialization, const std::string& pathEvent, const std::string& pathSimulation)
{
  oms_status_enu_t status = updateDependencyGraphs();

  initializationGraph.dotExport(pathInitialization);
  eventGraph.dotExport(pathEvent);
  simulationGraph.dotExport(pathSimulation);
  return status;
}

oms_status_enu_t oms::System::updateDependencyGraphs()
{
  initializationGraph.clear();
  eventGraph.clear();
  simulationGraph.clear();

  for (const auto& subsystem : subsystems)
  {
    if (oms_status_ok != subsystem.second->updateDependencyGraphs())
      return oms_status_error;

    initializationGraph.includeGraph(subsystem.second->getInitialUnknownsGraph(), subsystem.first);
    eventGraph.includeGraph(subsystem.second->getOutputsGraph(), subsystem.first);
    simulationGraph.includeGraph(subsystem.second->getOutputsGraph(), subsystem.first);
  }

  for (const auto& component : components)
  {
    initializationGraph.includeGraph(component.second->getInitialUnknownsGraph(), component.first);
    eventGraph.includeGraph(component.second->getOutputsGraph(), component.first);
    simulationGraph.includeGraph(component.second->getOutputsGraph(), component.first);
  }

  for (const auto& connection : connections)
  {
    if (!connection || connection->getType() != oms_connection_single)
      continue;

    Connector* varA = getConnector(connection->getSignalA());
    Connector* varB = getConnector(connection->getSignalB());
    if (varA && varB)
    {
      bool validConnection = oms::Connection::isValid(connection->getSignalA(), connection->getSignalB(), *varA, *varB);
      if (validConnection)
      {
        initializationGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA(), this->getFullCref()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB(), this->getFullCref()));
        // Don't include parameter connections in simulation dependencies
        if (!varA->isParameter())
        {
          eventGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA(), this->getFullCref()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB(), this->getFullCref()));
        }
        // allow only real connections in Continuous time mode
        if (varA->getType() == oms_signal_type_real && !varA->isParameter())
        {
          simulationGraph.addEdge(Connector(varA->getCausality(), varA->getType(), connection->getSignalA(), this->getFullCref()), Connector(varB->getCausality(), varB->getType(), connection->getSignalB(), this->getFullCref()));
        }
      }
      else
        return logError("failed for " + std::string(connection->getSignalA()) + " -> " + std::string(connection->getSignalB()));
    }
    else
      return logError("invalid connection");
  }

  // set the units, once all the egdes are added to initialization and simulationGraph
  for (const auto &connection : connections)
  {
    if (!connection || connection->getType() != oms_connection_single)
      continue;
    Connector *varA = getConnector(connection->getSignalA());
    Connector *varB = getConnector(connection->getSignalB());
    if (varA && varB)
    {
      bool validConnection = oms::Connection::isValid(connection->getSignalA(), connection->getSignalB(), *varA, *varB);
      if (validConnection)
      {
        initializationGraph.setUnits(varA, varB, connection->getSuppressUnitConversion());
        eventGraph.setUnits(varA, varB, connection->getSuppressUnitConversion());
        simulationGraph.setUnits(varA, varB, connection->getSuppressUnitConversion());
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::getBoolean(const ComRef& cref, bool& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getBoolean(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getBoolean(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeBoolean())
    {
      // getBoolean from local resources
      if (values.hasResources())
      {
        if (oms_status_ok == values.getBooleanResources(cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value of boolean ?
          return oms_status_ok;
        }
      }
      // getBoolean from top level resources
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        if (oms_status_ok == getParentSystem()->values.getBooleanResources(getCref()+cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value
          return oms_status_ok;
        }
      }
      // no resources available, getBoolean from inline
      else
      {
        if (oms_modelState_simulation == getModel().getModelState() && values.booleanValues[cref] != 0)
        {
          value = values.booleanValues[cref];
          return oms_status_ok;
        }
        if (oms_status_ok == values.getBoolean(cref, value))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value
          return oms_status_ok;
        }
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getInteger(const ComRef& cref, int& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getInteger(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getInteger(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeInteger())
    {
      // getInteger from local resources
      if (values.hasResources())
      {
        if (oms_status_ok == values.getIntegerResources(cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value
          return oms_status_ok;
        }
      }
      // getInteger from top level resources
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        if (oms_status_ok == getParentSystem()->values.getIntegerResources(getCref()+cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value
          return oms_status_ok;
        }
      }
      // no resources available, getInteger from inline
      else
      {
        if (oms_modelState_simulation == getModel().getModelState() && values.integerValues[cref] != 0)
        {
          value = values.integerValues[cref];
          return oms_status_ok;
        }
        if (oms_status_ok == values.getInteger(cref, value))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0; // default value
          return oms_status_ok;
        }
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getReal(const ComRef& cref, double& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getReal(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getReal(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeReal())
    {
      // getReal from local resources
      if (values.hasResources())
      {
        if (oms_status_ok == values.getRealResources(cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0.0; // default value
          return oms_status_ok;
        }
      }
      // getReal from top level resources
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        if (oms_status_ok == getParentSystem()->values.getRealResources(getCref()+cref, value, true, getModel().getModelState()))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0.0; // default value
          return oms_status_ok;
        }
      }
      // no resources available, getReal from inline
      else
      {
        if (oms_modelState_simulation == getModel().getModelState() && values.realValues[cref] != 0.0)
        {
          value = values.realValues[cref];
          return oms_status_ok;
        }
        if (oms_status_ok == values.getReal(cref, value))
        {
          return oms_status_ok;
        }
        else
        {
          value = 0.0; // default value
          return oms_status_ok;
        }
      }
    }
  }

  if (cref == "$wallTime")
  {
    value = clock.getElapsedWallTime();
    return oms_status_ok;
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getString(const ComRef& cref, std::string& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->getString(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getString(tail, value);

  for (auto& connector : connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeString())
    {
      // getString from local resources
      if (values.hasResources())
      {
        if (oms_status_ok != values.getStringResources(cref, value, true, getModel().getModelState()))
          value = ""; // default value
        return oms_status_ok;
      }
      // getString from top level resources
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        if (oms_status_ok != getParentSystem()->values.getStringResources(getCref() + cref, value, true, getModel().getModelState()))
          value = ""; // default value
        return oms_status_ok;
      }
      // no resources available, getString from inline
      else
      {
        if (oms_modelState_simulation == getModel().getModelState() && values.stringValues[cref] != "")
        {
          value = values.stringValues[cref];
          return oms_status_ok;
        }
        if (oms_status_ok != values.getString(cref, value))
          value = 0.0; // default value
        return oms_status_ok;
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getDirectionalDerivative(const ComRef& unknownCref, const ComRef& knownCref, double& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(unknownCref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return logError("getDirectionalDerivative is computed only for fmu signals");

  auto component = components.find(head);
  if (component != components.end())
    return component->second->getDirectionalDerivative(tail, knownCref, value);

  return logError_UnknownSignal(getFullCref() + unknownCref);
}

oms_status_enu_t oms::System::getState(const ComRef& cref)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return logError("getState is computed only for fmu signals");

  auto component = components.find(head);
  if (component != components.end())
  {
    if (component->second->getCanGetAndSetState())
      return component->second->saveState();
    else
      return logError("the fmu \"" + std::string(component->second->getFullCref()) + "\" does not support this feature as canGetAndSetState = false in modeldescription.xml");
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setState(const ComRef& cref)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return logError("setState is computed only for fmu signals");

  auto component = components.find(head);
  if (component != components.end())
  {
    if (component->second->getCanGetAndSetState())
      return component->second->restoreState();
    else
      return logError("the fmu \"" + std::string(component->second->getFullCref()) + "\" does not support this feature as canGetAndSetState = false in modeldescription.xml");
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::freeState(const ComRef& cref)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return logError("freeState is computed only for fmu signals");

  auto component = components.find(head);
  if (component != components.end())
  {
    if (component->second->getCanGetAndSetState())
      return component->second->freeState();
    else
      return logError("the fmu \"" + std::string(component->second->getFullCref()) + "\" does not support this feature as canGetAndSetState = false in modeldescription.xml");
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setBoolean(const ComRef& cref, bool value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setBoolean(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setBoolean(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeBoolean())
    {
      // check for local resources available
      if (values.hasResources())
      {
        return values.setBooleanResources(cref, value, getFullCref(), true, getModel().getModelState());
      }
      // check for resources in top level system
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        return getParentSystem()->values.setBooleanResources(getCref() + cref, value, getParentSystem()->getFullCref(), true, getModel().getModelState());
      }
      else
      {
        // set external inputs, after initialization
        if (oms_modelState_simulation == getModel().getModelState())
        {
          values.booleanValues[cref] = value;
        }
        else
        {
          values.setBoolean(cref, value);
        }
        return oms_status_ok;
      }
    }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setInteger(const ComRef& cref, int value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setInteger(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setInteger(tail, value);

  for (auto& connector : connectors)
    if (connector && connector->getName() == cref && connector->isTypeInteger())
    {
      // check for local resources available
      if (values.hasResources())
      {
        return values.setIntegerResources(cref, value, getFullCref(), true, getModel().getModelState());
      }
      // check for resources in top level system
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        return getParentSystem()->values.setIntegerResources(getCref() + cref, value, getParentSystem()->getFullCref(), true, getModel().getModelState());
      }
      else
      {
        // set external inputs, after initialization
        if (oms_modelState_simulation == getModel().getModelState())
        {
          values.integerValues[cref] = value;
        }
        else
        {
          values.setInteger(cref, value);
        }
        return oms_status_ok;
      }
    }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setReal(const ComRef& cref, double value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setReal(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setReal(tail, value);

  for (auto &connector: connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeReal())
    {
      // check for local resources available
      if (values.hasResources())
      {
        return values.setRealResources(cref, value, getFullCref(), true, getModel().getModelState());
      }
      // check for resources in top level system
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        //return getParentSystem()->setRealSystemResources(getCref()+cref, value, connector->isOutput());
        return getParentSystem()->values.setRealResources(getCref() + cref, value, getParentSystem()->getFullCref(), true, getModel().getModelState());
      }
      else
      {
        // set external inputs, after initialization
        if (oms_modelState_simulation == getModel().getModelState())
        {
          values.realValues[cref] = value;
        }
        else
        {
          values.setReal(cref, value);
        }
        return oms_status_ok;
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setString(const ComRef& cref, const std::string& value)
{
  if (!getModel().validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated|oms_modelState_initialization|oms_modelState_simulation))
    return logError_ModelInWrongState(getModel().getCref());

  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setString(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setString(tail, value);

  for (auto &connector: connectors)
  {
    if (connector && connector->getName() == cref && connector->isTypeString())
    {
      // check for local resources available
      if (values.hasResources())
      {
        return values.setStringResources(cref, value, getFullCref(), true, getModel().getModelState());
      }
      // check for resources in top level system
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        //return getParentSystem()->setRealSystemResources(getCref()+cref, value, connector->isOutput());
        return getParentSystem()->values.setStringResources(getCref() + cref, value, getParentSystem()->getFullCref(), true, getModel().getModelState());
      }
      else
      {
        // set external inputs, after initialization
        if (oms_modelState_simulation == getModel().getModelState())
          values.stringValues[cref] = value;
        else
          values.setString(cref, value);
        return oms_status_ok;
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::setUnit(const ComRef& cref, const std::string& value)
{
  oms::ComRef tail(cref);
  oms::ComRef head = tail.pop_front();

  auto subsystem = subsystems.find(head);
  if (subsystem != subsystems.end())
    return subsystem->second->setUnit(tail, value);

  auto component = components.find(head);
  if (component != components.end())
    return component->second->setUnit(tail, value);

  for (auto &connector: connectors)
  {
    if (connector && connector->getName() == cref)
    {
      // check for local resources available
      if (values.hasResources())
      {
        return values.setUnitResources(cref, value, getFullCref());
      }
      // check for resources in top level system
      else if (getParentSystem() && getParentSystem()->values.hasResources())
      {
        return getParentSystem()->values.setUnitResources(getCref() + cref, value, getParentSystem()->getFullCref());
      }
      else
      {
        values.setUnit(cref, value);
        return oms_status_ok;
      }
    }
  }

  return logError_UnknownSignal(getFullCref() + cref);
}

oms_status_enu_t oms::System::getReals(const std::vector<oms::ComRef> &sr, std::vector<double> &values)
{
  oms_status_enu_t status = oms_status_ok;
  for(int i=0; i<sr.size(); ++i) {
    status = getReal(sr[i],values[i]);
    if(status != oms_status_ok)
      break;
  }
  return status;
}

oms_status_enu_t oms::System::setReals(const std::vector<oms::ComRef> &crefs, std::vector<double> values)
{
  oms_status_enu_t status = oms_status_ok;
  for(int i=0; i<crefs.size(); ++i) {
    status = setReal(crefs[i],values[i]);
    if(status != oms_status_ok)
      break;
  }
  return status;
}

oms_status_enu_t oms::System::setRealInputDerivatives(const oms::ComRef& cref, int order, double value)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms::System::importFromSSD_ConnectionGeometry(const pugi::xml_node& node, const ComRef& crefA, const ComRef& crefB)
{
  pugi::xml_node connectionGeometryNode = node.child(oms::ssp::Draft20180219::ssd::connection_geometry);
  if (connectionGeometryNode)
  {
    oms::Connection *connection = getConnection(crefA, crefB);
    if (connection)
    {
      oms::ssd::ConnectionGeometry geometry;
      geometry.importFromSSD(connectionGeometryNode);
      connection->setGeometry(&geometry);
    }
    else
    {
      return oms_status_error;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms::System::registerSignalsForResultFile(ResultWriter& resultFile)
{
  if (Flags::WallTime())
    clock_id = resultFile.addSignal(std::string(getFullCref() + ComRef("$wallTime")), "wall-clock time [s]", SignalType_REAL);
  else
    clock_id = 0;

  for (const auto& component : components)
    if (oms_status_ok != component.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->registerSignalsForResultFile(resultFile))
      return oms_status_error;

  resultFileMapping.clear();
  for (unsigned int i=0; i<connectors.size(); ++i)
  {
    if (!connectors[i])
      continue;
    // check for full cref in exportConnectors
    if (!exportConnectors[getFullCref()+connectors[i]->getName()])
      continue;

    auto const& connector = connectors[i];

    if (oms_signal_type_real == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(getFullCref() + connector->getName()), "connector", SignalType_REAL);
      resultFileMapping[ID] = i;
    }
    else if (oms_signal_type_integer == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(connector->getName()), "connector", SignalType_INT);
      resultFileMapping[ID] = i;
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      unsigned int ID = resultFile.addSignal(std::string(connector->getName()), "connector", SignalType_BOOL);
      resultFileMapping[ID] = i;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::updateSignals(ResultWriter& resultFile)
{
  if (clock_id)
  {
    SignalValue_t wallTime;
    wallTime.realValue = clock.getElapsedWallTime();
    resultFile.updateSignal(clock_id, wallTime);
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->updateSignals(resultFile))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->updateSignals(resultFile))
      return oms_status_error;

  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    auto const& connector = connectors[it.second];
    SignalValue_t value;
    if (oms_signal_type_real == connector->getType())
    {
      if (oms_status_ok != getReal(connector->getName(), value.realValue))
        return logError("failed to fetch connector " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_integer == connector->getType())
    {
      if (oms_status_ok != getInteger(connector->getName(), value.intValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
    else if (oms_signal_type_boolean == connector->getType())
    {
      if (oms_status_ok != getBoolean(connector->getName(), value.boolValue))
        return logError("failed to fetch variable " + std::string(connector->getName()));
      resultFile.updateSignal(ID, value);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::addSignalsToResults(const char* regex)
{
  try
  {
    std::regex exp(regex);
    for (auto& x: exportConnectors)
    {
      if (x.second)
        continue;

      if (regex_match(std::string(x.first), exp))
      {
        //logInfo("added \"" + std::string(x.first) + "\" to results");
        x.second = true;
      }
    }
  }
  catch (std::regex_error& e)
  {
    return logError("invalid regular expression");
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->addSignalsToResults(regex))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->addSignalsToResults(regex))
      return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::System::removeSignalsFromResults(const char* regex)
{
  try
  {
    std::regex exp(regex);
    for (auto& x: exportConnectors)
    {
      if (!x.second)
        continue;

      if (regex_match(std::string(x.first), exp))
      {
        //logInfo("removed \"" + std::string(x.first) + "\" from results");
        x.second = false;
      }
    }
  }
  catch (std::regex_error& e)
  {
    return logError("invalid regular expression");
  }

  for (const auto& component : components)
    if (oms_status_ok != component.second->removeSignalsFromResults(regex))
      return oms_status_error;

  for (const auto& subsystem : subsystems)
    if (oms_status_ok != subsystem.second->removeSignalsFromResults(regex))
      return oms_status_error;

  return oms_status_ok;
}

bool oms::System::useThreadPool()
{
  return getModel().useThreadPool();
}

ctpl::thread_pool& oms::System::getThreadPool()
{
  return getModel().getThreadPool();
}

std::string oms::System::getUniqueID() const
{
  static unsigned int id = 0;
  std::string str = std::to_string(++id);

  while (str.length() < 4)
    str = "0" + str;

  return str;
}

oms_status_enu_t oms::System::setFaultInjection(const oms::ComRef& signal, oms_fault_type_enu_t faultType, double faultValue)
{
  oms::System* system = NULL;

  oms::ComRef tail(signal);
  oms::ComRef front = tail.pop_front();

  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
    return subsystem->second->setFaultInjection(tail, faultType, faultValue);

  auto component = components.find(front);
  if (component != components.end())
    return component->second->setFaultInjection(tail, faultType, faultValue);

  return oms_status_error;
}

oms_status_enu_t oms::System::importTLMBus(const pugi::xml_node& node, Component* component)
{
  std::string busname = node.attribute("name").as_string();
  std::string domainstr = node.attribute("domain").as_string();
  int dimensions = node.attribute("dimensions").as_int();
  std::string interpolationstr = node.attribute("interpolation").as_string();

  oms_tlm_interpolation_t interpolation;
  if (interpolationstr == "none")
    interpolation = oms_tlm_no_interpolation;
  else if (interpolationstr == "coarsegrained")
    interpolation = oms_tlm_coarse_grained;
  else if (interpolationstr == "finegrained")
    interpolation = oms_tlm_fine_grained;
  else
    return logError("Unsupported interpolation type: " + interpolationstr);

  oms_tlm_domain_t domain;
  if (domainstr == "input")
    domain = oms_tlm_domain_input;
  else if (domainstr == "output")
    domain = oms_tlm_domain_output;
  else if (domainstr == "mechanical")
    domain = oms_tlm_domain_mechanical;
  else if (domainstr == "rotational")
    domain = oms_tlm_domain_rotational;
  else if (domainstr == "hydraulic")
    domain = oms_tlm_domain_hydraulic;
  else if (domainstr == "electric")
    domain = oms_tlm_domain_electric;
  else
    return logError("Unsupported TLM domain: " + domainstr);

  if (component)
  {
    if (oms_status_ok != component->addTLMBus(busname, domain, dimensions, interpolation))
      return oms_status_error;
  }
  else
  {
    if (oms_status_ok != addTLMBus(busname, domain, dimensions, interpolation))
      return oms_status_error;
  }

  return oms_status_ok;
}


oms_status_enu_t oms::System::importBusConnectorSignals(const pugi::xml_node& node)
{
  std::string busname = node.attribute("name").as_string();
  //Load bus connector signals
  pugi::xml_node signals_node = node.child(oms::ssp::Draft20180219::signals);

  if (signals_node)
  {
    for(pugi::xml_node_iterator itSignals = signals_node.begin(); itSignals != signals_node.end(); ++itSignals)
    {
      std::string name = itSignals->name();
      if (name == oms::ssp::Draft20180219::signal)
      {
        std::string signalname = itSignals->attribute("name").as_string();
        if (std::string(node.attribute("type").as_string()) == "tlm")
        {
          std::string signaltype = itSignals->attribute("type").as_string();
          addConnectorToTLMBus(busname, signalname, signaltype);
        }
        else
          addConnectorToBus(busname, signalname);
      }
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::System::importBusConnectorGeometry(const pugi::xml_node& node)
{
  std::string busname = node.attribute("name").as_string();
  //Load bus connector signals
  pugi::xml_node connectorGeometryNode = node.child(oms::ssp::Draft20180219::ssd::connector_geometry);

  if (connectorGeometryNode)
  {
    oms::ssd::ConnectorGeometry geometry(0.0, 0.0);
    geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
    if (std::string(node.attribute("type").as_string()) == "tlm")
    {
#if !defined(NO_TLM)
      oms::TLMBusConnector* tlmBusConnector = getTLMBusConnector(busname);
      if (tlmBusConnector)
        tlmBusConnector->setGeometry(&geometry);
#else
      return LOG_NO_TLM();
#endif
    }
    else
    {
      oms::BusConnector* busConnector = getBusConnector(busname);
      if (busConnector)
        busConnector->setGeometry(&geometry);
    }
  }

  return oms_status_ok;
}

oms::AlgLoop* oms::System::getAlgLoop(const int systemNumber)
{
  if (systemNumber > algLoops.size()-1 || systemNumber < 0)
  {
    logError("Invalid system number for algebraic loop.");
    return nullptr;
  }

  return &algLoops[systemNumber];
}

oms_status_enu_t oms::System::addAlgLoop(scc_t SCC, const int algLoopNum, DirectedGraph& graph, bool supportsDirectionalDerivatives)
{
  if (loopsNeedUpdate)
  {
    algLoops.clear();
    loopsNeedUpdate = false;
  }

  algLoops.push_back( AlgLoop(Flags::AlgLoopSolver(), absoluteTolerance, relativeTolerance, SCC, algLoopNum, supportsDirectionalDerivatives));

  return oms_status_ok;
}

/*
 * check all Strongly Connected fmus have providesDirectionalDerivative = true or false, to be used by kinsolSolver
 * returns true if all fmu have providesDirectionalDerivative = "true" else "false"
*/
bool oms::System::supportsDirectionalDerivatives(int i, DirectedGraph& graph)
{
  // get Strongly connected FMU's
  std::set<oms::ComRef> scc_component_names = graph.getSortedConnections()[i].component_names;

  for (const auto &component : components)
    if (oms_component_fmu == component.second->getType())
      // check if FMU is part of the strongly connected system (alg. loop)
      if (std::find(scc_component_names.begin(), scc_component_names.end(), (getFullCref() + component.first)) != scc_component_names.end())
        if (!component.second->getFMUInfo()->getProvidesDirectionalDerivative())
          return false;
  return true;
}

oms_status_enu_t oms::System::updateAlgebraicLoops(const std::vector<scc_t>& sortedConnections, DirectedGraph& graph)
{
  // instantiate loops
  if (loopsNeedUpdate)
  {
    int systCount = 0;
    for(int i=0; i<sortedConnections.size(); i++)
    {
      if (sortedConnections[i].thisIsALoop)
      {
        addAlgLoop(sortedConnections[i], systCount, graph, supportsDirectionalDerivatives(i, graph));
        systCount++;
      }
    }
    loopsNeedUpdate = false;
  }

  return oms_status_ok;
}


oms_status_enu_t oms::System::solveAlgLoop(DirectedGraph& graph, int loopNumber, double tolerance)
{
  return algLoops[loopNumber].solveAlgLoop(*this, graph, tolerance);
}

oms_status_enu_t oms::System::rename(const oms::ComRef& newCref)
{
  this->cref = newCref;
  this->renameConnectors();

  // update subsystem connector owner
  for (const auto &it : subsystems)
    it.second->renameConnectors();

  return oms_status_ok;
}

oms_status_enu_t oms::System::rename(const ComRef& cref, const ComRef& newCref)
{
  // renaming the system itself
  if (cref.isEmpty())
    return this->rename(newCref);

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  // rename subsystems
  auto subsystem = subsystems.find(front);
  if (subsystem != subsystems.end())
  {
    subsystem->second->rename(tail, newCref);
    // rename values
    if (subsystem->second->values.hasResources()) // check for local resources
    {
      subsystem->second->values.renameInResources(cref, newCref);
    }
    // top level root resources
    else if(subsystem->second->getParentSystem() && subsystem->second->getParentSystem()->values.hasResources())
    {
      subsystem->second->getParentSystem()->values.renameInResources(cref, newCref);
    }
    // inline resources
    else
    {
      subsystem->second->values.rename(cref, newCref);
    }
    this->renameConnections(cref, newCref);
    subsystems[newCref] = subsystem->second;
    subsystems.erase(subsystem);  // delete the old cref from the lookup
    return oms_status_ok;
  }

  // rename components
  if (!tail.isEmpty())
    return logError("renaming of parts within a component isn't implemented");

  // rename components
  auto component = components.find(front);
  if (component != components.end())
  {
    component->second->rename(newCref);
    this->renameConnections(cref, newCref);
    components[newCref] = component->second;
    components.erase(component);  // delete the old cref from the lookup
    return oms_status_ok;
  }

  return logError("failed for \"" + std::string(getFullCref() + cref) + "\""  + " as the identifier could not be resolved to a system or subsystem or component");
}

oms_status_enu_t oms::System::renameConnections(const ComRef &cref, const ComRef &newCref)
{
  //logInfo("renameConnections in " + std::string(getFullCref()) + ": [" + std::string(cref) + "], [" + std::string(newCref) + "]");
  for (const auto &connection : connections)
    if (connection)
      connection->rename(cref, newCref);

  return oms_status_ok;
}

oms_status_enu_t oms::System::renameConnectors()
{
  // update the connector owner with new cref
  for (const auto &connector : connectors)
  {
    if (connector)
    {
      exportConnectors[getFullCref() + connector->getName()] = exportConnectors[connector->getOwner() + connector->getName()]; // add newCref with value
      exportConnectors.erase(connector->getOwner() + connector->getName()); // remove the old look up
      connector->setOwner(getFullCref());
    }
  }

  return oms_status_ok;
}
