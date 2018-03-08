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

#include "ComRef.h"
#include "FMICompositeModel.h"
#include "Logging.h"
#include "Scope.h"
#include "TLMCompositeModel.h"
#include "FMUWrapper.h"
#include "../Types.h"
#include "ssd/ElementGeometry.h"
#include "ssd/ConnectionGeometry.h"

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

oms2::Scope::Scope()
{
  boost::filesystem::path tempPath = boost::filesystem::temp_directory_path();
  tempDir = tempPath.string();
}

oms2::Scope::~Scope()
{
  // free memory if no one else does
  for (auto it=models.begin(); it != models.end(); it++)
    unloadModel(it->first);
}

oms2::Scope& oms2::Scope::getInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms_status_enu_t oms2::Scope::newFMIModel(const oms2::ComRef& name)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  auto it = scope.models.find(name);
  if (it != scope.models.end())
  {
    logError("A model called \"" + name + "\" is already in the scope.");
    return oms_status_error;
  }

  Model* model = oms2::FMICompositeModel::newModel(name);
  if (!model)
    return oms_status_error;

  scope.models[name] = model;
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::newTLMModel(const oms2::ComRef& name)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  auto it = scope.models.find(name);
  if (it != scope.models.end())
  {
    logError("A model called \"" + name + "\" is already in the scope.");
    return oms_status_error;
  }

  Model* model = oms2::TLMCompositeModel::newModel(name);
  if (!model)
    return oms_status_error;

  scope.models[name] = model;
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::unloadModel(const oms2::ComRef& name)
{
  logTrace();

  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  auto it = scope.models.find(name);
  if (it == scope.models.end())
  {
    logError("[oms2::Scope::unloadModel] There is no model called \"" + name + "\" in the scope.");
    return oms_status_error;
  }

  oms2::Model::deleteModel(it->second);
  scope.models.erase(it);
  logInfo("Removed model from scope: " + name);

  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::addFMU(const oms2::ComRef& modelIdent, const std::string& fmuPath, const oms2::ComRef& fmuIdent)
{
  Scope& scope = oms2::Scope::getInstance();

  oms2::Model* model = scope.getModel(modelIdent);
  if (!model)
    return oms_status_error;

  if (oms_component_fmi != model->getType())
  {
    logError("[oms2::Scope::addFMU] \"" + modelIdent + "\" is not a FMI composite model.");
  }

  FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
  return fmiModel->addFMU(fmuPath, fmuIdent);
}

oms_status_enu_t oms2::Scope::deleteSubModel(const oms2::ComRef& modelIdent, const oms2::ComRef& subModelIdent)
{
  Scope& scope = oms2::Scope::getInstance();

  oms2::Model* model = scope.getModel(modelIdent);
  if (!model)
    return oms_status_error;

  if (oms_component_fmi != model->getType())
  {
    logError("[oms2::Scope::deleteSubModel] \"" + modelIdent + "\" is not a FMI composite model.");
  }

  FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
  return fmiModel->deleteSubModel(subModelIdent);
}

oms_status_enu_t oms2::Scope::rename(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  Scope& scope = oms2::Scope::getInstance();

  // renaming a (root) model or a sub.model?
  if (identOld.first() == identNew.first())
  {
    auto it = scope.models.find(identOld.first());
    if (it == scope.models.end())
    {
      logError("[oms2::Scope::rename] no model called \"" + identOld.first() + "\" in scope.");
      return oms_status_error;
    }

    Model* model = it->second;

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      return fmiModel->renameSubModel(identOld, identNew);
    }
    else
    {
      logError("[oms2::Scope::rename] is not implemented for TLM sub-models yet");
      return oms_status_error;
    }
  }
  else // (root) model
  {
    return scope.renameModel(identOld, identNew);
  }

  logError("[oms2::Scope::rename] failed");
  return oms_status_error;
}

oms2::Model* oms2::Scope::loadModel(const std::string& filename)
{
  logTrace();

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
  {
    logError("[oms2::Scope::loadModel] failed for \"" + std::string(filename) + "\"");
    return NULL;
  }

  pugi::xml_node root = doc.document_element();
  oms_element_type_enu_t modelType = oms_component_none;
  bool defaultExperiment = true;

  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if (name == "TLMModel" && modelType == oms_component_none)
      modelType = oms_component_tlm;
    else if (name == "FMICompositeModel" && modelType == oms_component_none)
      modelType = oms_component_fmi;
    else if (name == "Experiment" && defaultExperiment)
      defaultExperiment = false;
    else
    {
      logError("[oms2::Scope::loadModel] wrong xml schema detected: \"" + std::string(filename) + "\"");
      return NULL;
    }
  }

  oms2::Model* model = NULL;
  if (modelType == oms_component_fmi)
    model = loadFMIModel(root.child("FMICompositeModel"));
  else if (modelType == oms_component_tlm)
    model = loadTLMModel(root.child("TLMModel"));

  if (!model)
  {
    logError("[oms2::Scope::loadModel] failed");
    return NULL;
  }

  if (!defaultExperiment)
  {
    const pugi::xml_node& experiment = root.child("Experiment");
    for (auto it = experiment.attributes_begin(); it != experiment.attributes_end(); ++it)
    {
      std::string name = it->name();
      if (name == "StartTime")
        model->setStartTime(it->as_double());
      else if (name == "StopTime")
        model->setStopTime(it->as_double());
      else if (name == "ResultFile")
        model->setResultFile(std::string(it->value()));
      else
      {
        logError("[oms2::Scope::loadModel] failed");
        unloadModel(model->getName());
        return NULL;
      }
    }
  }

  return model;
}

oms2::Model* oms2::Scope::loadFMIModel(const pugi::xml_node& xml)
{
  logTrace();
  oms2::Scope& scope = oms2::Scope::getInstance();

  // read model name
  std::string ident_;
  for (auto it = xml.attributes_begin(); it != xml.attributes_end(); ++it)
  {
    std::string name = it->name();
    if (name == "Name")
      ident_ = it->value();
  }

  // create empty model
  ComRef cref_model(ident_);
  oms_status_enu_t status = newFMIModel(cref_model);
  if (status != oms_status_ok)
  {
    logError("[oms2::Scope::loadFMIModel] newFMIModel failed");
    return NULL;
  }

  // import components
  FMICompositeModel* model = dynamic_cast<FMICompositeModel*>(scope.getModel(cref_model));

  for (auto it = xml.begin(); it != xml.end(); ++it)
  {
    std::string node = it->name();

    // instantiate FMUs / tables
    if (node == "SubModel")
    {
      std::string instancename;
      std::string filename;
      std::string type;
      for (auto ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
      {
        std::string name = ait->name();
        if (name == "Name")
        {
          instancename = ait->value();
        }
        else if (name == "ModelFile")
        {
          filename = ait->value();
        }
        else if (name == "Type")
        {
          type = ait->value();
        }
      }

      oms2::ComRef cref_submodel = cref_model + ComRef(instancename);

      oms_status_enu_t status = oms_status_error;
      if (type == "FMU")
        status = model->addFMU(filename, cref_submodel.last());
      else
        status = model->addTable(filename, cref_submodel.last());

      if (oms_status_ok != status)
      {
        unloadModel(cref_model);
        return NULL;
      }

      oms2::FMISubModel* subModel = model->getSubModel(cref_submodel);
      if (!subModel)
      {
        unloadModel(cref_model);
        return NULL;
      }

      for (auto child = it->first_child(); child; child = child.next_sibling())
      {
        // import ssd:ElementGeometry
        if (std::string(child.name()) == "ssd:ElementGeometry")
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
            if (oms_status_ok != model->setRealParameter(oms2::SignalRef(cref_submodel, _name), realValue))
            {
              logError("[oms2::Scope::loadFMIModel] wrong xml schema detected (2)");
              unloadModel(cref_model);
              return NULL;
            }
          }
          else
          {
            logError("[oms2::Scope::loadFMIModel] unsupported parameter type " + _type);
            unloadModel(cref_model);
            return NULL;
          }
        }
      }
    }// if (node == "SubModel")
    else if (node == "Connections")
    {
      for (auto connectionNode = it->first_child(); connectionNode; connectionNode = connectionNode.next_sibling())
      {
        if (std::string(connectionNode.name()) != "Connection")
        {
          logError("[oms2::Scope::loadFMIModel] wrong xml schema detected (3)");
          unloadModel(cref_model);
          return NULL;
        }

        std::string sigA = connectionNode.attribute("From").as_string();
        std::string sigB = connectionNode.attribute("To").as_string();
        if (oms_status_ok != model->addConnection(SignalRef(sigA), SignalRef(sigB)))
        {
          logError("[oms2::Scope::loadFMIModel] wrong xml schema detected (4)");
          unloadModel(cref_model);
          return NULL;
        }
        oms2::Connection* connection = model->getConnection(oms2::SignalRef(sigA), oms2::SignalRef(sigB));
        if (connection)
        {
          for (pugi::xml_node child: connectionNode.children())
          {
            // import ssd:ConnectionGeometry
            if (std::string(child.name()) == "ssd:ConnectionGeometry")
            {
              oms2::ssd::ConnectionGeometry geometry;
              std::string pointsXStr = child.attribute("pointsX").as_string();
              std::istringstream pointsXStream(pointsXStr);
              std::vector<std::string> pointsXVector(std::istream_iterator<std::string>{pointsXStream}, std::istream_iterator<std::string>());
              double* pointsX = new double[pointsXVector.size()];
              int i = 0;
              for ( auto &px : pointsXVector ) {
                pointsX[i++] = std::atof(px.c_str());
              }

              std::string pointsYStr = child.attribute("pointsY").as_string();
              std::istringstream pointsYStream(pointsYStr);
              std::vector<std::string> pointsYVector(std::istream_iterator<std::string>{pointsYStream}, std::istream_iterator<std::string>());
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
    }// if (node == "Connections")
    else if (node == "Solver")
    {
      /// \todo implement xml import for solver settings
      logWarning("[oms2::Scope::loadFMIModel] Solver");
    }// if (node == "Solver")
    else if (node == "ssd:ElementGeometry")
    {
      // import ssd:ElementGeometry
      oms2::ssd::ElementGeometry geometry;
      double x1 = 0.0;
      double y1 = 0.0;
      double x2 = 0.0;
      double y2 = 0.0;
      for (auto ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
      {
        std::string name = ait->name();
        if (name == "x1") x1 = ait->as_double();
        if (name == "y1") y1 = ait->as_double();
        if (name == "x2") x2 = ait->as_double();
        if (name == "y2") y2 = ait->as_double();
        if (name == "rotation") geometry.setRotation(ait->as_double());
        if (name == "iconSource") geometry.setIconSource(ait->as_string());
        if (name == "iconRotation") geometry.setIconRotation(ait->as_double());
        if (name == "iconFlip") geometry.setIconFlip(ait->as_bool());
        if (name == "iconFixedAspectRatio") geometry.setIconFixedAspectRatio(ait->as_bool());
      }
      geometry.setSizePosition(x1, y1, x2, y2);
      model->setGeometry(geometry);
    }// ssd:ElementGeometry
    else
    {
      logError("[oms2::Scope::loadFMIModel] wrong xml schema detected (5)");
      unloadModel(cref_model);
      return NULL;
    }
  }

  logInfo("New FMI model in scope: " + cref_model);
  return model;
}

oms2::Model* oms2::Scope::loadTLMModel(const pugi::xml_node& xml)
{
  logError("oms2::Scope::loadTLMModel: not implemented yet");
  return NULL;
}

oms_status_enu_t oms2::Scope::SetTempDirectory(const std::string& newTempDir)
{
  Scope& scope = oms2::Scope::getInstance();

  if (!boost::filesystem::is_directory(newTempDir))
  {
    if (!boost::filesystem::create_directory(newTempDir))
    {
      logError("Changing working directory to \"" + std::string(newTempDir) + "\" failed");
      return oms_status_error;
    }
    else
      logInfo("New temp directory has been created: \"" + std::string(newTempDir) + "\"");
  }

  boost::filesystem::path path(newTempDir.c_str());
  path = boost::filesystem::canonical(path);
  scope.tempDir = path.string();

  logInfo("New temp directory: \"" + std::string(scope.tempDir) + "\"");
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::SetWorkingDirectory(const std::string& path)
{
  logTrace();
  Scope& scope = oms2::Scope::getInstance();

  boost::filesystem::path path_(path.c_str());
  if (!boost::filesystem::is_directory(path_))
  {
    logError("Set working directory to \"" + path + "\" failed");
    return oms_status_error;
  }

  boost::filesystem::current_path(path_);
  scope.workingDir = path;
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::saveModel(const std::string& filename, const oms2::ComRef& name)
{
  logTrace();
  oms2::Scope& scope = oms2::Scope::getInstance();

  oms2::Model* model = scope.getModel(name);
  if (!model)
    return oms_status_error;

  switch (model->getType())
  {
  case oms_component_fmi:
    return scope.saveFMIModel(dynamic_cast<oms2::FMICompositeModel*>(model), filename);

  case oms_component_tlm:
    return scope.saveTLMModel(dynamic_cast<oms2::TLMCompositeModel*>(model), filename);
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::saveFMIModel(oms2::FMICompositeModel* model, const std::string& filename)
{
  logTrace();
  pugi::xml_document doc;
  std::string value;

  // generate XML declaration
  pugi::xml_node declarationNode = doc.append_child(pugi::node_declaration);
  declarationNode.append_attribute("version") = "1.0";
  declarationNode.append_attribute("encoding") = "UTF-8";
  pugi::xml_node compositeModel = doc.append_child("ssd:SystemStructureDescription");
  pugi::xml_node fmiCompositeModel = compositeModel.append_child("FMICompositeModel");
  pugi::xml_node experiment = compositeModel.append_child("Experiment");

  // ssd:SystemStructureDescription
  compositeModel.append_attribute("xmlns:ssc") = "http://www.pmsf.net/xsd/SystemStructureCommonDraft";
  compositeModel.append_attribute("xmlns:ssd") = "http://www.pmsf.net/xsd/SystemStructureDescriptionDraft";
  compositeModel.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
  compositeModel.append_attribute("xsi:schemaLocation") = "http://www.pmsf.net/xsd/SystemStructureDescriptionDraft http://www.pmsf.net/xsd/SSP/Draft20170606/SystemStructureDescription.xsd";
  value = model->getName().toString();

  fmiCompositeModel.append_attribute("Name") = value.c_str();
  // export ssd:ElementGeometry
  const oms2::ssd::ElementGeometry* elementGeometry = model->getGeometry();
  if (elementGeometry->getY1() != elementGeometry->getY2())
  {
    pugi::xml_node node = fmiCompositeModel.append_child("ssd:ElementGeometry");
    value = std::to_string(elementGeometry->getX1());
    node.append_attribute("x1") = value.c_str();
    value = std::to_string(elementGeometry->getY1());
    node.append_attribute("y1") = value.c_str();
    value = std::to_string(elementGeometry->getX2());
    node.append_attribute("x2") = value.c_str();
    value = std::to_string(elementGeometry->getY2());
    node.append_attribute("y2") = value.c_str();

    value = std::to_string(elementGeometry->getRotation());
    node.append_attribute("rotation") = value.c_str();

    if (elementGeometry->hasIconSource())
      node.append_attribute("iconSource") = elementGeometry->getIconSource().c_str();

    value = std::to_string(elementGeometry->getIconRotation());
    node.append_attribute("iconRotation") = value.c_str();

    if (elementGeometry->getIconFlip())
      node.append_attribute("iconFlip") = "true";
    else
      node.append_attribute("iconFlip") = "false";

    if (elementGeometry->getIconFixedAspectRatio())
      node.append_attribute("iconFixedAspectRatio") = "true";
    else
      node.append_attribute("iconFixedAspectRatio") = "false";
  }

  oms2::Element** elements = model->getElements();
  for (int i=0; elements[i]; ++i)
  {
    const oms2::Element* element = elements[i];

    oms2::ComRef cref = element->getName();
    pugi::xml_node subModel = fmiCompositeModel.append_child("SubModel");
    value = cref.last().toString();
    subModel.append_attribute("Name") = value.c_str();

    // export ssd:ElementGeometry
    const oms2::ssd::ElementGeometry* elementGeometry = element->getGeometry();
    if (elementGeometry->getY1() != elementGeometry->getY2())
    {
      pugi::xml_node node = subModel.append_child("ssd:ElementGeometry");
      value = std::to_string(elementGeometry->getX1());
      node.append_attribute("x1") = value.c_str();
      value = std::to_string(elementGeometry->getY1());
      node.append_attribute("y1") = value.c_str();
      value = std::to_string(elementGeometry->getX2());
      node.append_attribute("x2") = value.c_str();
      value = std::to_string(elementGeometry->getY2());
      node.append_attribute("y2") = value.c_str();

      value = std::to_string(elementGeometry->getRotation());
      node.append_attribute("rotation") = value.c_str();

      if (elementGeometry->hasIconSource())
        node.append_attribute("iconSource") = elementGeometry->getIconSource().c_str();

      value = std::to_string(elementGeometry->getIconRotation());
      node.append_attribute("iconRotation") = value.c_str();

      if (elementGeometry->getIconFlip())
        node.append_attribute("iconFlip") = "true";
      else
        node.append_attribute("iconFlip") = "false";

      if (elementGeometry->getIconFixedAspectRatio())
        node.append_attribute("iconFixedAspectRatio") = "true";
      else
        node.append_attribute("iconFixedAspectRatio") = "false";
    }
    if (oms_component_fmu == element->getType())
    {
      subModel.append_attribute("Type") = "FMU";

      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(model->getSubModel(cref));
      const std::string& fmuPath = fmuWrapper->getFMUPath();
      subModel.append_attribute("ModelFile") = fmuPath.c_str();

      const std::map<std::string, oms2::Option<double>>& realParameters = fmuWrapper->getRealParameters();
      for (auto it=realParameters.begin(); it != realParameters.end(); it++)
      {
        if (it->second.isSome())
        {
          pugi::xml_node parameter = subModel.append_child("Parameter");
          parameter.append_attribute("Type") = "Real";
          value = it->first;
          parameter.append_attribute("Name") = value.c_str();
          value = std::to_string(it->second.getValue());
          parameter.append_attribute("Value") = value.c_str();
        }
      }
    }
    else
      logWarning("SubModel is not an FMU");
  }

  pugi::xml_node nodeConnections = fmiCompositeModel.append_child("Connections");
  oms2::Connection** connections = model->getConnections();
  for (int i=0; connections[i]; ++i)
  {
    oms2::Connection* connection = connections[i];
    pugi::xml_node connectionNode = nodeConnections.append_child("Connection");
    value = connection->getSignalA().toString();
    connectionNode.append_attribute("From") = value.c_str();
    value = connection->getSignalB().toString();
    connectionNode.append_attribute("To") = value.c_str();

    // export ssd:ConnectionGeometry
    const oms2::ssd::ConnectionGeometry* connectionGeometry = connection->getGeometry();
    if (connectionGeometry && connectionGeometry->getLength() > 0)
    {
      pugi::xml_node node = connectionNode.append_child("ssd:ConnectionGeometry");
      const double* pointsX = connectionGeometry->getPointsX();
      const double* pointsY = connectionGeometry->getPointsY();
      std::string pointsXStr, pointsYStr;
      for (int i = 0 ; i < connectionGeometry->getLength() ; i++)
      {
        pointsXStr += std::to_string(pointsX[i]);
        pointsYStr += std::to_string(pointsY[i]);
        if (i != connectionGeometry->getLength() - 1)
        {
          pointsXStr += " ";
          pointsYStr += " ";
        }
      }
      node.append_attribute("pointsX") = pointsXStr.c_str();
      node.append_attribute("pointsY") = pointsYStr.c_str();
    }
  }

  // add experiment settings
  value = std::to_string(model->getStartTime());
  experiment.append_attribute("StartTime") = value.c_str();
  value = std::to_string(model->getStopTime());
  experiment.append_attribute("StopTime") = value.c_str();
  value = model->getResultFile();
  experiment.append_attribute("ResultFile") = value.c_str();

  if (!doc.save_file(filename.c_str()))
  {
    logError("[oms2::Scope::saveFMIModel] xml export failed for " + filename);
    return oms_status_error;
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::saveTLMModel(oms2::TLMCompositeModel* model, const std::string& filename)
{
  logError("[oms2::Scope::saveTLMModel]: not implemented yet");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getElement(const oms2::ComRef& cref, oms2::Element** element)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  if (!element)
  {
    logWarning("[oms2::Scope::getElement] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = scope.getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::getElement] failed");
      return oms_status_error;
    }
    *element = model->getElement();
    return oms_status_ok;
  }
  else
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = scope.getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getElement] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getElement] failed");
        return oms_status_error;
      }
      *element = subModel->getElement();
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::getElement] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getElements(const oms2::ComRef& cref, oms2::Element*** elements)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  if (!elements)
  {
    logWarning("[oms2::Scope::getElements] NULL pointer");
    return oms_status_warning;
  }

  Model* model = scope.getModel(cref);
  if (model && oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
    *elements = fmiModel->getElements();
    return oms_status_ok;
  }

  logError("[oms2::Scope::getElements] is only implemented for FMI models yet");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getFMUPath(const oms2::ComRef& cref, char** path)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = scope.getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getFMUPath] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getFMUPath] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      *path = const_cast<char*>(fmuWrapper->getFMUPath().c_str());
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::getFMUPath] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setElementGeometry(const oms2::ComRef& cref, const oms2::ssd::ElementGeometry* geometry)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  if (!geometry)
  {
    logWarning("[oms2::Scope::setElementGeometry] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = scope.getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setElementGeometry] failed");
      return oms_status_error;
    }
    model->setGeometry(*geometry);
    return oms_status_ok;
  }
  else
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = scope.getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setElementGeometry] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setElementGeometry] failed");
        return oms_status_error;
      }
      subModel->setGeometry(*geometry);
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::setElementGeometry] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getConnections(const oms2::ComRef& cref, oms2::Connection*** connections)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  if (!connections)
  {
    logWarning("[oms2::Scope::getConnections] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = scope.getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::getConnections] failed");
      return oms_status_error;
    }

    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      *connections = fmiModel->getConnections();
      return oms_status_ok;
    }

    return oms_status_error;
  }

  logError("[oms2::Scope::getConnections] is only implemented for FMI models yet");
  return oms_status_error;
}

oms2::Connection* oms2::Scope::getConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB)
{
  oms2::Model* model = getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::getConnection] failed");
    return NULL;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
    return fmiModel->getConnection(conA, conB);
  }

  logError("[oms2::Scope::getConnection] only implemented for FMI models");
  return NULL;
}

oms_status_enu_t oms2::Scope::addConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  oms2::Model* model = scope.getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::addConnection] failed");
    return oms_status_error;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
    return fmiModel->addConnection(conA, conB);
  }

  logError("[oms2::Scope::addConnection] only implemented for FMI models");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::deleteConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  oms2::Model* model = scope.getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::deleteConnection] failed");
    return oms_status_error;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
    return fmiModel->deleteConnection(conA, conB);
  }

  logError("[oms2::Scope::deleteConnection] only implemented for FMI models");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::updateConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB, const oms2::Connection* connection)
{
  oms2::Scope& scope = oms2::Scope::getInstance();

  oms2::Connection* connection_ = scope.getConnection(cref, conA, conB);

  if (connection_)
  {
    *connection_ = *connection;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::renameModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  if (!identNew.isValidIdent())
  {
    logError("Identifier \"" + identNew + "\" is invalid.");
    return oms_status_error;
  }

  if (!identOld.isValidIdent())
  {
    logError("Identifier \"" + identOld + "\" is invalid.");
    return oms_status_error;
  }

  // check if identNew is in scope
  auto it = models.find(identNew);
  if (it != models.end())
  {
    logError("A model called \"" + identNew.toString() + "\" is already in scope.");
    return oms_status_error;
  }

  // check if identOld is in scope
  it = models.find(identOld);
  if (it == models.end())
  {
    logError("There is no model called \"" + identOld.toString() + "\" in scope.");
    return oms_status_error;
  }

  it->second->setName(identNew);
  models[identNew] = it->second;
  models.erase(it);

  return oms_status_ok;
}

oms2::Model* oms2::Scope::getModel(const oms2::ComRef& name)
{
  auto it = models.find(name);
  if (it == models.end())
  {
    logWarning("There is no model called \"" + name + "\" in scope.");
    return NULL;
  }

  return it->second;
}

oms_status_enu_t oms2::Scope::getRealParameter(const oms2::SignalRef& signal, double& value)
{
  oms2::Scope& scope = oms2::Scope::getInstance();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = scope.getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getRealParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getRealParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      fmuWrapper->getRealParameter(var, value);
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::getRealParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setRealParameter(const oms2::SignalRef& signal, double value)
{
  oms2::Scope& scope = oms2::Scope::getInstance();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = scope.getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setRealParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = dynamic_cast<FMICompositeModel*>(model);
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setRealParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      fmuWrapper->setRealParameter(var, value);
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::setRealParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}
