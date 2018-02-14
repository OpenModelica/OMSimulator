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

#include "oms2_ComRef.h"
#include "oms2_FMICompositeModel.h"
#include "oms2_Logging.h"
#include "oms2_Scope.h"
#include "oms2_TLMCompositeModel.h"
#include "Types.h"

#include <iostream>

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
    oms2::Model::deleteModel(it->second);
}

oms2::Scope& oms2::Scope::getInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms_status_t oms2::Scope::newFMIModel(const ComRef& name)
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

oms_status_t oms2::Scope::newTLMModel(const ComRef& name)
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

oms_status_t oms2::Scope::unloadModel(const ComRef& name)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  auto it = scope.models.find(name);
  if (it == scope.models.end())
  {
    logError("There is no model called \"" + name + "\" in the scope.");
    return oms_status_error;
  }

  oms2::Model::deleteModel(it->second);
  scope.models.erase(it);

  return oms_status_ok;
}

oms_status_t oms2::Scope::renameModel(const ComRef& identOld, const ComRef& identNew)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if identNew is in scope
  auto it = scope.models.find(identNew);
  if (it != scope.models.end())
  {
    logError("A model called \"" + identNew.toString() + "\" is already in the scope.");
    return oms_status_error;
  }

  // check if identOld is in scope
  if (it == scope.models.end())
  {
    logError("There is no model called \"" + identOld.toString() + "\" in the scope.");
    return oms_status_error;
  }

  it->second->setName(identNew);
  scope.models[identNew] = it->second;
  scope.models.erase(it);

  return oms_status_ok;
}

oms2::Model* oms2::Scope::getModel(const ComRef& name)
{
  Scope& scope = oms2::Scope::getInstance();

  auto it = scope.models.find(name);
  if (it == scope.models.end())
  {
    logWarning("oms2::Scope::getModel: There is no model called \"" + name + "\" in the scope.");
    return NULL;
  }

  return it->second;
}

oms2::Model* oms2::Scope::loadModel(const std::string& filename)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
  {
    logError("[oms2::Scope::loadModel] failed for \"" + std::string(filename) + "\"");
    return NULL;
  }

  pugi::xml_node root = doc.document_element();
  oms_component_type_t modelType = oms_component_none;

  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if (name == "TLMModel" && modelType == oms_component_none)
      modelType = oms_component_tlm;
    else if (name == "FMIModel" && modelType == oms_component_none)
      modelType = oms_component_fmi;
    else
    {
      logError("[oms2::Scope::loadModel] wrong xml schema detected: \"" + std::string(filename) + "\"");
      return NULL;
    }
  }

  if (modelType == oms_component_fmi)
    return loadFMIModel(root.child("FMIModel"));
  else if (modelType == oms_component_tlm)
    return loadTLMModel(root.child("TLMModel"));

  logError("[oms2::Scope::loadModel] failed");
  return NULL;
}

oms2::Model* oms2::Scope::loadFMIModel(const pugi::xml_node& xml)
{
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
  oms_status_t status = newFMIModel(cref_model);
  if (status != oms_status_ok)
  {
    logError("[oms2::Scope::loadFMIModel]");
    return NULL;
  }

  // import components
  FMICompositeModel* model = dynamic_cast<FMICompositeModel*>(getModel(cref_model));

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

          if (filename.length() > 5)
            type = filename.substr(filename.length() - 4);
          else
          {
            logError("[oms2::Scope::loadFMIModel] Invalid filename: " + filename);
            unloadModel(cref_model);
            return NULL;
          }
        }
      }

      oms2::ComRef cref_submodel = cref_model + ComRef(instancename);

      oms_status_t status = oms_status_error;
      if (type == ".fmu" || type == ".FMU")
        status = model->instantiateFMU(filename, cref_submodel.last());
      else
        status = model->instantiateTable(filename, cref_submodel.last());

      if (oms_status_ok != status)
      {
        unloadModel(cref_model);
        return NULL;
      }

      // import parameters
      for (auto parameter = it->first_child(); parameter; parameter = parameter.next_sibling())
      {
        if (std::string(parameter.name()) != "Parameter")
        {
          logError("[oms2::Scope::loadFMIModel] wrong xml schema detected");
          unloadModel(cref_model);
          return NULL;
        }
        std::string _type = parameter.attribute("Type").as_string();
        std::string _name = parameter.attribute("Name").as_string();

        if (_type == "Real")
        {
          double realValue = parameter.attribute("Value").as_double();
          if (oms_status_ok != model->setRealParameter(oms2::SignalRef(cref_submodel, _name), realValue))
          {
            logError("[oms2::Scope::loadFMIModel] wrong xml schema detected");
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
    }// instantiate FMUs
    else
    {
      logError("[oms2::Scope::loadFMIModel] wrong xml schema detected");
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

oms_status_t oms2::Scope::SetTempDirectory(const std::string& newTempDir)
{
  Scope& scope = oms2::Scope::getInstance();

  if (!boost::filesystem::is_directory(newTempDir))
  {
    if (!boost::filesystem::create_directory(newTempDir))
    {
      logError("changing working directory to \"" + std::string(newTempDir) + "\" failed");
      return oms_status_error;
    }
    else
      logInfo("new temp directory has been created: \"" + std::string(newTempDir) + "\"");
  }

  boost::filesystem::path path(newTempDir.c_str());
  path = boost::filesystem::canonical(path);
  scope.tempDir = path.string();

  logInfo("new temp directory: \"" + std::string(scope.tempDir) + "\"");
  return oms_status_ok;
}
