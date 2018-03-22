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

#include "Scope.h"

#include "../Types.h"
#include "ComRef.h"
#include "FMICompositeModel.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "ssd/ConnectionGeometry.h"
#include "ssd/ElementGeometry.h"
#include "TLMCompositeModel.h"

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

oms2::Scope::Scope()
{
  logTrace();

  boost::filesystem::path tempPath = boost::filesystem::temp_directory_path();
  tempDir = tempPath.string();
}

oms2::Scope::~Scope()
{
  logTrace();

  // free memory if no one else does
  for (auto it=models.begin(); it != models.end(); it++)
    unloadModel(it->first);
}

oms2::Scope& oms2::Scope::GetInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms_status_enu_t oms2::Scope::newFMIModel(const oms2::ComRef& name)
{
  logTrace();

  // check if name is in scope
  auto it = models.find(name);
  if (it != models.end())
  {
    logError("A model called \"" + name + "\" is already in the scope.");
    return oms_status_error;
  }

  Model* model = oms2::Model::NewModel(oms_component_fmi, name);
  if (!model)
    return oms_status_error;

  models[name] = model;
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::newTLMModel(const oms2::ComRef& name)
{
  logTrace();

  // check if name is in scope
  auto it = models.find(name);
  if (it != models.end())
  {
    logError("A model called \"" + name + "\" is already in the scope.");
    return oms_status_error;
  }

  Model* model = oms2::Model::NewModel(oms_component_tlm, name);
  if (!model)
    return oms_status_error;

  models[name] = model;

  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::unloadModel(const oms2::ComRef& name)
{
  logTrace();

  // check if name is in scope
  auto it = models.find(name);
  if (it == models.end())
  {
    logError("[oms2::Scope::unloadModel] There is no model called \"" + name + "\" in the scope.");
    return oms_status_error;
  }

  oms2::Model::DeleteModel(it->second);
  models.erase(it);
  logInfo("Removed model from scope: " + name);

  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::addFMU(const oms2::ComRef& modelIdent, const std::string& fmuPath, const oms2::ComRef& fmuIdent)
{
  logTrace();

  oms2::Model* model = getModel(modelIdent);
  if (!model)
    return oms_status_error;

  if (oms_component_fmi != model->getType())
  {
    logError("[oms2::Scope::addFMU] \"" + modelIdent + "\" is not a FMI composite model.");
  }

  FMICompositeModel* fmiModel = model->getFMICompositeModel();
  return fmiModel->addFMU(fmuPath, fmuIdent);
}

oms_status_enu_t oms2::Scope::deleteSubModel(const oms2::ComRef& modelIdent, const oms2::ComRef& subModelIdent)
{
  logTrace();

  oms2::Model* model = getModel(modelIdent);
  if (!model)
    return oms_status_error;

  if (oms_component_fmi != model->getType())
  {
    logError("[oms2::Scope::deleteSubModel] \"" + modelIdent + "\" is not a FMI composite model.");
  }

  FMICompositeModel* fmiModel = model->getFMICompositeModel();
  return fmiModel->deleteSubModel(subModelIdent);
}

oms_status_enu_t oms2::Scope::rename(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  logTrace();

  // renaming a (root) model or a sub.model?
  if (identOld.first() == identNew.first())
  {
    auto it = models.find(identOld.first());
    if (it == models.end())
    {
      logError("[oms2::Scope::rename] no model called \"" + identOld.first() + "\" in scope.");
      return oms_status_error;
    }

    Model* model = it->second;

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
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
    return renameModel(identOld, identNew);
  }

  logError("[oms2::Scope::rename] failed");
  return oms_status_error;
}

oms2::Model* oms2::Scope::loadModel(const std::string& filename)
{
  logTrace();

  oms2::Model* model = oms2::Model::LoadModel(filename);
  if (!model)
    return NULL;

  oms2::ComRef cref = model->getName();
  models[cref] = model;
  return model;
}

oms_status_enu_t oms2::Scope::saveModel(const std::string& filename, const oms2::ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->save(filename);
}

oms_status_enu_t oms2::Scope::setTempDirectory(const std::string& newTempDir)
{
  logTrace();

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
  tempDir = path.string();

  logInfo("New temp directory: \"" + std::string(tempDir) + "\"");
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::setWorkingDirectory(const std::string& path)
{
  logTrace();

  boost::filesystem::path path_(path.c_str());
  if (!boost::filesystem::is_directory(path_))
  {
    logError("Set working directory to \"" + path + "\" failed");
    return oms_status_error;
  }

  boost::filesystem::current_path(path_);
  workingDir = path;
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::getElement(const oms2::ComRef& cref, oms2::Element** element)
{
  logTrace();

  if (!element)
  {
    logWarning("[oms2::Scope::getElement] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
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
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getElement] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
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
  logTrace();

  if (!elements)
  {
    logWarning("[oms2::Scope::getElements] NULL pointer");
    return oms_status_warning;
  }

  Model* model = getModel(cref);
  if (model && oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    *elements = fmiModel->getElements();
    return oms_status_ok;
  }

  logError("[oms2::Scope::getElements] is only implemented for FMI models yet");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getFMUPath(const oms2::ComRef& cref, char** path)
{
  logTrace();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getFMUPath] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
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

oms_status_enu_t oms2::Scope::getFMUInfo(const oms2::ComRef& cref, const oms2::FMUInfo** fmuInfo)
{
  logTrace();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getFMUInfo] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getFMUInfo] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      *fmuInfo = fmuWrapper->getFMUInfo();
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::getFMUInfo] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setElementGeometry(const oms2::ComRef& cref, const oms2::ssd::ElementGeometry* geometry)
{
  logTrace();

  if (!geometry)
  {
    logWarning("[oms2::Scope::setElementGeometry] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setElementGeometry] failed");
      return oms_status_error;
    }
    if (model->getFMICompositeModel())
      model->getFMICompositeModel()->setGeometry(*geometry);
    else if (model->getTLMCompositeModel())
      model->getTLMCompositeModel()->setGeometry(*geometry);
    else
      return oms_status_error;
    return oms_status_ok;
  }
  else
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setElementGeometry] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
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
  logTrace();

  if (!connections)
  {
    logWarning("[oms2::Scope::getConnections] NULL pointer");
    return oms_status_warning;
  }

  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::getConnections] failed");
      return oms_status_error;
    }

    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
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
  logTrace();

  oms2::Model* model = getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::getConnection] failed");
    return NULL;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    return fmiModel->getConnection(conA, conB);
  }

  logError("[oms2::Scope::getConnection] only implemented for FMI models");
  return NULL;
}

oms_status_enu_t oms2::Scope::addConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  logTrace();

  oms2::Model* model = getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::addConnection] failed");
    return oms_status_error;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    return fmiModel->addConnection(conA, conB);
  }

  logError("[oms2::Scope::addConnection] only implemented for FMI models");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::deleteConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  logTrace();

  oms2::Model* model = getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::deleteConnection] failed");
    return oms_status_error;
  }

  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    return fmiModel->deleteConnection(conA, conB);
  }

  logError("[oms2::Scope::deleteConnection] only implemented for FMI models");
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::updateConnection(const oms2::ComRef& cref, const oms2::SignalRef& conA, const oms2::SignalRef& conB, const oms2::Connection* connection)
{
  logTrace();

  oms2::Connection* connection_ = getConnection(cref, conA, conB);

  if (connection_)
  {
    *connection_ = *connection;
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::renameModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  logTrace();

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
  logTrace();

  auto it = models.find(name);
  if (it == models.end())
  {
    logWarning("There is no model called \"" + name + "\" in scope.");
    return NULL;
  }

  return it->second;
}

oms2::FMICompositeModel* oms2::Scope::getFMICompositeModel(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return NULL;
  return model->getFMICompositeModel();
}

oms2::TLMCompositeModel* oms2::Scope::getTLMCompositeModel(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return NULL;
  return model->getTLMCompositeModel();
}

oms_status_enu_t oms2::Scope::getRealParameter(const oms2::SignalRef& signal, double& value)
{
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getRealParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getRealParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getRealParameter(var, value);
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
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setRealParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setRealParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setRealParameter(var, value);
    }
    else
    {
      logError("[oms2::Scope::setRealParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getIntegerParameter(const oms2::SignalRef& signal, int& value)
{
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getIntegerParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getIntegerParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getIntegerParameter(var, value);
    }
    else
    {
      logError("[oms2::Scope::getIntegerParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setIntegerParameter(const oms2::SignalRef& signal, int value)
{
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setIntegerParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setIntegerParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setIntegerParameter(var, value);
    }
    else
    {
      logError("[oms2::Scope::setIntegerParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getBooleanParameter(const oms2::SignalRef& signal, bool& value)
{
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getBooleanParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getBooleanParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getBooleanParameter(var, value);
    }
    else
    {
      logError("[oms2::Scope::getBooleanParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setBooleanParameter(const oms2::SignalRef& signal, bool value)
{
  logTrace();
  ComRef cref = signal.getCref();
  std::string var = signal.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setBooleanParameter] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setBooleanParameter] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setBooleanParameter(var, value);
    }
    else
    {
      logError("[oms2::Scope::setBooleanParameter] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::addFMISubModel(const oms2::ComRef &cref, const oms2::ComRef &subref)
{
  Model* model = getModel(cref);
  if(!model) {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("FMI sub-models can only be added to TLM composite models.");
    return oms_status_error;
  }
  TLMCompositeModel *tlmmodel = dynamic_cast<oms2::TLMCompositeModel*>(model);
  model = getModel(subref);
  if(!model) {
    logError("Model: "+subref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_fmi) {
    logError("Model: "+subref.toString()+" is not an FMI composite model.");
    return oms_status_error;
  }
  FMICompositeModel *fmimodel = dynamic_cast<oms2::FMICompositeModel*>(model);

  return tlmmodel->addFMIModel(fmimodel);
}

oms_status_enu_t oms2::Scope::addExternalModel(const oms2::ComRef &cref, const oms2::ComRef &name, const std::string &modelfile, const std::string &startscript)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("External models can only be added to TLM composite models.");
    return oms_status_error;
  }
  TLMCompositeModel *tlmmodel = dynamic_cast<oms2::TLMCompositeModel*>(model);
  if(!tlmmodel) {
    return oms_status_error;    //Should never happen, but check just in case
  }

  tlmmodel->addExternalModel(modelfile, startscript, name);
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::addTLMInterface(const oms2::ComRef &cref, const oms2::ComRef &subref, const oms2::ComRef &name, int dimensions, oms_tlm_causality_t causality, std::string domain)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("External models can only be added to TLM composite models.");
    return oms_status_error;
  }
  TLMCompositeModel *tlmmodel = dynamic_cast<oms2::TLMCompositeModel*>(model);
  if(!tlmmodel) {
    return oms_status_error;    //Should never happen, but check just in case
  }

  return tlmmodel->addInterface(name.toString(), dimensions, causality, domain, subref);
}


oms_status_enu_t oms2::Scope::addTLMConnection(const oms2::ComRef &cref, const oms2::SignalRef &from, const oms2::SignalRef &to,
                                           double delay, double alpha, double Zf, double Zfr)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("External models can only be added to TLM composite models.");
    return oms_status_error;
  }
  TLMCompositeModel *tlmmodel = dynamic_cast<oms2::TLMCompositeModel*>(model);
  if(!tlmmodel) {
    return oms_status_error;    //Should never happen, but check just in case
  }

  return tlmmodel->addConnection(from, to, delay, alpha, Zf, Zfr);
}

oms_status_enu_t oms2::Scope::describeModel(const oms2::ComRef &cref)
{
  oms2::Model* model = getModel(cref);
  if (!model)
  {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }

  return model->describe();
}

oms_status_enu_t oms2::Scope::setStartTime(const ComRef& cref, double startTime)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setStartTime] failed");
      return oms_status_error;
    }
    model->setStartTime(startTime);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setStopTime(const ComRef& cref, double stopTime)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setStartTime] failed");
      return oms_status_error;
    }
    model->setStopTime(stopTime);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setResultFile(const ComRef& cref, const std::string& filename)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setStartTime] failed");
      return oms_status_error;
    }
    model->setResultFile(filename);
    return oms_status_ok;
  }
  return oms_status_error;
}
