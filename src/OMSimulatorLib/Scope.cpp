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

#include "Scope.h"
#include "Flags.h"
#include "System.h"
#include "Component.h"
#include "Snapshot.h"
#include "miniunz.h"
#include <time.h>
#include "ssd/Tags.h"
#include <iostream>

oms::Scope::Scope()
  : tempDir("."), workDir(".")
{
  // initialize random seed
  srand(time(NULL));

  this->models.push_back(NULL);

  setTempDirectory(tempDir);
  setWorkingDirectory(workDir);
}

oms::Scope::~Scope()
{
  // free memory if no one else does
  for (const auto& model : models)
    if (model)
      delete model;
}

oms::Scope& oms::Scope::GetInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms::Model* oms::Scope::newModel(const oms::ComRef& cref)
{
  // check if cref is in scope
  if (getModel(cref))
  {
    logError_AlreadyInScope(cref);
    return NULL;
  }

  Model* model = oms::Model::NewModel(cref);
  if (!model)
    return NULL;

  models.back() = model;
  models_map[cref] = models.size() - 1;
  models.push_back(NULL);

  return model;
}

oms_status_enu_t oms::Scope::deleteModel(const oms::ComRef& cref)
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

oms_status_enu_t oms::Scope::renameModel(const oms::ComRef& cref, const oms::ComRef& newCref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  auto it = models_map.find(front);
  if (it == models_map.end())
    return logError("Model \"" + std::string(front) + "\" does not exist in the scope");

  unsigned int index = it->second;
  if (tail.isEmpty()) // renaming the model itself
  {
    oms_status_enu_t status = models[index]->rename(newCref);
    if (oms_status_ok != status)
      return status;

    models_map.erase(it);
    models_map[newCref] = index;

    return oms_status_ok;
  }
  else // renaming a subcomponent
  {
    return models[index]->rename(tail, newCref);
  }
}

oms_status_enu_t oms::Scope::exportModel(const oms::ComRef& cref, const std::string& filename)
{
  oms::Model* model = getModel(cref);
  if (!model)
    return logError("Model \"" + std::string(cref) + "\" does not exist in the scope");

  return model->exportToFile(filename);
}

oms_status_enu_t oms::Scope::miniunz(const std::string& filename, const std::string& extractdir)
{
  // This function is used to extract complete SSP/FMU files:
  //        miniunz -xo <filename> -d <extractdir>

  // Usage: miniunz [-e] [-x] [-v] [-l] [-o] [-p password] file.zip [file_to_extr.] [-d extractdir]
  //        -e  Extract without pathname (junk paths)
  //        -x  Extract with pathname
  //        -v  list files
  //        -l  list files
  //        -d  directory to extract into
  //        -o  overwrite files without prompting
  //        -p  extract crypted file using password

  std::string cd = Scope::GetInstance().getWorkingDirectory();

  int argc = 5;
  char **argv = new char*[argc];
  int i=0;
  argv[i++] = (char*)"miniunz";
  argv[i++] = (char*)"-xo";
  argv[i++] = (char*)filename.c_str();
  argv[i++] = (char*)"-d";
  argv[i++] = (char*)extractdir.c_str();
  int status = ::miniunz(argc, argv);
  delete[] argv;

  // Note: miniunz can change the working directory
  // Restore working directory if it was changed
  std::string cd2 = Scope::GetInstance().getWorkingDirectory();
  if (cd != cd2)
    Scope::GetInstance().setWorkingDirectory(cd);

  if (status != 0)
    return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::Scope::importModel(const std::string& filename, char** _cref)
{
  if (_cref)
    *_cref = NULL;

  std::string extension = "";
  if (filename.length() > 4)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssp")
    return logError("filename extension must be \".ssp\"; no other formats are supported");

  // extract SystemStructure.ssd to memory
  const char* systemStructure = ::miniunz_onefile_to_memory(filename.c_str(), "SystemStructure.ssd");
  if (!systemStructure)
    return logError("failed to extract \"SystemStructure.ssd\" from \"" + std::string(filename) + "\"");

  Snapshot snapshot;
  oms_status_enu_t status = snapshot.importResourceMemory("SystemStructure.ssd", systemStructure);
  ::miniunz_free(systemStructure);
  if (oms_status_ok != status)
    return logError("Failed to import");

  const pugi::xml_node node = snapshot.getResourceNode("SystemStructure.ssd");
  if (!node)
    return logError("failed to load \"SystemStructure.ssd\"");

  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string ssdVersion = node.attribute("version").as_string();

  Model* model = newModel(cref);
  if (!model)
    return oms_status_error;

  if (ssdVersion != "Draft20180219" && ssdVersion != "1.0" && ssdVersion != "2.0")
    logWarning("Unknown SSD version: " + ssdVersion);

  // extract the ssp file
  oms::Scope::miniunz(filename, model->getTempDirectory());

  std::string cd = Scope::GetInstance().getWorkingDirectory();
  Scope::GetInstance().setWorkingDirectory(model->getTempDirectory());

  bool old_copyResources = model->copyResources();
  model->copyResources(false);

  // add the remaining resources (e.g) .ssv, .ssm and signalFilter.xml to oms:snapshot
  for (const auto &entry : OMS_RECURSIVE_DIRECTORY_ITERATOR(model->getTempDirectory()))
    if (entry.path().has_extension())
    {
      // TODO collect all the files and sort the ssd variant, because the iteration order is different for windows and linux
      // check for ssd variants
      if (".ssd" == entry.path().extension())
      {
        if (entry.path().filename() != "SystemStructure.ssd")
        {
          //std::cout << "\n Debug print : " << entry.path().filename().generic_string();
          Snapshot variants;
          variants.importResourceFile(naive_uncomplete(entry.path(), model->getTempDirectory()), model->getTempDirectory());
          char* variantSnapshot = NULL;
          variants.writeDocument(&variantSnapshot);
          {
            auto it = model->ssdVariants.find(variants.getRootCref());
            if (it != model->ssdVariants.end())
            {
              free(model->ssdVariants[variants.getRootCref()]);
            }
          }
          model->ssdVariants[variants.getRootCref()] = variantSnapshot;
        }
      }

      if (".ssv" == entry.path().extension() || ".ssm" == entry.path().extension() || ".xml" == entry.path().extension())
      {
        model->importedResources.push_back(entry.path().filename().generic_string());
        snapshot.importResourceFile(naive_uncomplete(entry.path(), model->getTempDirectory()), model->getTempDirectory());
      }
    }
  // snapshot.debugPrintAll();

  status = model->importFromSnapshot(snapshot);
  model->copyResources(old_copyResources);

  Scope::GetInstance().setWorkingDirectory(cd);

  if (oms_status_ok != status)
  {
    deleteModel(cref);
    return oms_status_error;
  }

  if (_cref)
    *_cref = (char*)model->getCref().c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::Scope::setTempDirectory(const std::string& newTempDir)
{
  try
  {
    std::string newTempDirWorkaround = newTempDir;
#if defined(__MINGW32__)
    // create_directory() behaves different with clang 11 than gcc for path with trailing "/"
    if('/' == newTempDirWorkaround.back()) {
      newTempDirWorkaround.pop_back();
    }
#endif
    if (!filesystem::is_directory(newTempDirWorkaround))
    {
      if (!filesystem::create_directory(newTempDirWorkaround))
        return logError("Changing temp directory to \"" + newTempDirWorkaround + "\" failed");
      else if (!Flags::SuppressPath())
        logInfo("New temp directory has been created: \"" + newTempDir + "\"");
    }

    filesystem::path path(newTempDirWorkaround.c_str());
    path = oms_canonical(path);

    this->tempDir = path.string();

    if (!Flags::SuppressPath())
      logInfo("Set temp directory to    \"" + this->tempDir + "\"");

    return oms_status_ok;
  }
  catch (const std::exception& e)
  {
    return logError("failed to set temp directory to \"" + newTempDir + "\": " + e.what());
  }
}

oms_status_enu_t oms::Scope::setWorkingDirectory(const std::string& newWorkingDir)
{
  try
  {
    std::string newWorkingDirWorkaround = newWorkingDir;
#if defined(__MINGW32__)
    // create_directory() behaves different with clang 11 than gcc for path with trailing "/"
    if('/' == newWorkingDirWorkaround.back()) {
      newWorkingDirWorkaround.pop_back();
    }
#endif
    if (!filesystem::is_directory(newWorkingDirWorkaround))
      return logError("Set working directory to \"" + newWorkingDirWorkaround + "\" failed");

    filesystem::path path(newWorkingDirWorkaround.c_str());
    path = oms_canonical(path);

    filesystem::current_path(path);
    workDir = path.string();

    if (!Flags::SuppressPath())
      logInfo("Set working directory to \"" + workDir + "\"");

    return oms_status_ok;
  }
  catch (const std::exception& e)
  {
    return logError("failed to set working directory to \"" + newWorkingDir + "\": " + e.what());
  }
}

const std::string& oms::Scope::getWorkingDirectory()
{
  try
  {
    workDir = filesystem::current_path().string();
    return workDir;
  }
  catch (const std::exception& e)
  {
    logError("failed to get working directory: " + std::string(e.what()));
    workDir = "";
    return workDir;
  }
}

oms_status_enu_t oms::Scope::getElement(const oms::ComRef& cref, oms::Element** element)
{
  if (!element)
    return logWarning("[oms::Scope::getElement] NULL pointer");

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  oms::Model* model = getModel(front);
  if (!model)
    return logError("Model \"" + std::string(front) + "\" does not exist in the scope");

  if (cref.isValidIdent())
    return logError("A model has no element information");

  oms::System* system = model->getSystem(tail);
  oms::Component* component = model->getComponent(tail);
  if (!system && !component)
    return logError("Model \"" + std::string(front) + "\" does not contain system or component \"" + std::string(tail) + "\"");

  if (system)
    *element = system->getElement();
  else
    *element = component->getElement();

  return oms_status_ok;
}

oms_status_enu_t oms::Scope::getElements(const oms::ComRef& cref, oms::Element*** elements)
{
  if (!elements)
    return logWarning("[oms::Scope::getElements] NULL pointer");

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();
  oms::Model* model = getModel(front);
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

oms::Model* oms::Scope::getModel(const oms::ComRef& cref)
{
  auto it = models_map.find(cref);
  if (it == models_map.end())
    return NULL;

  return models[it->second];
}

oms_status_enu_t oms::Scope::loadSnapshot(const oms::ComRef& cref, const char* snapshot, char** newCref)
{
  if (newCref)
    *newCref = NULL;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load(snapshot);
  if (!result)
    return logError("loading snapshot failed (" + std::string(result.description()) + ")");

  const pugi::xml_node node = doc.document_element(); // ssd:SystemStructureDescription

  ComRef name(node.attribute("name").as_string());
  if (name != cref && getModel(name))
    return logError("failed to load snapshot, because it would change the model's name but it already exists in the scope");

  std::string ssdVersion = node.attribute("version").as_string();
  if (ssdVersion != "Draft20180219" && ssdVersion != "1.0" && ssdVersion != "2.0")
    return logError("Unknown SSD version \"" + ssdVersion + "\"; supported version are \"1.0\" and \"2.0\" and\"Draft20180219\".");

  oms_status_enu_t status = getModel(cref)->loadSnapshot(node);

  ComRef new_name(cref);
  if (oms_status_ok == status && name != cref)
    if (oms_status_ok == renameModel(cref, name))
      new_name = name;

  if (newCref)
    *newCref = (char*)getModel(new_name)->getCref().c_str();

  return status;
}

// TODO: renaming not yet supported
oms_status_enu_t oms::Scope::importSnapshot(const oms::ComRef& cref, const char* snapshot, char** newCref)
{
  if (newCref)
    *newCref = NULL;

  oms::Model* model = oms::Scope::GetInstance().getModel(cref);
  if (!model)
    return logError_ModelNotInScope(cref);

  oms_status_enu_t status = model->importSnapshot(snapshot, newCref);

  return status;
}
