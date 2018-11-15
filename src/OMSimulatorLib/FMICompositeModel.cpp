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

#include "FMICompositeModel.h"

#include "Types.h"
#include "Pkg_oms2.h"
#include "ResultWriter.h"
#include "ComRef.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "SignalRef.h"
#include "Scope.h"
#include "ssd/Tags.h"
#include "Table.h"
#if !defined(NO_TLM)
#include "Plugin/PluginImplementer.h"
#endif

#if !defined(__arm__) // @adrpo: parallel stuff doesn't work on arm yet
#include "PMRChannelMaster.h"
#endif // #if !defined(__arm__)

#include <iostream>
#include <OMSBoost.h>
#include <pugixml.hpp>
#include <sstream>
#include <thread>

oms2::FMICompositeModel::FMICompositeModel(const ComRef& name)
  : oms2::CompositeModel(oms_component_fmi, name)
{
  logTrace();
  connections.push_back(NULL);
  components = NULL;
}

oms2::FMICompositeModel::~FMICompositeModel()
{
  logTrace();

  // free memory if no one else does
  deleteComponents();

  for (auto& it : solvers)
    delete it.second;

  for (auto& connection : connections)
    if (connection)
      delete connection;

  for (auto it=subModels.begin(); it != subModels.end(); it++)
    oms2::FMISubModel::deleteSubModel(it->second);
}

oms2::FMICompositeModel* oms2::FMICompositeModel::NewModel(const ComRef& name)
{
  if (!name.isValidIdent())
  {
    logError("\"" + name + "\" is not a valid model name.");
    return NULL;
  }

  oms2::FMICompositeModel *model = new oms2::FMICompositeModel(name);
  return model;
}

oms2::FMICompositeModel* oms2::FMICompositeModel::LoadModel(const pugi::xml_node& node)
{
  logTrace();

  // read model name
  std::string ident_;
  for (auto it = node.attributes_begin(); it != node.attributes_end(); ++it)
  {
    std::string name = it->name();
    if (name == "name")
      ident_ = it->value();
  }

  // create empty model
  ComRef cref_model(ident_);
  oms2::FMICompositeModel* model = oms2::FMICompositeModel::NewModel(cref_model);
  if (!model)
    return NULL;

  for (auto it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    oms_status_enu_t status = oms_status_error;

    if (name == oms2::ssd::ssd_component)
      status = model->loadSubModel(*it);
    else if (name == oms2::ssd::ssd_connections)
      status = model->loadConnections(*it);
    else if (name == "Solver")
    {
      /// \todo implement xml import for solver settings
      logWarning("[oms2::FMICompositeModel::LoadModel] \"Solver\" not implemented yet");
    }
    else if (name == oms2::ssd::ssd_element_geometry)
      status = model->loadElementGeometry(*it);

    if (oms_status_ok != status)
    {
      logError("[oms2::FMICompositeModel::LoadModel] wrong xml schema detected");
      oms2::CompositeModel::DeleteModel(model);
      return NULL;
    }
  }

  return model;
}

oms_status_enu_t oms2::FMICompositeModel::save(pugi::xml_node& node)
{
  oms_status_enu_t status = element.getGeometry()->exportToSSD(node);
  if (oms_status_ok != status)
    return status;

  for (const auto& subModel : subModels)
  {
    status = subModel.second->exportToSSD(node);
    if (oms_status_ok != status)
      return status;
  }

  pugi::xml_node nodeConnections = node.append_child(oms2::ssd::ssd_connections);
  for (const auto& connection : connections)
  {
    if (!connection)
      continue;

    pugi::xml_node connectionNode = nodeConnections.append_child(oms2::ssd::ssd_connection);
    connectionNode.append_attribute("startElement") = connection->getSignalA().getCref().toString().c_str();
    connectionNode.append_attribute("startConnector") = connection->getSignalA().getVar().c_str();
    connectionNode.append_attribute("endElement") = connection->getSignalB().getCref().toString().c_str();
    connectionNode.append_attribute("endConnector") = connection->getSignalB().getVar().c_str();

    // export ssd:ConnectionGeometry
    status = connection->getGeometry()->exportToSSD(connectionNode);
    if (oms_status_ok != status)
      return status;
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::loadElementGeometry(const pugi::xml_node& node)
{
  if (std::string(node.name()) != oms2::ssd::ssd_element_geometry)
  {
    logError("[oms2::FMICompositeModel::loadElementGeometry] failed");
    return oms_status_error;
  }

  // import ssd:ElementGeometry
  oms2::ssd::ElementGeometry geometry;
  for (auto ait = node.attributes_begin(); ait != node.attributes_end(); ++ait)
  {
    std::string name = ait->name();
    if (name == "x1") geometry.setX1(ait->as_double());
    if (name == "y1") geometry.setY1(ait->as_double());
    if (name == "x2") geometry.setX2(ait->as_double());
    if (name == "y2") geometry.setY2(ait->as_double());
    if (name == "rotation") geometry.setRotation(ait->as_double());
    if (name == "iconSource") geometry.setIconSource(ait->as_string());
    if (name == "iconRotation") geometry.setIconRotation(ait->as_double());
    if (name == "iconFlip") geometry.setIconFlip(ait->as_bool());
    if (name == "iconFixedAspectRatio") geometry.setIconFixedAspectRatio(ait->as_bool());
  }
  this->setGeometry(geometry);
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::loadConnections(const pugi::xml_node& node)
{
  if (std::string(node.name()) != oms2::ssd::ssd_connections)
  {
    logError("[oms2::FMICompositeModel::loadConnections] failed");
    return oms_status_error;
  }

  for (auto connectionNode = node.first_child(); connectionNode; connectionNode = connectionNode.next_sibling())
  {
    if (std::string(connectionNode.name()) != oms2::ssd::ssd_connection)
    {
      logError("[oms2::FMICompositeModel::loadConnection] wrong xml schema detected (3)");
      return oms_status_error;
    }

    oms2::ComRef startElement = oms2::ComRef(connectionNode.attribute("startElement").as_string());
    std::string startConnector = connectionNode.attribute("startConnector").as_string();
    oms2::ComRef endElement = oms2::ComRef(connectionNode.attribute("endElement").as_string());
    std::string endConnector = connectionNode.attribute("endConnector").as_string();
    oms2::SignalRef sigA(startElement, startConnector);
    oms2::SignalRef sigB(endElement, endConnector);
    if (oms_status_ok != this->addConnection(SignalRef(sigA), SignalRef(sigB)))
    {
      logError("[oms2::FMICompositeModel::loadConnection] wrong xml schema detected (4)");
      return oms_status_error;
    }
    oms2::Connection* connection = this->getConnection(oms2::SignalRef(sigA), oms2::SignalRef(sigB));
    if (connection)
    {
      for (pugi::xml_node child: connectionNode.children())
      {
        // import ssd:ConnectionGeometry
        if (std::string(child.name()) == oms2::ssd::ssd_connection_geometry)
        {
          oms3::ssd::ConnectionGeometry geometry;
          std::string pointsXStr = child.attribute("pointsX").as_string();
          std::istringstream pointsXStream(pointsXStr);
          std::vector<std::string> pointsXVector(std::istream_iterator<std::string>{pointsXStream}, std::istream_iterator<std::string>());

          std::string pointsYStr = child.attribute("pointsY").as_string();
          std::istringstream pointsYStream(pointsYStr);
          std::vector<std::string> pointsYVector(std::istream_iterator<std::string>{pointsYStream}, std::istream_iterator<std::string>());

          if (pointsXVector.size() != pointsYVector.size())
          {
            logError("[oms2::FMICompositeModel::loadConnection] wrong xml schema detected (2)");
            return oms_status_error;
          }

          double* pointsX = new double[pointsXVector.size()];
          int i = 0;
          for ( auto &px : pointsXVector ) {
            pointsX[i++] = std::atof(px.c_str());
          }

          double* pointsY = new double[pointsYVector.size()];
          i = 0;
          for ( auto &py : pointsYVector ) {
            pointsY[i++] = std::atof(py.c_str());
          }

          geometry.setPoints(pointsXVector.size(), pointsX, pointsY);
          connection->setGeometry(&geometry);

          delete[] pointsX;
          delete[] pointsY;
        }
      }
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::loadSubModel(const pugi::xml_node& node)
{
  if (std::string(node.name()) != oms2::ssd::ssd_component)
  {
    logError("[oms2::FMICompositeModel::loadSubModel] failed");
    return oms_status_error;
  }

  std::string instancename;
  std::string filename;
  std::string type;
  for (auto ait = node.attributes_begin(); ait != node.attributes_end(); ++ait)
  {
    std::string name = ait->name();
    if (name == "name")
    {
      instancename = ait->value();
    }
    else if (name == "source")
    {
      filename = ait->value();
    }
    else if (name == "type")
    {
      type = ait->value();
    }
  }

  oms2::ComRef cref_model = getName();
  oms2::ComRef cref_submodel = cref_model + ComRef(instancename);

  oms_status_enu_t status = oms_status_error;
  if (type == "application/x-fmu-sharedlibrary")
    status = this->addFMU(filename, cref_submodel.last());
  else
    status = this->addTable(filename, cref_submodel.last());

  if (oms_status_ok != status)
    return status;

  oms2::FMISubModel* subModel = this->getSubModel(cref_submodel);
  if (!subModel)
    return oms_status_error;

  for (auto child = node.first_child(); child; child = child.next_sibling())
  {
    // import ssd:ElementGeometry
    if (std::string(child.name()) == oms2::ssd::ssd_element_geometry)
    {
      oms2::ssd::ElementGeometry geometry;
      double x1 = child.attribute("x1").as_double();
      double y1 = child.attribute("y1").as_double();
      double x2 = child.attribute("x2").as_double();
      double y2 = child.attribute("y2").as_double();
      geometry.setSizePosition(x1, y1, x2, y2);

      double rotation = child.attribute("rotation").as_double();
      geometry.setRotation(rotation);

      std::string iconSource = child.attribute("iconSource").as_string();
      geometry.setIconSource(iconSource);

      double iconRotation = child.attribute("iconRotation").as_double();
      geometry.setIconRotation(iconRotation);

      bool iconFlip = child.attribute("iconFlip").as_bool();
      geometry.setIconFlip(iconFlip);

      bool iconFixedAspectRatio = child.attribute("iconFixedAspectRatio").as_bool();
      geometry.setIconFixedAspectRatio(iconFixedAspectRatio);

      subModel->setGeometry(geometry);
    }
    // import connectors
    else if (std::string(child.name()) == oms2::ssd::ssd_connectors)
    {
      for (auto connectorNode = child.first_child(); connectorNode; connectorNode = connectorNode.next_sibling())
      {
        if (std::string(connectorNode.name()) == oms2::ssd::ssd_connector) {
          oms2::Element* element = subModel->getElement();
          if (element)
          {
            oms2::Connector** connectors = element->getConnectors();
            for (int i=0; connectors[i]; ++i)
            {
              if (connectors[i]->getName().getVar() == connectorNode.attribute("name").as_string())
              {
                auto connectorGeometryNode = connectorNode.first_child();
                if (std::string(connectorGeometryNode.name()) == oms2::ssd::ssd_connector_geometry)
                {
                  oms2::ssd::ConnectorGeometry geometry(0.0, 0.0);
                  geometry.setPosition(connectorGeometryNode.attribute("x").as_double(), connectorGeometryNode.attribute("y").as_double());
                  connectors[i]->setGeometry(&geometry);
                }
                break;  // since we got the connector we are looking for so quit the loop.
              }
            }
          }
        }
      }
    }
    // import parameters
    else if (std::string(child.name()) == "Parameter")
    {
      std::string _type = child.attribute("Type").as_string();
      std::string _name = child.attribute("Name").as_string();

      if (_type == "Real")
      {
        double realValue = child.attribute("Value").as_double();
        if (oms_status_ok != this->setRealParameter(oms2::SignalRef(cref_submodel, _name), realValue))
        {
          logError("[oms2::FMICompositeModel::loadSubModel] wrong xml schema detected (2)");
          return oms_status_error;
        }
      }
      else if (_type == "Integer")
      {
        int intValue = child.attribute("Value").as_int();
        if (oms_status_ok != this->setIntegerParameter(oms2::SignalRef(cref_submodel, _name), intValue))
        {
          logError("[oms2::FMICompositeModel::loadSubModel] wrong xml schema detected (2)");
          return oms_status_error;
        }
      }
      else if (_type == "Boolean")
      {
        int booleanValue = child.attribute("Value").as_int();
        if (oms_status_ok != this->setBooleanParameter(oms2::SignalRef(cref_submodel, _name), booleanValue))
        {
          logError("[oms2::FMICompositeModel::loadSubModel] wrong xml schema detected (2)");
          return oms_status_error;
        }
      }
      else
      {
        logError("[oms2::FMICompositeModel::loadSubModel] unsupported parameter type " + _type);
        return oms_status_error;
      }
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::addFMU(const std::string& filename, const oms2::ComRef& cref)
{
  if (!validAndUnusedCref(cref, true))
    return logError("[oms2::FMICompositeModel::addFMU] invalid fmu identifier");

  oms2::ComRef parent = getName();
  oms2::FMUWrapper* subModel = oms2::FMUWrapper::newSubModel(cref, filename, this->getName());
  if (!subModel)
    return oms_status_error;

  deleteComponents();

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::addTable(const std::string& filename, const oms2::ComRef& cref)
{
  if (!validAndUnusedCref(cref, true))
    return logError("[oms2::FMICompositeModel::addTable] invalid table identifier");

  oms2::ComRef parent = getName();
  oms2::Table* subModel = oms2::Table::newSubModel(cref, filename);
  if (!subModel)
    return oms_status_error;

  deleteComponents();

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::deleteSubModel(const oms2::ComRef& cref)
{
  // case 1: <cref> is submodel
  {
    auto it = subModels.find(cref);
    if (it != subModels.end())
    {
      oms2::FMISubModel::deleteSubModel(it->second);
      subModels.erase(it);

      // delete associated connections
      for (int i=0; i<connections.size()-1; ++i)
      {
        if (!connections[i])
          return logError("[oms2::FMICompositeModel::deleteSubModel] null pointer");
        else if(connections[i]->getSignalA().getCref() == cref)
        {
          delete connections[i];
          connections.pop_back();   // last element is always NULL
          connections[i] = connections.back();
          connections.back() = NULL;
          i--;
        }
        else if(connections[i]->getSignalB().getCref() == cref)
        {
          delete connections[i];
          connections.pop_back();   // last element is always NULL
          connections[i] = connections.back();
          connections.back() = NULL;
          i--;
        }
      }

      deleteComponents();
      return oms_status_ok;
    }
  }

  // case 2: <cref> is solver
  {
    auto it = solvers.find(cref);
    if (it != solvers.end())
    {
      delete it->second;
      solvers.erase(it);
      return oms_status_ok;
    }
  }

  return logError("No submodel called \"" + cref + "\" instantiated.");
}

oms_status_enu_t oms2::FMICompositeModel::setRealParameter(const oms2::SignalRef& sr, double value)
{
  auto it = subModels.find(sr.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + sr.getCref() + "\" found.");
    return oms_status_error;
  }

  if (oms_component_fmu_old != it->second->getType())
  {
    logError("[oms2::FMICompositeModel::setRealParameter] can only be used for FMUs");
    return oms_status_error;
  }

  FMUWrapper* fmu = dynamic_cast<FMUWrapper*>(it->second);
  return fmu->setRealParameter(sr.getVar(), value);
}

oms_status_enu_t oms2::FMICompositeModel::setIntegerParameter(const oms2::SignalRef& sr, int value)
{
  auto it = subModels.find(sr.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + sr.getCref() + "\" found.");
    return oms_status_error;
  }

  if (oms_component_fmu_old != it->second->getType())
  {
    logError("[oms2::FMICompositeModel::setIntegerParameter] can only be used for FMUs");
    return oms_status_error;
  }

  FMUWrapper* fmu = dynamic_cast<FMUWrapper*>(it->second);
  return fmu->setIntegerParameter(sr.getVar(), value);
}

oms_status_enu_t oms2::FMICompositeModel::setBooleanParameter(const oms2::SignalRef& sr, int value)
{
  auto it = subModels.find(sr.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + sr.getCref() + "\" found.");
    return oms_status_error;
  }

  if (oms_component_fmu_old != it->second->getType())
  {
    logError("[oms2::FMICompositeModel::setBooleanParameter] can only be used for FMUs");
    return oms_status_error;
  }

  FMUWrapper* fmu = dynamic_cast<FMUWrapper*>(it->second);
  return fmu->setBooleanParameter(sr.getVar(), value);
}

oms2::Connection* oms2::FMICompositeModel::getConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::ComRef parent = getName();
  for (auto& it : connections)
    if (it && it->isEqual(parent, conA, conB))
      return it;
  return NULL;
}

oms_status_enu_t oms2::FMICompositeModel::addConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::Variable *varA = getVariable(conA);
  oms2::Variable *varB = getVariable(conB);
  oms2::ComRef parent = getName();

  if (varA && varB)
  {
    if (varA->isOutput() && varB->isInput())
    {
      connections.back() = new oms2::Connection(parent, conA, conB);
      connections.push_back(NULL);
      return oms_status_ok;
    }
    else if (varB->isOutput() && varA->isInput())
    {
      connections.back() = new oms2::Connection(parent, conB, conA);
      connections.push_back(NULL);
      return oms_status_ok;
    }
  }

  logError("[oms2::FMICompositeModel::addConnection] failed for " + conA.toString() + " -> " + conB.toString());
  return oms_status_error;
}

oms_status_enu_t oms2::FMICompositeModel::deleteConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::ComRef parent = getName();
  for (auto& it : connections)
  {
    if (it && it->isEqual(parent, conA, conB))
    {
      delete it;

      connections.pop_back();   // last element is always NULL
      it = connections.back();
      connections.back() = NULL;

      return oms_status_ok;
    }
  }

  return oms_status_error;
}

oms2::FMISubModel* oms2::FMICompositeModel::getSubModel(const oms2::ComRef& cref, bool showWarning)
{
  auto it = subModels.find(cref.last());
  if (it == subModels.end())
  {
    if (showWarning)
      logWarning("composite model \"" + getName() + "\" doesn't contain a submodel called \"" + cref + "\"");
    return NULL;
  }

  return it->second;
}

oms2::Solver* oms2::FMICompositeModel::getSolver(const oms2::ComRef& cref, bool showWarning)
{
  auto it = solvers.find(cref.last());
  if (it == solvers.end())
  {
    if (showWarning)
      logWarning("composite model \"" + getName() + "\" doesn't contain a solver called \"" + cref + "\"");
    return NULL;
  }

  return it->second;
}

void oms2::FMICompositeModel::deleteComponents()
{
  logTrace();

  if (this->components)
  {
    delete[] components;
    components = NULL;
  }
}

void oms2::FMICompositeModel::updateComponents()
{
  logTrace();

  deleteComponents();

  components = new oms2::Element*[subModels.size() + 1];
  components[subModels.size()] = NULL;

  int i=0;
  for (auto& it : subModels)
    components[i++] = it.second->getElement();
}

oms_status_enu_t oms2::FMICompositeModel::renameSubModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  oms2::ComRef identOld_ = identOld.last();
  oms2::ComRef identNew_ = identNew.last();

  if (!identNew_.isValidIdent())
  {
    logError("Identifier \"" + identNew + "\" is invalid.");
    return oms_status_error;
  }

  if (!identOld_.isValidIdent())
  {
    logError("Identifier \"" + identOld + "\" is invalid.");
    return oms_status_error;
  }

  // check if identNew is in scope
  auto it = subModels.find(identNew_);
  if (it != subModels.end())
  {
    logError("A sub-model called \"" + identNew.toString() + "\" is already in scope.");
    return oms_status_error;
  }

  // check if identOld is in scope
  it = subModels.find(identOld_);
  if (it == subModels.end())
  {
    logError("There is no sub-model called \"" + identOld.toString() + "\" in scope.");
    return oms_status_error;
  }

  it->second->setName(identNew);
  subModels[identNew_] = it->second;
  subModels.erase(it);

  return oms_status_ok;
}

oms2::Element** oms2::FMICompositeModel::getElements()
{
  logTrace();

  if (components)
    return components;

  updateComponents();
  return components;
}

oms2::Variable* oms2::FMICompositeModel::getVariable(const oms2::SignalRef& signal)
{
  auto it = subModels.find(signal.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + signal.getCref() + "\" found.");
    return NULL;
  }

  return it->second->getVariable(signal.getVar());
}

oms_causality_enu_t oms2::FMICompositeModel::getSignalCausality(const oms2::SignalRef& signal)
{
  auto it = subModels.find(signal.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + signal.getCref() + "\" found.");
    return oms_causality_undefined;
  }

  if (oms_component_table_old == it->second->getType())
    return oms_causality_output;

  return it->second->getVariable(signal.getVar())->getCausality();
}

oms_status_enu_t oms2::FMICompositeModel::exportCompositeStructure(const std::string& filename)
{
  logTrace();

  /*
   * #dot -Gsplines=none test.dot | neato -n -Gsplines=ortho -Tpng -otest.png
   * digraph G
   * {
   *   graph [rankdir=LR, splines=ortho];
   *
   *   node[shape=record];
   *   A [label="A", height=2, width=2];
   *   B [label="B", height=2, width=2];
   *
   *   A -> B [label="A.y -> B.u"];
   * }
   */

  if (!(filename.length() > 4 && filename.substr(filename.length() - 4) == ".dot"))
  {
    logError("[oms2::FMICompositeModel::exportCompositeStructure] The filename must have .dot as extension.");
    return oms_status_error;
  }

  std::ofstream dotFile(filename);
  dotFile << "#dot -Gsplines=none " << filename.c_str() << " | neato -n -Gsplines=ortho -Tpng -o" << filename.substr(0, filename.length() - 4).c_str() << ".png" << std::endl;
  dotFile << "digraph G" << std::endl;
  dotFile << "{" << std::endl;
  dotFile << "  graph [rankdir=LR, splines=ortho];\n" << std::endl;
  dotFile << "  node[shape=record];" << std::endl;

  for (const auto& it : subModels)
  {
    dotFile << "  " << it.first.toString() << "[label=\"" << it.first.toString();
    switch (it.second->getType())
    {
    case oms_component_table_old:
      dotFile << "\\n(table)";
      break;
    case oms_component_fmu_old:
      dotFile << "\\n(fmu)";
      break;
    }
    dotFile << "\", height=2, width=2];" << std::endl;
  }

  dotFile << std::endl;
  for (auto& connection : connections)
  {
    if (connection)
    {
      SignalRef A = connection->getSignalA();
      SignalRef B = connection->getSignalB();

      oms_causality_enu_t varA = getSignalCausality(A);
      oms_causality_enu_t varB = getSignalCausality(B);

      if (oms_causality_output == varA && oms_causality_input == varB)
        dotFile << "  " << A.getCref().toString() << " -> " << B.getCref().toString() << " [taillabel=\"" << A.getVar() << "\", headlabel=\"" << B.getVar() /*<< "\", label=\"" << A.getVar() << " -> " << B.getVar()*/ << "\"];" << std::endl;
      else
        return logError("[oms2::FMICompositeModel::exportCompositeStructure] failed");
    }
  }

  dotFile << "}" << std::endl;
  dotFile.close();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::exportDependencyGraphs(const std::string& initialization, const std::string& simulation)
{
  logTrace();

  oms_status_enu_t status = updateDependencyGraphs();

  if (!initialization.empty())
    initialUnknownsGraph.dotExport(initialization);
  else
    status = logWarning("[exportDependencyGraphs] empty graph for initial unknowns");

  if (!simulation.empty())
    outputsGraph.dotExport(simulation);
  else
    status = logWarning("[exportDependencyGraphs] empty graph for initial unknowns");

  return status;
}

oms_status_enu_t oms2::FMICompositeModel::updateDependencyGraphs()
{
  initialUnknownsGraph.clear();
  outputsGraph.clear();

  for (const auto& it : subModels)
  {
    initialUnknownsGraph.includeGraph(it.second->getInitialUnknownsGraph());
    outputsGraph.includeGraph(it.second->getOutputsGraph());
  }

  for (auto& connection : connections)
  {
    if (!connection)
      continue;

    oms2::Variable *varA = getVariable(connection->getSignalA());
    oms2::Variable *varB = getVariable(connection->getSignalB());
    if (varA && varB)
    {
      if (varA->isOutput() && varB->isInput())
      {
        initialUnknownsGraph.addEdge(*varA, *varB);
        outputsGraph.addEdge(*varA, *varB);
      }
      else
        return logError("[oms2::FMICompositeModel::initialize] failed for " + connection->getSignalA().toString() + " -> " + connection->getSignalB().toString());
    }
    else
      return logError("[oms2::FMICompositeModel::initialize] failed");
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::initialize(double startTime, double tolerance)
{
  if (oms_status_error == updateDependencyGraphs())
    return oms_status_error;

#if !defined (NO_TLM)
  if(plugin)
    plugin->CheckModel();
#endif

  this->time = startTime;
  this->tolerance = tolerance;
  this->tLastEmit = startTime;

  // check if there is a solver instance assigned to each FMU
  for (const auto& it : subModels)
  {
    if (oms_component_fmu_old == it.second->getType())
    {
      if (!dynamic_cast<FMUWrapper*>(it.second)->getSolver())
      {
        std::string solverName = "solver_" + it.first;
        std::string method = "internal";
        if (oms_fmi_kind_cs != dynamic_cast<FMUWrapper*>(it.second)->getFMUInfo()->getKind())
          method = "euler";

        logWarning("No solver instance is assigned to FMU \"" + it.first + "\"; A default solver will be selected: " + method);
        addSolver(oms2::ComRef(solverName), method);
        connectSolver(it.first, oms2::ComRef(solverName));
      }
    }
  }

  for (const auto& it : solvers)
    it.second->setTime(startTime);

  // Enter initialization
  for (const auto& it : subModels)
  {
    if (oms_status_ok != it.second->enterInitialization(startTime))
      return logError("[oms2::FMICompositeModel::initialize] failed");
  }

#if !defined(NO_TLM)
    //Update initial values from TLM sockets
    readFromTLMSockets(time);
#endif
  updateInputs(initialUnknownsGraph);

  // Exit initialization
  for (const auto& it : subModels)
    if (oms_status_ok != it.second->exitInitialization())
      return logError("[oms2::FMICompositeModel::initialize] failed");

  // Initialize solvers
  for (const auto& it : solvers)
    it.second->initializeSolver(startTime);

  updateInputs(outputsGraph);

  clock.reset();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::reset(bool terminate)
{
  logTrace();

  for (const auto& it : solvers)
    it.second->freeSolver();

  for (const auto& it : subModels)
    it.second->reset(terminate);

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::stepUntil(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, MasterAlgorithm masterAlgorithm, bool realtime_sync)
{
  logTrace();
  clock.tic();
  oms_status_enu_t status = oms_status_error;

  switch (masterAlgorithm)
  {
  case MasterAlgorithm::STANDARD :
    logDebug("oms2::FMICompositeModel::stepUntil: Using master algorithm 'standard'");
    status = stepUntilStandard(resultWriter, stopTime, communicationInterval, loggingInterval, realtime_sync);
    break;
#if !defined(__arm__)
  case MasterAlgorithm::PCTPL :
    logDebug("oms2::FMICompositeModel::stepUntil: Using master algorithm 'pctpl'");
    status = stepUntilPCTPL(resultWriter, stopTime, communicationInterval, loggingInterval, realtime_sync);
    break;
  case MasterAlgorithm::PMRCHANNELA :
    logDebug("oms2::FMICompositeModel::stepUntil: Using master algorithm 'pmrchannela'");
    status = oms2::stepUntilPMRChannel<oms2::PMRChannelA>(resultWriter, stopTime, communicationInterval, loggingInterval, this->getName().toString(), outputsGraph, subModels, realtime_sync);
    break;
  case MasterAlgorithm::PMRCHANNELCV :
    logDebug("oms2::FMICompositeModel::stepUntil: Using master algorithm 'pmrchannelcv'");
    status = oms2::stepUntilPMRChannel<oms2::PMRChannelCV>(resultWriter, stopTime, communicationInterval, loggingInterval, this->getName().toString(), outputsGraph, subModels, realtime_sync);
    break;
  case MasterAlgorithm::PMRCHANNELM :
    logDebug("oms2::FMICompositeModel::stepUntil: Using master algorithm 'pmrchannelm'");
    status = oms2::stepUntilPMRChannel<oms2::PMRChannelM>(resultWriter, stopTime, communicationInterval, loggingInterval, this->getName().toString(), outputsGraph, subModels, realtime_sync);
    break;
#endif
  default:
    logError("oms2::FMICompositeModel::stepUntil: Internal error: Request for using unknown master algorithm.");
  }

  clock.toc();
  return status;
}

oms_status_enu_t oms2::FMICompositeModel::doSteps(oms3::ResultWriter& resultWriter, const int numberOfSteps, double communicationInterval, double loggingInterval)
{
  logTrace();
  clock.tic();

  for(int step=0; step<numberOfSteps; step++)
  {
    time += communicationInterval;

    // call doStep, except for FMUs
    for (const auto& it : subModels)
      if (oms_component_fmu_old != it.second->getType())
        it.second->doStep(time);

    // call doStep for FMUs
    for (const auto& it : solvers)
      it.second->doStep(time);

    if (loggingInterval >= 0.0 && time - tLastEmit >= loggingInterval)
    {
      if (loggingInterval <= 0.0)
        emit(resultWriter);
      updateInputs(outputsGraph);
      emit(resultWriter);
    }
    else
      updateInputs(outputsGraph);
  }

  clock.toc();
  return oms_status_ok;
}


oms_status_enu_t oms2::FMICompositeModel::stepUntilStandard(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, bool realtime_sync)
{
  logTrace();
  auto start = std::chrono::steady_clock::now();

  while (time < stopTime)
  {
    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(time+communicationInterval));
    time += communicationInterval;
    if (time > stopTime)
      time = stopTime;

    // call doStep, except for FMUs
    for (const auto& it : subModels)
      if (oms_component_fmu_old != it.second->getType())
        it.second->doStep(time);

    // call doStep for FMUs
    for (const auto& it : solvers)
      it.second->doStep(time);

    if (realtime_sync)
    {
      auto now = std::chrono::steady_clock::now();
      // seems a cast to a sufficient high resolution of time is necessary for avoiding truncation errors
      auto next = start + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(time));
      std::chrono::duration<double> margin = next - now;
      // std::cout << "[oms2::FMICompositeModel::stepUntilStandard] doStep: " << std::to_string(time  - communicationInterval) << "s -> " << std::to_string(time) << "s, real-time margin=" << std::to_string(margin.count()) << "s" << std::endl;
      if (margin < std::chrono::duration<double>(0))
        logError(std::string("[oms2::FMICompositeModel::stepUntilStandard] real-time frame overrun, time=") + std::to_string(time) + std::string("s, exceeded margin=") + std::to_string(margin.count()) + std::string("s\n"));

      std::this_thread::sleep_until(next);
    }

    // input := output
    if (loggingInterval >= 0.0 && time - tLastEmit >= loggingInterval)
    {
      if (loggingInterval <= 0.0)
        emit(resultWriter);
      updateInputs(outputsGraph);
      emit(resultWriter);
    }
    else
      updateInputs(outputsGraph);
  }

  return oms_status_ok;
}

#if !defined(__arm__)

/**
 * \brief Parallel "doStep(..)" execution using task pool CTPL library (https://github.com/vit-vit/CTPL).
 */
oms_status_enu_t oms2::FMICompositeModel::stepUntilPCTPL(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, bool realtime_sync)
{
  logTrace();

  std::vector<oms2::FMISubModel*> sub_model;
  for (auto it=subModels.begin(); it != subModels.end(); ++it)
    if (oms_component_fmu_old != it->second->getType())
      sub_model.push_back(it->second);

  std::vector<oms2::Solver*> _solver;
  for (auto& it : solvers)
    _solver.push_back(it.second);

  int numThreads = sub_model.size() + _solver.size();
  if (numThreads > std::thread::hardware_concurrency())
    numThreads = std::thread::hardware_concurrency();

  logInfo(std::string("oms2::FMICompositeModel::stepUntilPCTPL: Creating thread pool with ") + std::to_string(numThreads) + std::string(" threads in the pool"));
  ctpl::thread_pool p(numThreads);
  std::vector<std::future<void>> results(numThreads);

  auto start = std::chrono::steady_clock::now();
  while (time < stopTime)
  {
    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(time+communicationInterval));
    time += communicationInterval;
    if (time > stopTime)
      time = stopTime;

    // call doStep, except for FMUs
    for (int i=0; i < sub_model.size(); ++i) {
      auto time_tmp = time;
      results[i] = p.push([&sub_model, i, time_tmp](int){ sub_model[i]->doStep(time_tmp); });
    }
    // call doStep for FMUs
    for (int i=0; i < _solver.size(); ++i) {
      auto time_tmp = time;
      results[sub_model.size()+i] = p.push([&_solver, i, time_tmp](int){ _solver[i]->doStep(time_tmp); });
    }
    for (int i=0; i < sub_model.size() + _solver.size(); ++i)
      results[i].get();

    if (realtime_sync)
    {
      auto now = std::chrono::steady_clock::now();
      // seems a cast to a sufficient high resolution of time is necessary for avoiding truncation errors
      auto next = start + std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(time));
      std::chrono::duration<double> margin = next - now;
      // std::cout << "[oms2::FMICompositeModel::stepUntilStandard] doStep: " << std::to_string(time  - communicationInterval) << "s -> " << std::to_string(time) << "s, real-time margin=" << std::to_string(margin.count()) << "s" << std::endl;
      if (margin < std::chrono::duration<double>(0))
        logError(std::string("[oms2::FMICompositeModel::stepUntilPCTPL] real-time frame overrun, time=") + std::to_string(time) + std::string("s, exceeded margin=") + std::to_string(margin.count()) + std::string("s\n"));

      std::this_thread::sleep_until(next);
    }

    if (loggingInterval >= 0.0 && time - tLastEmit >= loggingInterval)
    {
      if (loggingInterval <= 0.0)
        emit(resultWriter);
      updateInputs(outputsGraph);
      emit(resultWriter);
    }
    else
      updateInputs(outputsGraph);
  }
  return oms_status_ok;
}

#endif // #if !defined(__arm__)

void oms2::FMICompositeModel::simulate_asynchronous(oms3::ResultWriter& resultWriter, double stopTime, double communicationInterval, double loggingInterval, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  logTrace();
  oms_status_enu_t statusSubModel;
  oms_status_enu_t status;

  while (time < stopTime)
  {
    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(time+communicationInterval));
    time += communicationInterval;
    if (time > stopTime)
      time = stopTime;

    // call doStep, except for FMUs
    status = oms_status_ok;
    for (const auto& it : subModels)
    {
      if (oms_component_fmu_old != it.second->getType())
        statusSubModel = it.second->doStep(time);
      status = statusSubModel > status ? statusSubModel : status;
    }

    // call doStep for FMUs
    for (const auto& it : solvers)
    {
      statusSubModel = it.second->doStep(time);
      status = statusSubModel > status ? statusSubModel : status;
    }

    if (loggingInterval >= 0.0 && time - tLastEmit >= loggingInterval)
    {
      if (loggingInterval <= 0.0)
        emit(resultWriter);
      updateInputs(outputsGraph);
      emit(resultWriter);
    }
    else
      updateInputs(outputsGraph);

    cb(this->getName().c_str(), time, status);
  }
}

#if !defined(NO_TLM)
oms_status_enu_t oms2::FMICompositeModel::initializeTLM(double startTime, double tolerance, std::string server)
{
  logTrace();

  Model *model = oms2::Scope::GetInstance().getModel(getName());
  model->setStartTime(startTime);
  model->setTolerance(tolerance);

  if(oms_status_ok != model->initialize()) {
    return logError("[oms2::FMICompositeModel::simulateTLM] model initialization failed");
  }

  if(oms_status_ok != updateInitialTLMValues()) {
    return logError("[oms2::FMICompositeModel::simulateTLM] socket initialization failed");
  }

  tlmInitialized = true;

  return oms_status_ok;
}


oms_status_enu_t oms2::FMICompositeModel::simulateTLM(double stopTime, double loggingInterval)
{
  logTrace();

  Model *model = oms2::Scope::GetInstance().getModel(getName());
  oms3::ResultWriter *resultWriter = model->getResultWriter();

  logInfo("Starting simulation loop.");

  while (time < stopTime)
  {
    logDebug("doStep: " + std::to_string(time) + " -> " + std::to_string(time+communicationInterval));
    clock.tic();
    time += communicationInterval;
    if (time > stopTime)
      time = stopTime;

    // call doStep, except for FMUs
    for (const auto& it : subModels)
      if (oms_component_fmu_old != it.second->getType())
        it.second->doStep(time);

    // call doStep for FMUs
    for (const auto& it : solvers)
      it.second->doStep(time);

    if (loggingInterval >= 0.0 && time - tLastEmit >= loggingInterval)
    {
      if (loggingInterval <= 0.0)
        emit(*resultWriter);
      updateInputs(outputsGraph);
      emit(*resultWriter);
    }
    else
      updateInputs(outputsGraph);
    clock.toc();
  }

  finalizeTLMSockets();

  logInfo("Simulation of model "+getName().toString()+" complete.");

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::setupTLMSockets(double startTime, std::string server)
{
  logInfo("Starting TLM simulation thread for model "+getName().toString());

  time = startTime;

  Model *model = oms2::Scope::GetInstance().getModel(getName());
  communicationInterval = model->getCommunicationInterval();

  //Limit communication interval to half TLM delay
  //This is for avoiding extrapolation when running asynchronously.
  for(TLMInterface* ifc: tlmInterfaces) {
      if(communicationInterval > ifc->getDelay()*0.5) {
        communicationInterval = ifc->getDelay()*0.5;
        logInfo("Limiting communicationInterval to "+std::to_string(communicationInterval));
      }
  }

  logInfo("Creating plugin instance.");

  plugin = TLMPlugin::CreateInstance();

  logInfo("Initializing plugin.");

  if(!plugin->Init(this->getName().toString(),
                   time,
                   1, //Unused argument anyway
                   communicationInterval,
                   server)) {
    logError("Error initializing the TLM plugin.");
    return oms_status_error;
  }

  logInfo("Registering interfaces.");

  for(TLMInterface *ifc : tlmInterfaces) {
    oms_status_enu_t status = ifc->doRegister(plugin);
    if(status == oms_status_error) {
      return oms_status_error;
    }
  }

  tlmConnected = true;

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::updateInitialTLMValues()
{
  //Apply initial values for signal and effort
  for(TLMInterface *ifc : tlmInterfaces) {
    if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_input) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      if(tlmInitialValues.find(ifc->getName()) != tlmInitialValues.end()) {
        value = tlmInitialValues.find(ifc->getName())->second[0];
      }
      else {
        this->getReal(ifc->getSubSignal(tlmrefs.y), value);
      }
      plugin->SetInitialValue(ifc->getId(), value);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_1d_t tlmrefs;
      double effort,flow;
      if(tlmInitialValues.find(ifc->getName()) != tlmInitialValues.end()) {
        effort = tlmInitialValues.find(ifc->getName())->second[0];
        flow = tlmInitialValues.find(ifc->getName())->second[1];
      }
      else {
        this->getReal(ifc->getSubSignal(tlmrefs.f), effort);
        this->getReal(ifc->getSubSignal(tlmrefs.v), flow);
      }
      plugin->SetInitialForce1D(ifc->getId(), effort);
      plugin->SetInitialFlow1D(ifc->getId(), flow);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() != oms_tlm_no_interpolation) {
      if(tlmInitialValues.find(ifc->getName()) != tlmInitialValues.end()) {
        double effort = tlmInitialValues.find(ifc->getName())->second[0];
        double flow = tlmInitialValues.find(ifc->getName())->second[1];
        plugin->SetInitialForce1D(ifc->getId(), effort);
        plugin->SetInitialFlow1D(ifc->getId(), flow);
      }
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      if(tlmInitialValues.find(ifc->getName()) != tlmInitialValues.end()) {
        effort[0] = tlmInitialValues.find(ifc->getName())->second[0];
        effort[1] = tlmInitialValues.find(ifc->getName())->second[1];
        effort[2] = tlmInitialValues.find(ifc->getName())->second[2];
        effort[3] = tlmInitialValues.find(ifc->getName())->second[3];
        effort[4] = tlmInitialValues.find(ifc->getName())->second[4];
        effort[5] = tlmInitialValues.find(ifc->getName())->second[5];
        flow[0] = tlmInitialValues.find(ifc->getName())->second[6];
        flow[1] = tlmInitialValues.find(ifc->getName())->second[7];
        flow[2] = tlmInitialValues.find(ifc->getName())->second[8];
        flow[3] = tlmInitialValues.find(ifc->getName())->second[9];
        flow[4] = tlmInitialValues.find(ifc->getName())->second[10];
        flow[5] = tlmInitialValues.find(ifc->getName())->second[11];
      }
      else {
        this->getReals(ifc->getSubSignalSet(tlmrefs.f), effort);
        std::vector<int> flowrefs = tlmrefs.v;
        flowrefs.insert(flowrefs.end(), tlmrefs.w.begin(), tlmrefs.w.end());
        this->getReals(ifc->getSubSignalSet(flowrefs), flow);
      }
      plugin->SetInitialForce3D(ifc->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
      plugin->SetInitialFlow3D(ifc->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() != oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_3d_t tlmrefs;
      std::vector<double> effort(6,0);
      std::vector<double> flow(6,0);
      if(tlmInitialValues.find(ifc->getName()) != tlmInitialValues.end()) {
        effort[0] = tlmInitialValues.find(ifc->getName())->second[0];
        effort[1] = tlmInitialValues.find(ifc->getName())->second[1];
        effort[2] = tlmInitialValues.find(ifc->getName())->second[2];
        effort[3] = tlmInitialValues.find(ifc->getName())->second[3];
        effort[4] = tlmInitialValues.find(ifc->getName())->second[4];
        effort[5] = tlmInitialValues.find(ifc->getName())->second[5];
        flow[0] = tlmInitialValues.find(ifc->getName())->second[6];
        flow[1] = tlmInitialValues.find(ifc->getName())->second[7];
        flow[2] = tlmInitialValues.find(ifc->getName())->second[8];
        flow[3] = tlmInitialValues.find(ifc->getName())->second[9];
        flow[4] = tlmInitialValues.find(ifc->getName())->second[10];
        flow[5] = tlmInitialValues.find(ifc->getName())->second[11];
        plugin->SetInitialForce3D(ifc->getId(), effort[0], effort[1], effort[2], effort[3], effort[4], effort[5]);
        plugin->SetInitialFlow3D(ifc->getId(), flow[0], flow[1], flow[2], flow[3], flow[4], flow[5]);
      }
    }
  }
  return oms_status_ok;
}

void oms2::FMICompositeModel::readFromTLMSockets(double time, std::string fmu)
{
  for(TLMInterface *ifc : tlmInterfaces) {
    if(!fmu.empty() &&
       fmu != ifc->getFMUName().toString()) {
      continue; //Ignore FMUs not specified in vector
    }

    if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_input) {
      oms_tlm_sigrefs_signal_t tlmrefs;

      double value;
      plugin->GetValueSignal(ifc->getId(), time, &value);
      this->setReal(ifc->getSubSignal(tlmrefs.y), value);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_no_interpolation) {
      oms_tlm_sigrefs_1d_t tlmrefs;

      double flow,effort;

      //Read position and speed from FMU
      this->getReal(ifc->getSubSignal(tlmrefs.v), flow);

      //Get interpolated force
      plugin->GetForce1D(ifc->getId(), time, flow, &effort);

      if(ifc->getDomain() != "Hydraulic") {
          effort = -effort;
      }

      //Write force to FMU
      this->setReal(ifc->getSubSignal(tlmrefs.f), effort);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_1d_cg_t tlmrefs;

      double impedance, wave;
      plugin->GetWaveImpedance1D(ifc->getId(), time, &impedance, &wave);
      this->setReal(ifc->getSubSignal(tlmrefs.c), wave);
      this->setReal(ifc->getSubSignal(tlmrefs.Z), impedance);

      double impedance2, wave2;
      plugin->GetWaveImpedance1D(ifc->getId(), time+communicationInterval, &impedance2, &wave2);

      double dWave = (wave2-wave)/communicationInterval;

      this->setRealInputDerivatives(ifc->getSubSignal(tlmrefs.c), 1, dWave);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_1d_fg_t tlmrefs;

      double wave;
      double impedance;

      double t = time;
      for(size_t i=0; i<10; ++i) {
        plugin->GetWaveImpedance1D(ifc->getId(), t, &impedance, &wave);
        t += communicationInterval/9;

        this->setReal(ifc->getSubSignal(tlmrefs.c[i]), wave);
        this->setReal(ifc->getSubSignal(tlmrefs.t[i]), t);
      }

      this->setReal(ifc->getSubSignal(tlmrefs.Z), impedance);
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_no_interpolation) {

      oms_tlm_sigrefs_3d_t tlmrefs;

      std::vector<double> x(3,0); //Dummy, GetForce3D needs it but does not use it
      std::vector<double> A(9,0); //Dummy
      std::vector<double> v(3,0);
      std::vector<double> w(3,0);
      std::vector<double> f(6,0);

      //Read position and speed from FMU
      this->getReals(ifc->getSubSignalSet(tlmrefs.v), v);
      this->getReals(ifc->getSubSignalSet(tlmrefs.w), w);

      //Get interpolated force
      plugin->GetForce3D(ifc->getId(), time,&x[0], &A[0], &v[0], &w[0], &f[0]);

      for(size_t i=0; i<6; ++i) {
        f[i] = -f[i];
      }

      //Write force to FMU
      this->setReals(ifc->getSubSignalSet(tlmrefs.f), f);
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_coarse_grained) {
      oms_tlm_sigrefs_3d_cg_t tlmrefs;
      std::vector<double> waves(6,0);
      double Zt, Zr;
      plugin->GetWaveImpedance3D(ifc->getId(), time, &Zt, &Zr, &waves[0]);
      this->setReals(ifc->getSubSignalSet(tlmrefs.c), waves);
      this->setReal(ifc->getSubSignal(tlmrefs.Zt), Zt);
      this->setReal(ifc->getSubSignal(tlmrefs.Zr), Zr);

      std::vector<double> waves2(6,0);
      double Zt2, Zr2;
      plugin->GetWaveImpedance3D(ifc->getId(), time+communicationInterval, &Zt2, &Zr2, &waves2[0]);

      std::vector<double> dWaves(6,0);
      for(size_t i=0; i<6; ++i) {
        double dWave = (waves2[i]-waves[i])/communicationInterval;
        this->setRealInputDerivatives(ifc->getSubSignal(tlmrefs.c[i]), 1, dWave);
      }
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir &&
            ifc->getInterpolationMethod() == oms_tlm_fine_grained) {
      oms_tlm_sigrefs_3d_fg_t tlmrefs;

      std::vector<double> waves(6,0);
      double Zt,Zr;

      double t = time;

      for(size_t i=0; i<10; ++i) {
        plugin->GetWaveImpedance3D(ifc->getId(), t, &Zt, &Zr, &waves[0]);
        t += communicationInterval/9;

        this->setReals(ifc->getSubSignalSet(tlmrefs.c[i]), waves);
        this->setReal(ifc->getSubSignal(tlmrefs.t[i]), t);
      }

      this->setReal(ifc->getSubSignal(tlmrefs.Zt), Zt);
      this->setReal(ifc->getSubSignal(tlmrefs.Zr), Zr);
    }
  }
}


void oms2::FMICompositeModel::writeToTLMSockets(double time, std::string fmu)
{
  for(TLMInterface *ifc : tlmInterfaces) {
    if(!fmu.empty() &&
       fmu != ifc->getFMUName().toString()) {
      continue; //Ignore FMUs not specified in vector
    }
    if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_output) {
      oms_tlm_sigrefs_signal_t tlmrefs;
      double value;
      this->getReal(ifc->getSubSignal(tlmrefs.y), value);
      plugin->SetValueSignal(ifc->getId(), time, value);
    }
    else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir) {
      oms_tlm_sigrefs_1d_t tlmrefs;
      double state, flow, force;
      this->getReal(ifc->getSubSignal(tlmrefs.x), state);
      this->getReal(ifc->getSubSignal(tlmrefs.v), flow);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      plugin->GetForce1D(ifc->getId(), time, flow, &force);

      //Send the resulting motion back to master
      plugin->SetMotion1D(ifc->getId(), time, state, flow);
    }
    else if(ifc->getDimensions() == 3 && ifc->getCausality() == oms_causality_bidir) {

      oms_tlm_sigrefs_3d_t tlmrefs;

      std::vector<double> x(3,0);
      std::vector<double> A(9,0);
      std::vector<double> v(3,0);
      std::vector<double> w(3,0);
      std::vector<double> f(6,0);

      this->getReals(ifc->getSubSignalSet(tlmrefs.x), x);
      this->getReals(ifc->getSubSignalSet(tlmrefs.A), A);
      this->getReals(ifc->getSubSignalSet(tlmrefs.v), v);
      this->getReals(ifc->getSubSignalSet(tlmrefs.w), w);

      //Important: OMTLMSimulator assumes that GetForce is called
      //before SetMotion, in order to calculate the wave variable
      plugin->GetForce3D(ifc->getId(), time, &x[0], &A[0], &v[0], &w[0], &f[0]);

      //Send the resulting motion back to master
      plugin->SetMotion3D(ifc->getId(), time, &x[0], &A[0], &v[0], &w[0]);
    }
  }
}

void oms2::FMICompositeModel::finalizeTLMSockets()
{
  //Wait for close permission, to prevent socket from being
  //destroyed before master has read all data
  plugin->AwaitClosePermission();

  delete plugin;
}
#endif //!defined(NO_TLM)

oms_status_enu_t oms2::FMICompositeModel::setInteger(const oms2::SignalRef& sr, int value)
{
  oms2::FMISubModel* model = getSubModel(sr.getCref());
  if (!model)
    return oms_status_error;

  return model->setInteger(sr, value);
}

oms_status_enu_t oms2::FMICompositeModel::getInteger(const oms2::SignalRef& sr, int& value)
{
  oms2::FMISubModel* model = getSubModel(sr.getCref());
  if (!model)
    return oms_status_error;

  oms_status_enu_t status = model->getInteger(sr, value);
  return status;
}


oms_status_enu_t oms2::FMICompositeModel::setReal(const oms2::SignalRef& sr, double value)
{
  oms2::FMISubModel* model = getSubModel(sr.getCref());
  if (!model)
    return oms_status_error;

  return model->setReal(sr, value);
}

oms_status_enu_t oms2::FMICompositeModel::setReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> values)
{
  oms_status_enu_t status = oms_status_ok;
  for(size_t i=0; i<sr.size(); ++i) {
    oms_status_enu_t tempStatus = setReal(sr[i], values[i]);
    if(tempStatus != oms_status_ok) {
      status = tempStatus;
    }
  }

  return status;
}

oms_status_enu_t oms2::FMICompositeModel::getReal(const oms2::SignalRef& sr, double& value)
{
  oms2::FMISubModel* model = getSubModel(sr.getCref());
  if (!model)
    return oms_status_error;

  oms_status_enu_t status = model->getReal(sr, value);
  return status;
}

oms_status_enu_t oms2::FMICompositeModel::getReals(const std::vector<oms2::SignalRef> &sr, std::vector<double> &values)
{
  oms_status_enu_t status;
  for(size_t i=0; i<sr.size(); ++i) {
    oms_status_enu_t status = getReal(sr[i], values[i]);
    if(status != oms_status_ok) {
      values.clear();
      return status;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::setRealInputDerivatives(const oms2::SignalRef &sr, int order, double value)
{
  oms2::FMISubModel *model = getSubModel(sr.getCref());
  if(!model)
    return oms_status_error;

  return model->setRealInputDerivatives(sr, order, value);
}

#if !defined(NO_TLM)
oms_status_enu_t oms2::FMICompositeModel::addTLMInterface(oms2::TLMInterface *ifc)
{
  tlmInterfaces.push_back(ifc);
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::setTLMInitialValues(std::string ifcname, std::vector<double> values)
{
  //Find interface log
  bool found = false;
  for(TLMInterface* ifc: tlmInterfaces) {
    if(ifc->getName() == ifcname) {
      found = true;
      if(ifc->getDimensions() == 1 && ifc->getCausality() != oms_causality_bidir) {
        if(values.size() < 1) {
          logError("No initial TLM value specified.");
          return oms_status_error;
        }
        tlmInitialValues.insert(std::make_pair(ifcname, values));
      }
      else if(ifc->getDimensions() == 1 && ifc->getCausality() == oms_causality_bidir) {
        if(values.size() < 2) {
          logError("Too few initial TLM values specified for 1D interface (should be 2, effort and flow).");
          return oms_status_error;
        }
        tlmInitialValues.insert(std::make_pair(ifcname, values));
      }
      else if(ifc->getDimensions() == 3) {
        if(values.size() < 12) {
          logError("Too few initial TLM values specified for 3D interface (should be 12, 3 forces, 3 torques, 3 velocities and 3 angular velocities).");
          return oms_status_error;
        }
        tlmInitialValues.insert(std::make_pair(ifcname, values));
      }
      break;
    }
  }

  if(!found) {
    logError("In FMICompositeModel::setTLMInitialValues(): TLM interface \""+ifcname+"\" not found.");
    return oms_status_error;
  }

  return oms_status_ok;
}
#endif

oms_status_enu_t oms2::FMICompositeModel::updateInputs(oms2::DirectedGraph& graph)
{
  // input := output
  const std::vector< std::vector< std::pair<int, int> > >& sortedConnections = graph.getSortedConnections();
  for(int i=0; i<sortedConnections.size(); i++)
  {
    if (sortedConnections[i].size() == 1)
    {
      int output = sortedConnections[i][0].first;
      int input = sortedConnections[i][0].second;

      if (graph.nodes[output].isTypeReal()) {
         double value = 0.0;
         getReal(graph.nodes[output].getSignalRef(), value);
         setReal(graph.nodes[input].getSignalRef(), value);
        //std::cout << "[time " << time << "] " << graph.nodes[output].getSignalRef().toString() << " -> " << graph.nodes[input].getSignalRef().toString() << " (value: " << value << ")" << std::endl;
      } else //if (graph.nodes[output].isTypeInteger())
      {
        int value=0;
        getInteger(graph.nodes[output].getSignalRef(), value);
        setInteger(graph.nodes[input].getSignalRef(), value);
      }

    }
    else
    {
      solveAlgLoop(graph, sortedConnections[i]);
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::solveAlgLoop(oms2::DirectedGraph& graph, const std::vector< std::pair<int, int> >& SCC)
{
  const int size = SCC.size();
  const int maxIterations = 10;
  double maxRes;
  double *res = new double[size]();
  double tcur = 0.0;

  int it=0;
  do
  {
    it++;
    // get old values
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      getReal(graph.nodes[output].getSignalRef(), res[i]);
    }

    // update inputs
    for (int i=0; i<size; ++i)
    {
      int input = SCC[i].second;
      setReal(graph.nodes[input].getSignalRef(), res[i]);
    }

    // calculate residuals
    maxRes = 0.0;
    double value;
    for (int i=0; i<size; ++i)
    {
      int output = SCC[i].first;
      getReal(graph.nodes[output].getSignalRef(), value);
      res[i] -= value;

      if (fabs(res[i]) > maxRes)
        maxRes = fabs(res[i]);
    }
  } while(maxRes > tolerance && it < maxIterations);

  delete[] res;

  if (it >= maxIterations)
    return logError("CompositeModel::solveAlgLoop: max. number of iterations (" + std::to_string(maxIterations) + ") exceeded at time = " + std::to_string(tcur));
  logDebug("CompositeModel::solveAlgLoop: maxRes: " + std::to_string(maxRes) + ", iterations: " + std::to_string(it) + " at time = " + std::to_string(tcur));
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::registerSignalsForResultFile(oms3::ResultWriter& resultWriter)
{
  clock_id = resultWriter.addSignal("wallTime", "wall-clock time [s]", oms3::SignalType_REAL);

  for (const auto& it : subModels)
    it.second->registerSignalsForResultFile(resultWriter);

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::emit(oms3::ResultWriter& resultWriter)
{
  oms3::SignalValue_t wallTime;
  wallTime.realValue = clock.getElapsedWallTime();
  resultWriter.updateSignal(clock_id, wallTime);

  for (const auto& it : subModels)
    if (oms_status_ok != it.second->emit(resultWriter))
      return logError("Failed to log simulation results");

  resultWriter.emit(time);
  tLastEmit = time;

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::addSignalsToResults(const std::string& regex)
{
  for (const auto& it : subModels)
    it.second->addSignalsToResults(regex);
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::removeSignalsFromResults(const std::string& regex)
{
  for (const auto& it : subModels)
    it.second->removeSignalsFromResults(regex);
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::describe()
{
  std::cout << "# FMI composite model \"" << getName().toString() << "\"" << std::endl;

  oms2::Element** elements = getElements();
  while(*elements)
  {
    oms2::Element* element = *elements;
    std::cout << "## ";
    element->describe();
    elements++;
  }

  oms2::Connection** connections = getConnections();
  std::cout << "## Connections" << std::endl;
  while(*connections)
  {
    oms2::Connection* connection = *connections;
    connection->describe();
    connections++;
  }

  return oms_status_ok;
}

void oms2::FMICompositeModel::setName(const oms2::ComRef& name)
{
  for (auto& connection : connections)
    if (connection)
      connection->setParent(name);
}

bool oms2::FMICompositeModel::validAndUnusedCref(const oms2::ComRef& cref, bool showWaring)
{
  if (!cref.isValidIdent())
  {
    if (showWaring)
      logWarning("Invalid identifier: \"" + cref + "\"");
    return oms_status_error;
  }

  // Check if there is a submodel called <cref>
  if (NULL != getSubModel(cref, false))
  {
    if (showWaring)
      logWarning("The composite model \"" + getName() + "\" contains already a submodel called \"" + cref + "\"");
    return false;
  }

  // Check if there is a solver called <cref>
  if (NULL != getSolver(cref, false))
  {
    if (showWaring)
      logWarning("The composite model \"" + getName() + "\" contains already a solver called \"" + cref + "\"");
    return false;
  }

  return true;
}

oms_status_enu_t oms2::FMICompositeModel::addSolver(const oms2::ComRef& cref, const std::string& methodString)
{
  if (!validAndUnusedCref(cref, true))
    return logError("[oms2::FMICompositeModel::addSolver] invalid solver identifier");

  oms_solver_enu_t method;
  if ("internal" == methodString)
    method = oms_solver_internal;
  else if("euler" == methodString)
    method = oms_solver_explicit_euler;
  else if("cvode" == methodString)
    method = oms_solver_cvode;
  else
    return logError("[oms2::FMICompositeModel::addSolver] Unknown solver: \"" + methodString + "\"");

  Solver* solver = new Solver(cref, method);
  solvers[cref] = solver;

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::setSolverTolerance(const oms2::ComRef& cref, double tolerance)
{
  Solver* solver = getSolver(cref, true);
  if (solver)
    return solver->setTolerance(tolerance);

  return oms_status_error;
}

oms_status_enu_t oms2::FMICompositeModel::connectSolver(const oms2::ComRef& fmuCref, const oms2::ComRef& solverCref)
{
  oms_status_enu_t status = oms_status_error;

  oms2::FMISubModel* fmu = oms2::FMICompositeModel::getSubModel(fmuCref);
  if (!fmu || oms_component_fmu_old != fmu->getType())
    return logError("Unknown fmu: " + fmuCref);

  for (auto& solver : solvers)
  {
    if (solver.second->getName() == solverCref)
      status = solver.second->addFMU(dynamic_cast<FMUWrapper*>(fmu));
    else
      solver.second->removeFMU(fmuCref); // Just make sure each FMU is only connected to a single solver.
  }

  if (oms_status_error == status)
    logError("connecting solver \"" + solverCref + "\" to FMU \"" + fmuCref + "\" failed");
  return status;
}

oms_status_enu_t oms2::FMICompositeModel::unconnectSolver(const oms2::ComRef& fmuCref, const oms2::ComRef& solverCref)
{
  oms_status_enu_t status = oms_status_error;

  oms2::FMISubModel* fmu = oms2::FMICompositeModel::getSubModel(fmuCref);
  if (!fmu || oms_component_fmu_old != fmu->getType())
    return logError("Unknown fmu: " + fmuCref);

  for (auto& solver : solvers)
  {
    if (solver.second->getName() == solverCref)
      status = solver.second->removeFMU(fmuCref); // Just make sure each FMU is only connected to a single solver.
  }

  return status;
}

