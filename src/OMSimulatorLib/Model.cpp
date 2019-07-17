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

#include "Model.h"

#include "CSVWriter.h"
#include "Flags.h"
#include "MATWriter.h"
#include "Scope.h"
#include "ssd/Tags.h"
#include "System.h"
#include "Component.h"

#include <OMSFileSystem.h>
#include <minizip.h>
#include <thread>

oms::Model::Model(const oms::ComRef& cref, const std::string& tempDir)
  : cref(cref), tempDir(tempDir), resultFilename(std::string(cref) + "_res.mat")
{
  if (!Flags::SuppressPath())
    logInfo("New model \"" + std::string(cref) + "\" with corresponding temp directory \"" + tempDir + "\"");

  elements.push_back(NULL);
  elements.push_back(NULL);
}

oms::Model::~Model()
{
  terminate();

  if (system)
    delete system;

  // delete temp directory
  if (Flags::DeleteTempFiles())
  {
    if (!tempDir.empty() && filesystem::is_directory(tempDir))
    {
      try
      {
        filesystem::remove_all(tempDir);
        logDebug("removed temp directory: \"" + tempDir + "\"");
      }
      catch (const std::exception& e)
      {
        logWarning("temp directory \"" + tempDir + "\" couldn't be removed\n" + e.what());
      }
    }
  }
}

oms::Model* oms::Model::NewModel(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  filesystem::path tempDir = (filesystem::path(Scope::GetInstance().getTempDirectory().c_str()) / oms_unique_path(std::string(cref))).string();
  if (filesystem::is_directory(tempDir))
  {
    logError("Unique temp directory does already exist. Clean up the temp directory \"" + Scope::GetInstance().getTempDirectory() + "\" and try again.");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir))
  {
    logError("Failed to create unique temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir / "temp"))
  {
    logError("Failed to create temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!filesystem::create_directory(tempDir / "resources"))
  {
    logError("Failed to create resources directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }

  oms::Model* model = new oms::Model(cref, tempDir.string());
  return model;
}

oms_status_enu_t oms::Model::rename(const oms::ComRef& cref)
{
  if (!cref.isValidIdent())
    return logError(std::string(cref) + " is not a valid ident");

  this->cref = cref;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::loadSnapshot(const char* snapshot)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load(snapshot);
  if (!result)
    return logError("loading snapshot failed (" + std::string(result.description()) + ")");

  const pugi::xml_node node = doc.document_element(); // ssd:SystemStructureDescription

  ComRef new_cref = ComRef(node.attribute("name").as_string());
  std::string ssdVersion = node.attribute("version").as_string();

  if (new_cref != getCref())
    return logError("this API cannot be used to rename a model");

  if (ssdVersion != "Draft20180219")
    logWarning("Unknown SSD version: " + ssdVersion);

  System* old_root_system = system;
  system = NULL;

  bool old_copyResources = copyResources();
  copyResources(false);
  oms_status_enu_t status = importFromSSD(node);
  copyResources(old_copyResources);

  if (oms_status_ok != status)
  {
    system = old_root_system;
    return logError("loading snapshot failed");
  }

  if (old_root_system)
  {
    delete old_root_system;
    old_root_system = NULL;
  }

  return oms_status_ok;
}

oms::System* oms::Model::getSystem(const oms::ComRef& cref)
{
  if (!system)
    return NULL;

  if (cref.isEmpty())
    return system;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
    return system->getSystem(tail);

  return NULL;
}

oms::Component* oms::Model::getComponent(const oms::ComRef& cref)
{
  if (!system)
    return NULL;

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
    return system->getComponent(tail);

  return NULL;
}

oms_status_enu_t oms::Model::delete_(const oms::ComRef& cref)
{
  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (!system || front != system->getCref())
    return oms_status_error;

  if (tail.isEmpty())
  {
    delete system;
    system = NULL;
    return oms_status_ok;
  }
  else
    return system->delete_(tail);
}

oms_status_enu_t oms::Model::list(const oms::ComRef& cref, char** contents)
{
  struct xmlStringWriter : pugi::xml_writer
  {
    std::string result;
    virtual void write(const void* data, size_t size)
    {
      result += std::string(static_cast<const char*>(data), size);
    }
  };

  xmlStringWriter writer;
  pugi::xml_document doc;

  // list model
  if (cref.isEmpty())
  {
    pugi::xml_node node = doc.append_child(oms::ssd::ssd_system_structure_description);
    exportToSSD(node);
  }
  else
  {
    // list system
    if (!system)
      return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");

    System* subsystem = getSystem(cref);
    if (subsystem)
    {
      pugi::xml_node node = doc.append_child(oms::ssd::ssd_system);
      subsystem->exportToSSD(node);
    }
    else
    {
      // list component
      Component* component = getComponent(cref);
      if (!component)
        return logError("error");

      pugi::xml_node node = doc.append_child(oms::ssd::ssd_system);
      component->exportToSSD(node);
    }
  }

  doc.save(writer);
  *contents = (char*) malloc(strlen(writer.result.c_str()) + 1);
  strcpy(*contents, writer.result.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms::Model::addSystem(const oms::ComRef& cref, oms_system_enu_t type)
{
  if (cref.isValidIdent() && !system)
  {
    system = System::NewSystem(cref, type, this, NULL);
    if (system)
    {
      elements[0] = system->getElement();
      return oms_status_ok;
    }
    return oms_status_error;
  }

  if (!system)
    return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");

  oms::ComRef tail(cref);
  oms::ComRef front = tail.pop_front();

  if (system->getCref() == front)
    return system->addSubSystem(tail, type);

  return logError("wrong input \"" + std::string(front) + "\" != \"" + std::string(system->getCref()) + "\"");
}

oms_status_enu_t oms::Model::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("version") = "Draft20180219";

  if (system)
  {
    pugi::xml_node system_node = node.append_child(oms::ssd::ssd_system);
    if (oms_status_ok != system->exportToSSD(system_node))
      return logError("export of system failed");
  }

  pugi::xml_node default_experiment = node.append_child(oms::ssd::ssd_default_experiment);
  default_experiment.append_attribute("startTime") = std::to_string(startTime).c_str();
  default_experiment.append_attribute("stopTime") = std::to_string(stopTime).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::Model::importFromSSD(const pugi::xml_node& node)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms::ssd::ssd_system)
    {
      ComRef systemCref = ComRef(it->attribute("name").as_string());

      // lochel: I guess that can somehow be improved
      oms_system_enu_t systemType = oms_system_tlm;
      if (std::string(it->child(oms::ssd::ssd_simulation_information).child("VariableStepSolver").attribute("description").as_string()) != "")
        systemType = oms_system_sc;
      if (std::string(it->child(oms::ssd::ssd_simulation_information).child("FixedStepSolver").attribute("description").as_string()) != "")
        systemType = oms_system_sc;
      if (std::string(it->child(oms::ssd::ssd_simulation_information).child("VariableStepMaster").attribute("description").as_string()) != "")
        systemType = oms_system_wc;
      if (std::string(it->child(oms::ssd::ssd_simulation_information).child("FixedStepMaster").attribute("description").as_string()) != "")
        systemType = oms_system_wc;

      if (oms_status_ok != addSystem(systemCref, systemType))
        return oms_status_error;

      System* system = getSystem(systemCref);
      if (!system)
        return oms_status_error;

      if (oms_status_ok != system->importFromSSD(*it))
        return oms_status_error;
    }
    else if (name == oms::ssd::ssd_default_experiment)
    {
      startTime = it->attribute("startTime").as_double(0.0);
      stopTime = it->attribute("stopTime").as_double(1.0);
    }
    else
      return logError("wrong xml schema detected");
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::exportToFile(const std::string& filename) const
{
  pugi::xml_document doc;

  std::string extension = "";
  if (filename.length() > 4)
    extension = filename.substr(filename.length() - 4);

  if (extension != ".ssp")
    return logError("filename extension must be \".ssp\"; no other formats are supported");

  // generate XML declaration
  pugi::xml_node declarationNode = doc.append_child(pugi::node_declaration);
  declarationNode.append_attribute("version") = "1.0";
  declarationNode.append_attribute("encoding") = "UTF-8";

  pugi::xml_node node = doc.append_child(oms::ssd::ssd_system_structure_description);
  exportToSSD(node);

  filesystem::path ssdPath = filesystem::path(tempDir) / "SystemStructure.ssd";
  if (!doc.save_file(ssdPath.string().c_str()))
    return logError("failed to export \"" + ssdPath.string() + "\" (for model \"" + std::string(this->getCref()) + "\")");

  // Usage: minizip [-o] [-a] [-0 to -9] [-p password] [-j] file.zip [files_to_add]
  //        -o  Overwrite existing file.zip
  //        -a  Append to existing file.zip
  //        -0  Store only
  //        -1  Compress faster
  //        -9  Compress better
  //        -j  exclude path. store only the file name
  std::vector<std::string> resources;
  if (oms_status_ok != getAllResources(resources))
    return logError("failed to gather all resources");

  std::string cd = Scope::GetInstance().getWorkingDirectory();
  Scope::GetInstance().setWorkingDirectory(tempDir);
  int argc = 4 + resources.size();
  char **argv = new char*[argc];
  int i=0;
  argv[i++] = (char*)"minizip";
  argv[i++] = (char*)"-o";
  argv[i++] = (char*)"-1";
  argv[i++] = (char*)"temp/model.ssp";
  for (const auto& file : resources)
    argv[i++]=(char*)file.c_str();
  minizip(argc, argv);
  delete[] argv;
  Scope::GetInstance().setWorkingDirectory(cd);

  filesystem::path full_path = filesystem::path(tempDir) / "temp/model.ssp";
  oms_copy_file(full_path, filesystem::path(filename));

  return oms_status_ok;
}

oms_status_enu_t oms::Model::getAllResources(std::vector<std::string>& resources) const
{
  resources.push_back("SystemStructure.ssd");
  if (system)
    return system->getAllResources(resources);
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setStartTime(double value)
{
  if (!validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated))
    return logError_ModelInWrongState(this);

  startTime = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setStopTime(double value)
{
  if (!validState(oms_modelState_virgin|oms_modelState_enterInstantiation|oms_modelState_instantiated))
    return logError_ModelInWrongState(this);

  stopTime = value;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::instantiate()
{
  if (!validState(oms_modelState_virgin|oms_modelState_enterInstantiation))
    return logError_ModelInWrongState(this);

  if (!system)
    return logError("Model doesn't contain a system");

  // initialize thread pool
  int numThreads = Flags::NumProcs();
  if (numThreads != 1)
  {
    if (numThreads > std::thread::hardware_concurrency() || 0 == numThreads)
      numThreads = std::thread::hardware_concurrency();
    pool = new ctpl::thread_pool(numThreads);
    logInfo("Initialized thread pool for model " + std::string(getCref()) + " with " + std::to_string(numThreads) + " threads");
  }

  modelState = oms_modelState_enterInstantiation;
  if (oms_status_ok != system->instantiate())
  {
    terminate();
    return oms_status_error;
  }
  modelState = oms_modelState_instantiated;

  return oms_status_ok;
}

oms_status_enu_t oms::Model::initialize()
{
  if (!validState(oms_modelState_instantiated))
    return logError_ModelInWrongState(this);

  if (!system)
    return logError("Model doesn't contain a system");

  modelState = oms_modelState_initialization;
  clock.reset();
  clock.tic();

  cancelSim = false;
  lastEmit = startTime;

  if (!resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 4)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(bufferSize);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(bufferSize);
    else
    {
      modelState = oms_modelState_instantiated;
      clock.toc();
      return logError("Unsupported format of the result file: " + resultFilename);
    }
  }

  if (oms_status_ok != system->initialize())
  {
    terminate();
    clock.toc();
    return logError_Initialization(system->getFullCref());
  }

  if (resultFile)
  {
    logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

    // add all signals
    if (oms_status_ok != registerSignalsForResultFile())
    {
      terminate();
      clock.toc();
      return logError_Initialization(system->getFullCref());
    }

    // create result file
    if (!resultFile->create(resultFilename, startTime, stopTime))
    {
      delete resultFile;
      resultFile = NULL;
      logError("Creating result file failed");
      terminate();
      clock.toc();
      return logError_Initialization(system->getFullCref());
    }

    // dump results
    emit(startTime, true);
  }
  else
    logInfo("No result file will be created");

  clock.toc();
  modelState = oms_modelState_simulation;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::simulate_asynchronous(void (*cb)(const char* cref, double time, oms_status_enu_t status))
{
  if (!validState(oms_modelState_simulation))
    return logError_ModelInWrongState(this);

  if (!system)
    return logError("Model doesn't contain a system");

  std::thread([=]{system->stepUntil(stopTime, cb);}).detach();
  return oms_status_pending;
}

oms_status_enu_t oms::Model::simulate()
{
  clock.tic();
  if (!validState(oms_modelState_simulation))
  {
    clock.toc();
    return logError_ModelInWrongState(this);
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  oms_status_enu_t status = system->stepUntil(stopTime, NULL);
  emit(stopTime, true);
  clock.toc();
  return status;
}

oms_status_enu_t oms::Model::stepUntil(double stopTime)
{
  clock.tic();
  if (!validState(oms_modelState_simulation))
  {
    clock.toc();
    return logError_ModelInWrongState(this);
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  oms_status_enu_t status = system->stepUntil(stopTime, NULL);
  emit(stopTime, true);
  clock.toc();
  return status;
}

oms_status_enu_t oms::Model::terminate()
{
  if (validState(oms_modelState_virgin))
    return oms_status_ok;

  if (validState(oms_modelState_instantiated) && oms_status_ok != system->initialize())
    return logError_Termination(system->getFullCref());

  if (!system)
    return logError("Model doesn't contain a system");

  if (oms_status_ok != system->terminate())
    return logError_Termination(system->getFullCref());

  if (useThreadPool())
  {
    delete pool;
    pool = nullptr;
  }

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_virgin;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::reset()
{
  if (!validState(oms_modelState_simulation))
    return logError_ModelInWrongState(this);

  if (!system)
    return logError("Model doesn't contain a system");

  if (oms_status_ok != system->reset())
    return logError_ResetFailed(system->getFullCref());

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_instantiated;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setLoggingInterval(double loggingInterval)
{
  if (loggingInterval < 0.0)
    this->loggingInterval = 0.0;
  this->loggingInterval = loggingInterval;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::registerSignalsForResultFile()
{
  if (!resultFile)
    return oms_status_ok;

  if (system)
    if (oms_status_ok != system->registerSignalsForResultFile(*resultFile))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::emit(double time, bool force, bool* emitted)
{
  if (emitted)
    *emitted = false;

  if (!resultFile)
    return oms_status_ok;
  if (!force && time < lastEmit + loggingInterval)
    return oms_status_ok;
  if (!force && time <= lastEmit)
    return oms_status_ok;

  if (system)
    if (oms_status_ok != system->updateSignals(*resultFile))
      return oms_status_error;

  resultFile->emit(time);
  lastEmit = time;
  if (emitted)
    *emitted = true;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::setResultFile(const std::string& filename, int bufferSize)
{
  this->resultFilename = filename;
  this->bufferSize = bufferSize;

  bool createNewResultfile = (resultFile != NULL);

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  if (createNewResultfile && !resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 4)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(bufferSize);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(bufferSize);
    else
      return logError("Unsupported format of the result file: " + resultFilename);

    logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

    // add all signals
    registerSignalsForResultFile();

    // create result file
    if (!resultFile->create(resultFilename, startTime, stopTime))
    {
      delete resultFile;
      resultFile = NULL;
      return logError("Creating result file failed");
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::Model::addSignalsToResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->addSignalsToResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::removeSignalsFromResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->removeSignalsFromResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::Model::cancelSimulation_asynchronous()
{
  if (!validState(oms_modelState_simulation))
    return logError_ModelInWrongState(this);

  cancelSim = true;
  return oms_status_ok;
}
