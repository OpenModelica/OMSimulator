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
#include "Component.h"
#include <miniunz.h>
#include <OMSBoost.h>
#include <time.h>

oms3::Scope::Scope()
  : tempDir(".")
{
  // initialize random seed
  srand(time(NULL));

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
    return logError_AlreadyInScope(cref);

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
    models_map[models[it->second]->getCref()] = it->second;
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
  argv[i++] = (char*)"miniunz";
  argv[i++] = (char*)"-xo";
  argv[i++] = (char*)filename.c_str();
  if (systemStructure)
    argv[i++] = (char*)"SystemStructure.ssd";
  argv[i++] = (char*)"-d";
  argv[i++] = (char*)extractdir.c_str();
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
  if (filename.length() > 4)
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

  *_cref = (char*)model->getCref().c_str();

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

  if (!Flags::SuppressPath())
    logInfo("Set temp directory to    \"" + this->tempDir + "\"");

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::setWorkingDirectory(const std::string& newWorkingDir)
{
  boost::filesystem::path path(newWorkingDir.c_str());
  if (!boost::filesystem::is_directory(path))
    return logError("Set working directory to \"" + newWorkingDir + "\" failed");

  boost::filesystem::current_path(path);

  if (!Flags::SuppressPath())
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
    return logWarning("[oms3::Scope::getElement] NULL pointer");

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  oms3::Model* model = getModel(front);
  if (!model)
    return logError("Model \"" + std::string(front) + "\" does not exist in the scope");

  if (cref.isValidIdent())
    return logError("A model has no element information");

  oms3::System* system = model->getSystem(tail);
  oms3::Component* component = model->getComponent(tail);
  if (!system && !component)
    return logError("Model \"" + std::string(front) + "\" does not contain system or component \"" + std::string(tail) + "\"");

  if (system)
    *element = system->getElement();
  else
    *element = component->getElement();

  return oms_status_ok;
}

oms_status_enu_t oms3::Scope::getElements(const oms3::ComRef& cref, oms3::Element*** elements)
{
  if (!elements)
    return logWarning("[oms3::Scope::getElements] NULL pointer");

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
