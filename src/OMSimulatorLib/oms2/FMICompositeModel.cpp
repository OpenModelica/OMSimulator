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

#include "../Types.h"
#include "ComRef.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "SignalRef.h"
#include "ssd/Tags.h"
#include "Table.h"

#include <pugixml.hpp>

#include <sstream>

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
          oms2::ssd::ConnectionGeometry geometry;
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
  if (!cref.isValidIdent())
    return oms_status_error;

  // check if cref is already used
  auto it = subModels.find(cref);
  if (it != subModels.end())
  {
    logError("A submodel called \"" + cref + "\" is already instantiated.");
    return oms_status_error;
  }

  oms2::ComRef parent = getName();
  oms2::FMUWrapper* subModel = oms2::FMUWrapper::newSubModel(parent + cref, filename);
  if (!subModel)
    return oms_status_error;

  deleteComponents();

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::addTable(const std::string& filename, const oms2::ComRef& cref)
{
  if (!cref.isValidIdent())
    return oms_status_error;

  // check if cref is already used
  auto it = subModels.find(cref);
  if (it != subModels.end())
  {
    logError("A submodel called \"" + cref + "\" is already instantiated.");
    return oms_status_error;
  }

  oms2::ComRef parent = getName();
  oms2::Table* subModel = oms2::Table::newSubModel(parent + cref, filename);
  if (!subModel)
    return oms_status_error;

  deleteComponents();

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::deleteSubModel(const oms2::ComRef& cref)
{
  auto it = subModels.find(cref);
  if (it == subModels.end())
  {
    logError("No sub-model called \"" + cref + "\" instantiated.");
    return oms_status_error;
  }
  oms2::FMISubModel::deleteSubModel(it->second);
  subModels.erase(it);

  // delete associated connections
  for (int i=0; i<connections.size()-1; ++i)
  {
    if (!connections[i])
    {
      logError("[oms2::FMICompositeModel::deleteSubModel] null pointer");
      return oms_status_error;
    }
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

oms_status_enu_t oms2::FMICompositeModel::setRealParameter(const oms2::SignalRef& sr, double value)
{
  auto it = subModels.find(sr.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + sr.getCref() + "\" found.");
    return oms_status_error;
  }

  if (oms_component_fmu != it->second->getType())
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

  if (oms_component_fmu != it->second->getType())
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

  if (oms_component_fmu != it->second->getType())
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
  /// \todo check the connection
  oms2::ComRef parent = getName();
  connections.back() = new oms2::Connection(parent, conA, conB);
  connections.push_back(NULL);
  return oms_status_ok;
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

oms2::FMISubModel* oms2::FMICompositeModel::getSubModel(const oms2::ComRef& cref)
{
  auto it = subModels.find(cref.last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + cref + "\" found.");
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

  if (oms_component_table == it->second->getType())
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

  if (!(filename.length() > 5 && filename.substr(filename.length() - 4) == ".dot"))
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
    dotFile << "  " << it.first.toString() << "[label=\"" << it.first.toString() << "\", height=2, width=2];" << std::endl;

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
        dotFile << "  " << A.getCref().toString() << " -> " << B.getCref().toString() << " [label=\"" << A.getVar() << " -> " << B.getVar() << "\"];" << std::endl;
      else if (oms_causality_input == varA && oms_causality_output == varB)
        dotFile << "  " << B.getCref().toString() << " -> " << A.getCref().toString() << " [label=\"" << B.getVar() << " -> " << A.getVar() << "\"];" << std::endl;
      else
        return oms_status_error;
    }
  }

  dotFile << "}" << std::endl;
  dotFile.close();

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::initialize()
{
  logError("[oms2::FMICompositeModel::initialize] dependency analysis is missing");
  return oms_status_error;
}
