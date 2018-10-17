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

#include "Flags.h"
#include "System.h"

#include <OMSBoost.h>
#include <miniunz.h>

oms3::Scope::Scope()
  : tempDir(".")
{
  this->models.push_back(NULL);

  boost::filesystem::path tempDir = oms_temp_directory_path() / "omsimulator";
  setTempDirectory(tempDir.string());

  setWorkingDirectory(".");
}

oms3::Scope::~Scope()
{
  // free memory if no one else does
  for (const auto& model : models)
    if (model)
      delete model;
}

oms3::Scope& oms3::Scope::GetInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms_status_enu_t oms3::Scope::newModel(const oms3::ComRef& cref)
{
  // check if cref is in scope
  if (getModel(cref))
    return logError("A model \"" + std::string(cref) + "\" already exists in the scope");

  Model* model = oms3::Model::NewModel(cref);
  if (!model)
    return oms_status_error;

  models.back() = model;
  models_map[cref] = models.size() - 1;
  models.push_back(NULL);

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::deleteModel(const oms3::ComRef& cref)
{
  auto it = models_map.find(cref);
  if (it == models_map.end())
    return logError("Model \"" + std::string(cref) + "\" does not exist in the scope");
  delete models[it->second];

  models.pop_back();
  models[it->second] = models.back();
  models.back() = NULL;

  // update models_map
  if (models[it->second])
    models_map[models[it->second]->getName()] = it->second;
  models_map.erase(it);

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::renameModel(const oms3::ComRef& cref, const oms3::ComRef& newCref)
{
  auto it = models_map.find(cref);
  if (it == models_map.end())
    return logError("Model \"" + std::string(cref) + "\" does not exist in the scope");

  unsigned int index = it->second;
  oms_status_enu_t status = models[index]->rename(newCref);
  if (oms_status_ok != status)
    return status;

  models_map.erase(it);
  models_map[newCref] = index;

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::exportModel(const oms3::ComRef& cref, const std::string& filename)
{
  oms3::Model* model = getModel(cref);
  if (!model)
    return logError("Model \"" + std::string(cref) + "\" does not exist in the scope");

  return model->exportToFile(filename);
}

oms_status_enu_t oms3::Scope::miniunz(const std::string& filename, const std::string& extractdir, bool systemStructure)
{
  // Usage: miniunz [-e] [-x] [-v] [-l] [-o] [-p password] file.zip [file_to_extr.] [-d extractdir]
  //        -e  Extract without pathname (junk paths)
  //        -x  Extract with pathname
  //        -v  list files
  //        -l  list files
  //        -d  directory to extract into
  //        -o  overwrite files without prompting
  //        -p  extract crypted file using password

  std::string cd = Scope::GetInstance().getWorkingDirectory();

  int argc = systemStructure ? 6 : 5;
  char **argv = new char*[argc];
  int i=0;
  argv[i++]=(char*)"miniunz";
  argv[i++]=(char*)"-xo";
  argv[i++]=(char*)filename.c_str();
  if (systemStructure)
    argv[i++]=(char*)"SystemStructure.ssd";
  argv[i++]=(char*)"-d";
  argv[i++]=(char*)extractdir.c_str();
  int status = ::miniunz(argc, argv);
  delete[] argv;
  std::string cd2 = Scope::GetInstance().getWorkingDirectory();
  if (cd != cd2)
    Scope::GetInstance().setWorkingDirectory(cd);

  if (status == 0)
    return oms_status_ok;
  return oms_status_error;
}

oms_status_enu_t oms3::Scope::importModel(const std::string& filename, char** _cref)
{
  *_cref = NULL;

  std::string extension = "";
  if (filename.length() > 5)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssp")
    return logError("filename extension must be \".ssp\"; no other formats are supported");

  // extract SystemStructure.ssd to temp
  boost::filesystem::path temp_root(getTempDirectory());
  if (oms_status_ok != oms3::Scope::miniunz(filename, temp_root.string(), true))
    return logError("failed to extract \"SystemStructure.ssd\" from \"" + filename + "\"");

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file((temp_root / "SystemStructure.ssd").string().c_str());
  if (!result)
    return logError("loading \"" + std::string(filename) + "\" failed (" + std::string(result.description()) + ")");

  const pugi::xml_node node = doc.document_element(); // ssd:SystemStructureDescription

  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string ssdVersion = node.attribute("version").as_string();

  oms_status_enu_t status = newModel(cref);
  if (oms_status_ok != status)
    return status;

  if (ssdVersion != "Draft20180219")
    logWarning("Unknown SSD version: " + ssdVersion);

  Model* model = getModel(cref);
  if (!model) // that should be impossible
    return oms_status_error;


  // extract the ssp file
  oms3::Scope::miniunz(filename, model->getTempDirectory(), false);

  std::string cd = Scope::GetInstance().getWorkingDirectory();
  Scope::GetInstance().setWorkingDirectory(model->getTempDirectory());
  model->copyResources(false);
  status = model->importFromSSD(node);
  model->copyResources(true);
  Scope::GetInstance().setWorkingDirectory(cd);
  if (oms_status_ok != status)
  {
    deleteModel(cref);
    return oms_status_error;
  }

  *_cref = (char*)model->getName().c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::setTempDirectory(const std::string& newTempDir)
{
  if (!boost::filesystem::is_directory(newTempDir))
  {
    if (!boost::filesystem::create_directory(newTempDir))
      return logError("Changing temp directory to \"" + newTempDir + "\" failed");
    else if (!Flags::SuppressPath())
      logInfo("New temp directory has been created: \"" + newTempDir + "\"");
  }

  boost::filesystem::path path(newTempDir.c_str());
  try
  {
    path = oms_canonical(path);
  }
  catch(std::exception e)
  {
    // do nothing, canonical fails if the directory contains a junction or a symlink!
    // https://svn.boost.org/trac10/ticket/11138
  }

  this->tempDir = path.string();

  if (Flags::SuppressPath())
    logInfo("Set temp directory to    <suppressed>");
  else
    logInfo("Set temp directory to    \"" + this->tempDir + "\"");

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::setWorkingDirectory(const std::string& newWorkingDir)
{
  boost::filesystem::path path(newWorkingDir.c_str());
  if (!boost::filesystem::is_directory(path))
    return logError("Set working directory to \"" + newWorkingDir + "\" failed");

  boost::filesystem::current_path(path);
  try
  {
    path = oms_canonical(path);
  }
  catch(std::exception e)
  {
    // do nothing, canonical fails if the directory contains a junction or a symlink!
    // https://svn.boost.org/trac10/ticket/11138
  }

  if (Flags::SuppressPath())
    logInfo("Set working directory to <suppressed>");
  else
    logInfo("Set working directory to \"" + newWorkingDir + "\"");

  return oms_status_ok;
}

std::string oms3::Scope::getWorkingDirectory()
{
  boost::filesystem::path workingDir(boost::filesystem::current_path());
  return workingDir.string();
}

oms_status_enu_t oms3::Scope::getElement(const oms3::ComRef& cref, oms3::Element** element)
{
  if (!element)
  {
    logWarning("[oms3::Scope::getElement] NULL pointer");
    return oms_status_warning;
  }

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();
  oms3::Model* model = getModel(front);
  if (!model)
    return logError("Model \"" + std::string(front) + "\" does not exist in the scope");

  if (cref.isValidIdent())
    return logError("A model has no element information");

  oms3::System* system = model->getSystem(tail);
  if (!system)
    return logError("Model \"" + std::string(front) + "\" does not contain system \"" + std::string(tail) + "\"");

  *element = system->getElement();
  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::getElements(const oms3::ComRef& cref, oms3::Element*** elements)
{
  if (!elements)
  {
    logWarning("[oms3::Scope::getElements] NULL pointer");
    return oms_status_warning;
  }

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();
  oms3::Model* model = getModel(front);
  if (!model)
    return logError("Model \"" + std::string(front) + "\" does not exist in the scope");

  if (cref.isValidIdent())
  {
    *elements = model->getElements();
    return oms_status_ok;
  }

  return logError("Only implemented for model identifiers");
  return oms_status_error;
}

oms3::Model* oms3::Scope::getModel(const oms3::ComRef& cref)
{
  auto it = models_map.find(cref);
  if (it == models_map.end())
    return NULL;

  return models[it->second];
}

/* ************************************ */
/* oms2                                 */
/*                                      */
/*                                      */
/* ************************************ */

#include "Types.h"
#include "ComRef.h"
#include "FMICompositeModel.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "ssd/ConnectionGeometry.h"
#include "ssd/ElementGeometry.h"
#include "TLMCompositeModel.h"

#include <iostream>
#include <sstream>

oms2::Scope::Scope()
{
  logTrace();
  boost::filesystem::path tempPath = oms_temp_directory_path();
  tempDir = tempPath.string();
}

oms2::Scope::~Scope()
{
  logTrace();

  // free memory if no one else does
  for (auto it=models.begin(); it != models.end(); it++)
    oms2::Model::DeleteModel(it->second);
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
    return logError("A model called \"" + name + "\" is already in the scope.");

  Model* model = oms2::Model::NewModel(oms_component_fmi, name);
  if (!model)
    return oms_status_error;

  models[name] = model;

  return oms_status_ok;
}

#if !defined(NO_TLM)
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
#endif

oms_status_enu_t oms2::Scope::unloadModel(const oms2::ComRef name)
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

oms_status_enu_t oms2::Scope::addTable(const ComRef& modelIdent, const std::string& tablePath, const ComRef& tableIdent)
{
  logTrace();

  oms2::Model* model = getModel(modelIdent);
  if (!model)
    return oms_status_error;

  FMICompositeModel* fmiModel = model->getFMICompositeModel();
  if (!fmiModel)
    return oms_status_error;

  return fmiModel->addTable(tablePath, tableIdent);
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

oms_status_enu_t oms2::Scope::parseString(const std::string& contents, char** ident)
{
  logTrace();
  return oms2::Model::ParseString(contents, ident);
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

oms_status_enu_t oms2::Scope::saveModel(const oms2::ComRef& name, const std::string& filename)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->save(filename);
}

oms_status_enu_t oms2::Scope::listModel(const oms2::ComRef& name, char** contents)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->list(contents);
}

oms_status_enu_t oms2::Scope::initialize(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->initialize();
}

oms_status_enu_t oms2::Scope::reset(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->reset();
}

oms_status_enu_t oms2::Scope::simulate(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->simulate();
}

oms_status_enu_t oms2::Scope::simulate_realtime(const ComRef& name)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->simulate_realtime();
}

oms_status_enu_t oms2::Scope::simulate_asynchronous(const ComRef& name, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->simulate_asynchronous(cb);
}

oms_status_enu_t oms2::Scope::doSteps(const ComRef& name, const int numberOfSteps)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->doSteps(numberOfSteps);
}

oms_status_enu_t oms2::Scope::stepUntil(const ComRef& name, const double timeValue)
{
  logTrace();

  oms2::Model* model = getModel(name);
  if (!model)
    return oms_status_error;

  return model->stepUntil(timeValue);
}

oms_status_enu_t oms2::Scope::setTempDirectory(const std::string& newTempDir)
{
  logTrace();

  if (!boost::filesystem::is_directory(newTempDir))
  {
    if (!boost::filesystem::create_directory(newTempDir))
    {
      logError("Changing temp directory to \"" + newTempDir + "\" failed");
      return oms_status_error;
    }
    else
      logInfo("New temp directory has been created: \"" + newTempDir + "\"");
  }

  boost::filesystem::path path(newTempDir.c_str());
  try
  {
    path = oms_canonical(path);
  }
  catch(std::exception e)
  {
    // do nothing, canonical fails if the directory contains a junction or a symlink!
    // https://svn.boost.org/trac10/ticket/11138
  }
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

oms_status_enu_t oms2::Scope::getSubModelPath(const oms2::ComRef& cref, char** path)
{
  logTrace();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::getSubModelPath] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getSubModelPath] failed");
        return oms_status_error;
      }
      *path = const_cast<char*>(subModel->getPath().c_str());
      return oms_status_ok;
    }
    else
    {
      logError("[oms2::Scope::getSubModelPath] is only implemented for FMI models yet");
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
#if !defined(NO_TLM)
    else if (model->getTLMCompositeModel())
      model->getTLMCompositeModel()->setGeometry(*geometry);
#endif
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

oms_status_enu_t oms2::Scope::setConnectorGeometry(const oms2::SignalRef& connector, const oms2::ssd::ConnectorGeometry* geometry)
{
  logTrace();

  if (!geometry)
  {
    logWarning("[oms2::Scope::setConnectorGeometry] NULL pointer");
    return oms_status_warning;
  }

  ComRef cref = connector.getCref();
  std::string var = connector.getVar();

  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setConnectorGeometry] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setConnectorGeometry] failed");
        return oms_status_error;
      }

      oms2::Element* element = subModel->getElement();
      if (!element)
      {
        logError("[oms2::Scope::setConnectorGeometry] failed");
        return oms_status_error;
      }
      oms2::Connector** connectors = element->getConnectors();
      for (int i=0; connectors[i]; ++i)
      {
        if (connector == connectors[i]->getName())
        {
          connectors[i]->setGeometry(geometry);
          return oms_status_ok;
        }
      }
    }
    else
    {
      logError("[oms2::Scope::setConnectorGeometry] is only implemented for FMI models yet");
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

oms2::Model* oms2::Scope::getModel(const oms2::ComRef& name, bool showWarning)
{
  logTrace();

  auto it = models.find(name);
  if (it == models.end())
  {
    if (showWarning)
      logWarning("There is no model called \"" + name + "\" in scope.");
    return NULL;
  }

  return it->second;
}

bool oms2::Scope::hasFMICompositeModel(const oms2::ComRef &name)
{
  logTrace();

  return (models.find(name) != models.end());
}

#if !defined(NO_TLM)
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
#endif

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

oms_status_enu_t oms2::Scope::getReal(const oms2::SignalRef& signal, double& value)
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
      logError("[oms2::Scope::getReal] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getReal] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getReal(signal, value);
    }
    else
    {
      logError("[oms2::Scope::getReal] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setReal(const oms2::SignalRef& signal, double value)
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
      logError("[oms2::Scope::setReal] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setReal] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setReal(signal, value);
    }
    else
    {
      logError("[oms2::Scope::setReal] is only implemented for FMI models yet");
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

oms_status_enu_t oms2::Scope::getInteger(const oms2::SignalRef& signal, int& value)
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
      logError("[oms2::Scope::getInteger] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getInteger] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getInteger(signal, value);
    }
    else
    {
      logError("[oms2::Scope::getInteger] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setInteger(const oms2::SignalRef& signal, int value)
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
      logError("[oms2::Scope::setInteger] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setInteger] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setInteger(signal, value);
    }
    else
    {
      logError("[oms2::Scope::setInteger] is only implemented for FMI models yet");
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

oms_status_enu_t oms2::Scope::getBoolean(const oms2::SignalRef& signal, bool& value)
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
      logError("[oms2::Scope::getBoolean] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::getBoolean] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->getBoolean(signal, value);
    }
    else
    {
      logError("[oms2::Scope::getBoolean] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }

  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setBoolean(const oms2::SignalRef& signal, bool value)
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
      logError("[oms2::Scope::setBoolean] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setBoolean] failed");
        return oms_status_error;
      }
      oms2::FMUWrapper* fmuWrapper = dynamic_cast<oms2::FMUWrapper*>(subModel);
      return fmuWrapper->setBoolean(signal, value);
    }
    else
    {
      logError("[oms2::Scope::setBoolean] is only implemented for FMI models yet");
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

#if !defined(NO_TLM)
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
  TLMCompositeModel *tlmmodel = model->getTLMCompositeModel();
  model = getModel(subref);
  if(!model) {
    logError("Model: "+subref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_fmi) {
    logError("Model: "+subref.toString()+" is not an FMI composite model.");
    return oms_status_error;
  }
  FMICompositeModel *fmimodel = model->getFMICompositeModel();

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
  return model->getTLMCompositeModel()->addExternalModel(modelfile, startscript, name);
}

oms_status_enu_t oms2::Scope::addTLMInterface(const oms2::ComRef &cref, const oms2::ComRef &subref, const oms2::ComRef &name, int dimensions, oms_causality_enu_t causality, std::string domain, oms_tlm_interpolation_t interpolation, std::vector<SignalRef> &sigrefs)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("Model: "+cref.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("TLM interfaces can only be added to TLM composite models.");
    return oms_status_error;
  }

  return model->getTLMCompositeModel()->addInterface(name.toString(), dimensions, causality, domain, interpolation, subref, sigrefs);
}

oms_status_enu_t oms2::Scope::setTLMPositionAndOrientation(const oms2::ComRef &cref, const SignalRef &ifc, std::vector<double> x, std::vector<double> A)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("In Scope::setTLMPositionAndOrientation(): Model \""+cref.toString()+"\" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("In Scope::setTLMPositionAndOrientation(): Not a TLM model.");
    return oms_status_error;
  }
  return model->getTLMCompositeModel()->setPositionAndOrientation(ifc,x,A);
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

  return model->getTLMCompositeModel()->addConnection(from, to, delay, alpha, Zf, Zfr);
}

oms_status_enu_t oms2::Scope::setTLMSocketData(oms2::ComRef modelIdent,
                                               const std::string& address,
                                               int managerPort,
                                               int monitorPort)
{
  Model* model = getModel(modelIdent);
  if (!model) {
    logError("Model: "+modelIdent.toString()+" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("Can only set socket data on TLM models.");
    return oms_status_error;
  }
  TLMCompositeModel *tlmModel = model->getTLMCompositeModel();

  return tlmModel->setSocketData(address, managerPort, monitorPort);
}

oms_status_enu_t oms2::Scope::setTLMInitialValues(const oms2::ComRef &cref, const oms2::SignalRef &ifc, std::vector<double> values)
{
  Model *model = getModel(cref);
  if(!model) {
    logError("In Scope::setTLMSocketData(): Model \""+cref.toString()+"\" not found.");
    return oms_status_error;
  }

  return model->setTLMInitialValues(ifc, values);
}

oms_status_enu_t oms2::Scope::setTLMLoggingLevel(const oms2::ComRef &cref, int level)
{
  oms2::Model* model = getModel(cref);
  if (!model) {
    logError("In Scope::setTLMLoggingLevel(): Model \""+cref.toString()+"\" not found.");
    return oms_status_error;
  }
  if(model->getType() != oms_component_tlm) {
    logError("In Scope::setTLMLoggingLevel(): Not a TLM model.");
    return oms_status_error;
  }
  model->getTLMCompositeModel()->setLoggingLevel(level);
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::setLoggingSamples(const oms2::ComRef &cref, int loggingSamples)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setLoggingInterval] failed");
      return oms_status_error;
    }
    model->setLoggingSamples(loggingSamples);
    return oms_status_ok;
  }
  return oms_status_error;
}
#endif

oms_status_enu_t oms2::Scope::describeModel(const oms2::ComRef &cref)
{
  oms2::Model* model = getModel(cref);
  if (!model)
    return logError("Model: " + cref.toString() + " not found.");

  return model->describe();
}

oms_status_enu_t oms2::Scope::getStartTime(const ComRef& cref, double* startTime)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::getStartTime] failed");
      return oms_status_error;
    }
    *startTime = model->getStartTime();
    return oms_status_ok;
  }
  return oms_status_error;
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

oms_status_enu_t oms2::Scope::getStopTime(const ComRef& cref, double* stopTime)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::getStopTime] failed");
      return oms_status_error;
    }
    *stopTime = model->getStopTime();
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
      logError("[oms2::Scope::setStopTime] failed");
      return oms_status_error;
    }
    model->setStopTime(stopTime);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setCommunicationInterval(const ComRef& cref, double communicationInterval)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setCommunicationInterval] failed");
      return oms_status_error;
    }
    model->setCommunicationInterval(communicationInterval);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setLoggingInterval(const ComRef& cref, double loggingInterval)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setLoggingInterval] failed");
      return oms_status_error;
    }
    model->setLoggingInterval(loggingInterval);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setResultFile(const ComRef& cref, const std::string& filename, unsigned int bufferSize)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::setResultFile] failed");
      return oms_status_error;
    }
    model->setResultFile(filename, bufferSize);
    return oms_status_ok;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setMasterAlgorithm(const ComRef& cref, const std::string& masterAlgorithm)
{
  if (!cref.isIdent())
  {
    logError(std::string("[oms2::Scope::setMasterAlgorithm] require non-qualified component name, but got ") + cref.toString());
    return oms_status_error;
  }

  // Model
  Model* model = getModel(cref);
  if (!model)
  {
    logError("[oms2::Scope::setMasterAlgorithm] failed");
    return oms_status_error;
  }

  if (masterAlgorithm == "standard")
  {
    model->setMasterAlgorithm(oms2::MasterAlgorithm::STANDARD);
  }
  else if (masterAlgorithm == "pctpl")
  {
    model->setMasterAlgorithm(oms2::MasterAlgorithm::PCTPL);
  }
  else if (masterAlgorithm == "pmrchannela")
  {
    model->setMasterAlgorithm(oms2::MasterAlgorithm::PMRCHANNELA);
  }
  else if (masterAlgorithm == "pmrchannelcv")
  {
    model->setMasterAlgorithm(oms2::MasterAlgorithm::PMRCHANNELCV);
  }
  else if (masterAlgorithm == "pmrchannelm")
  {
    model->setMasterAlgorithm(oms2::MasterAlgorithm::PMRCHANNELM);
  }
  else {
    std::string message = std::string("Unsupported master algorithm ")
      + masterAlgorithm + std::string("\nFollowing master algorithms are supported:\n")
      + std::string("standard, pctpl.\n")
      + std::string("standard, pmrchannela.\n")
      + std::string("standard, pmrchannelcv.\n")
      + std::string("standard, pmrchannelm.\n")
      + std::string("Defaulting to \"standard\"!\n");
    logWarning(message);
    model->setMasterAlgorithm(oms2::MasterAlgorithm::STANDARD);
    return oms_status_warning;
  }

  return oms_status_ok;
}


oms_status_enu_t oms2::Scope::setActivationRatio(const ComRef& cref, int k)
{
  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setActivationRatio] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setActivationRatio] failed");
        return oms_status_error;
      }
      subModel->setActivationRatio(k);
    }
    else
    {
      logError("[oms2::Scope::setActivationRatio] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::Scope::exportCompositeStructure(const ComRef& cref, const std::string& filename)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::exportCompositeStructure] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      return fmiModel->exportCompositeStructure(filename);
    }

    logError("[oms2::Scope::exportCompositeStructure] is only implemented for FMI models yet");
    return oms_status_error;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::exportDependencyGraphs(const ComRef& cref, const std::string& initialization, const std::string& simulation)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::exportDependencyGraphs] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      return fmiModel->exportDependencyGraphs(initialization, simulation);
    }

    logError("[oms2::Scope::exportDependencyGraphs] is only implemented for FMI models yet");
    return oms_status_error;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::getCurrentTime(const oms2::ComRef& cref, double* time)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::exportDependencyGraphs] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      *time = fmiModel->getCurrentTime();
      return oms_status_ok;
    }

    logError("[oms2::Scope::getCurrentTime] is only implemented for FMI models yet");
    return oms_status_error;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::addSignalsToResults(const ComRef& cref, const std::string& regex)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::addSignalsToResults] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      return fmiModel->addSignalsToResults(regex);
    }

    logError("[oms2::Scope::addSignalsToResults] is only implemented for FMI models yet");
    return oms_status_error;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::removeSignalsFromResults(const ComRef& cref, const std::string& regex)
{
  if (cref.isIdent())
  {
    // Model
    Model* model = getModel(cref);
    if (!model)
    {
      logError("[oms2::Scope::removeSignalsFromResults] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      return fmiModel->removeSignalsFromResults(regex);
    }

    logError("[oms2::Scope::removeSignalsFromResults] is only implemented for FMI models yet");
    return oms_status_error;
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::setFlags(const ComRef& cref, const std::string& flags)
{
  if (!cref.isIdent())
  {
    // Sub-model
    ComRef modelCref = cref.first();
    Model* model = getModel(modelCref);
    if (!model)
    {
      logError("[oms2::Scope::setFlags] failed");
      return oms_status_error;
    }

    // FMI model?
    if (oms_component_fmi == model->getType())
    {
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      FMISubModel* subModel = fmiModel->getSubModel(cref);
      if (!subModel)
      {
        logError("[oms2::Scope::setFlags] failed");
        return oms_status_error;
      }
      return subModel->setFlags(flags);
    }
    else
    {
      logError("[oms2::Scope::setFlags] is only implemented for FMI models yet");
      return oms_status_error;
    }
  }
  return oms_status_error;
}

oms_status_enu_t oms2::Scope::addSolver(const ComRef& modelCref, const ComRef& name, const std::string& solver)
{
  // Sub-model
  Model* model = getModel(modelCref);
  if (!model)
  {
    logError("[oms2::Scope::addSolver] failed");
    return oms_status_error;
  }

  // FMI model?
  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    if (!fmiModel)
    {
      logError("[oms2::Scope::addSolver] failed");
      return oms_status_error;
    }
    return fmiModel->addSolver(name, solver);
  }
  else
  {
    logError("[oms2::Scope::addSolver] is only implemented for FMI models yet");
    return oms_status_error;
  }
}

oms_status_enu_t oms2::Scope::connectSolver(const ComRef& modelCref, const ComRef& name, const ComRef& fmu)
{
  // Sub-model
  Model* model = getModel(modelCref);
  if (!model)
  {
    logError("[oms2::Scope::connectSolver] failed");
    return oms_status_error;
  }

  // FMI model?
  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    if (!fmiModel)
    {
      logError("[oms2::Scope::connectSolver] failed");
      return oms_status_error;
    }
    return fmiModel->connectSolver(name, fmu);
  }
  else
  {
    logError("[oms2::Scope::connectSolver] is only implemented for FMI models yet");
    return oms_status_error;
  }
}

oms_status_enu_t oms2::Scope::unconnectSolver(const ComRef& modelCref, const ComRef& name, const ComRef& fmu)
{
  // Sub-model
  Model* model = getModel(modelCref);
  if (!model)
  {
    logError("[oms2::Scope::unconnectSolver] failed");
    return oms_status_error;
  }

  // FMI model?
  if (oms_component_fmi == model->getType())
  {
    FMICompositeModel* fmiModel = model->getFMICompositeModel();
    if (!fmiModel)
    {
      logError("[oms2::Scope::unconnectSolver] failed");
      return oms_status_error;
    }
    return fmiModel->unconnectSolver(name, fmu);
  }
  else
  {
    logError("[oms2::Scope::unconnectSolver] is only implemented for FMI models yet");
    return oms_status_error;
  }
}

bool oms2::Scope::exists(const ComRef& cref)
{
  Model* model = getModel(cref.first());
  if (NULL == model)
    return 0;

  if (cref.isIdent())
  {
    // case 1: composite model
    return (NULL != model);
  }
  else
  {
    if (oms_component_fmi == model->getType())
    {
      // case 2a: sub-model (FMI composite model)
      FMICompositeModel* fmiModel = model->getFMICompositeModel();
      if (NULL != fmiModel->getSubModel(cref, false))
        return true;

      // case 3: solver
      return (NULL != fmiModel->getSolver(cref, false));
    }
    else
    {
#if !defined(NO_TLM)
      // case 2b: sub-model (TLM composite model)
      TLMCompositeModel* tlmModel = model->getTLMCompositeModel();
      ComRef tempRef = cref;
      tempRef.popFirst();
      return tlmModel->exists(tempRef);
#endif
      logError("[oms2::Scope::exists] not implemented for TLM composite models");
      return 0;
    }
  }
}
