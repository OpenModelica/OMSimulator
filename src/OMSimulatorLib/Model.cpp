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

#include "Model.h"

#include "CSVWriter.h"
#include "Flags.h"
#include "MATWriter.h"
#include "Scope.h"
#include "ssd/Tags.h"
#include "System.h"
#include "Component.h"

#include <OMSBoost.h>
#include <minizip.h>

/* ************************************ */
/* oms3                                 */
/*                                      */
/* ************************************ */

oms3::Model::Model(const oms3::ComRef& cref, const std::string& tempDir)
  : cref(cref), tempDir(tempDir), resultFilename(std::string(cref) + "_res.mat")
{
  if (!Flags::SuppressPath())
    logInfo("New model \"" + std::string(cref) + "\" with corresponding temp directory \"" + tempDir + "\"");

  elements.push_back(NULL);
  elements.push_back(NULL);
}

oms3::Model::~Model()
{
  if (oms_modelState_terminated != modelState)
    terminate();

  if (system)
    delete system;
}

oms3::Model* oms3::Model::NewModel(const oms3::ComRef& cref)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  boost::filesystem::path tempDir = (boost::filesystem::path(Scope::GetInstance().getTempDirectory().c_str()) / oms_unique_path(std::string(cref))).string();
  if (boost::filesystem::is_directory(tempDir))
  {
    logError("Unique temp directory does already exist. Clean up the temp directory \"" + Scope::GetInstance().getTempDirectory() + "\" and try again.");
    return NULL;
  }
  if (!boost::filesystem::create_directory(tempDir))
  {
    logError("Failed to create unique temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!boost::filesystem::create_directory(tempDir / "temp"))
  {
    logError("Failed to create temp directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }
  if (!boost::filesystem::create_directory(tempDir / "resources"))
  {
    logError("Failed to create resources directory for the model \"" + std::string(cref) + "\"");
    return NULL;
  }

  oms3::Model* model = new oms3::Model(cref, tempDir.string());
  return model;
}

oms_status_enu_t oms3::Model::rename(const oms3::ComRef& cref)
{
  if (!cref.isValidIdent())
    return logError(std::string(cref) + " is not a valid ident");

  this->cref = cref;
  return oms_status_ok;
}

oms3::System* oms3::Model::getSystem(const oms3::ComRef& cref)
{
  if (!system)
    return NULL;

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
  {
    if (tail.isEmpty())
      return system;
    else
      return system->getSystem(tail);
  }

  return NULL;
}

oms3::Component* oms3::Model::getComponent(const oms3::ComRef& cref)
{
  if (!system)
    return NULL;

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  if (this->system->getCref() == front)
    return system->getComponent(tail);

  return NULL;
}

oms_status_enu_t oms3::Model::delete_(const oms3::ComRef& cref)
{
  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

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

oms_status_enu_t oms3::Model::list(const oms3::ComRef& cref, char** contents)
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
    pugi::xml_node node = doc.append_child(oms2::ssd::ssd_system_structure_description);
    exportToSSD(node);
  }
  else
  {
    // list system
    if (!system)
      return logError("Model \"" + std::string(getCref()) + "\" does not contain any system");

    System* subsystem = getSystem(cref);
    if (!subsystem)
      return logError("error");

    pugi::xml_node node = doc.append_child(oms2::ssd::ssd_system);
    subsystem->exportToSSD(node);
  }

  doc.save(writer);
  *contents = (char*) malloc(strlen(writer.result.c_str()) + 1);
  strcpy(*contents, writer.result.c_str());
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::addSystem(const oms3::ComRef& cref, oms_system_enu_t type)
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

  oms3::ComRef tail(cref);
  oms3::ComRef front = tail.pop_front();

  if (system->getCref() == front)
    return system->addSubSystem(tail, type);

  return logError("wrong input \"" + std::string(front) + "\" != \"" + std::string(system->getCref()) + "\"");
}

oms_status_enu_t oms3::Model::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("version") = "Draft20180219";

  if (system)
  {
    pugi::xml_node system_node = node.append_child(oms2::ssd::ssd_system);
    if (oms_status_ok != system->exportToSSD(system_node))
      return logError("export of system failed");
  }

  pugi::xml_node default_experiment = node.append_child(oms2::ssd::ssd_default_experiment);
  default_experiment.append_attribute("startTime") = std::to_string(startTime).c_str();
  default_experiment.append_attribute("stopTime") = std::to_string(stopTime).c_str();

  return oms_status_ok;
}

oms_status_enu_t oms3::Model::importFromSSD(const pugi::xml_node& node)
{
  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if (name == oms2::ssd::ssd_system)
    {
      ComRef systemCref = ComRef(it->attribute("name").as_string());

      // lochel: I guess that can somehow be improved
      oms_system_enu_t systemType = oms_system_tlm;
      if (it->child(oms2::ssd::ssd_simulation_information).child("VariableStepSolver").attribute("description").as_string() != "")
        systemType = oms_system_sc;
      if (it->child(oms2::ssd::ssd_simulation_information).child("FixedStepSolver").attribute("description").as_string() != "")
        systemType = oms_system_sc;
      if (it->child(oms2::ssd::ssd_simulation_information).child("VariableStepMaster").attribute("description").as_string() != "")
        systemType = oms_system_wc;
      if (it->child(oms2::ssd::ssd_simulation_information).child("FixedStepMaster").attribute("description").as_string() != "")
        systemType = oms_system_wc;

      if (oms_status_ok != addSystem(systemCref, systemType))
        return oms_status_error;

      System* system = getSystem(systemCref);
      if (!system)
        return oms_status_error;

      if (oms_status_ok != system->importFromSSD(*it))
        return oms_status_error;
    }
    else if (name == oms2::ssd::ssd_default_experiment)
    {
      startTime = it->attribute("startTime").as_double(0.0);
      stopTime = it->attribute("stopTime").as_double(1.0);
    }
    else
      return logError("wrong xml schema detected");
  }

  return oms_status_ok;
}

oms_status_enu_t oms3::Model::exportToFile(const std::string& filename) const
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

  pugi::xml_node node = doc.append_child(oms2::ssd::ssd_system_structure_description);
  exportToSSD(node);

  boost::filesystem::path ssdPath = boost::filesystem::path(tempDir) / "SystemStructure.ssd";
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

  boost::filesystem::path full_path = boost::filesystem::path(tempDir) / "temp/model.ssp";
  boost::filesystem::copy_file(full_path, boost::filesystem::path(filename), boost::filesystem::copy_option::overwrite_if_exists);

  return oms_status_ok;
}

oms_status_enu_t oms3::Model::getAllResources(std::vector<std::string>& resources) const
{
  resources.push_back("SystemStructure.ssd");
  if (system)
    return system->getAllResources(resources);
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::setStartTime(double value)
{
  if (oms_modelState_terminated != modelState)
    return logError_ModelInWrongState(this);

  startTime = value;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::setStopTime(double value)
{
  if (oms_modelState_terminated != modelState)
    return logError_ModelInWrongState(this);

  stopTime = value;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::instantiate()
{
  if (oms_modelState_terminated != modelState)
    return logError_ModelInWrongState(this);

  if (!system)
    return logError("Model doesn't contain a system");

  modelState = oms_modelState_initialization;
  if (oms_status_ok != system->instantiate())
  {
    terminate();
    return oms_status_error;
  }

  modelState = oms_modelState_instantiated;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::initialize()
{
  if (oms_modelState_instantiated != modelState)
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
      clock.toc();
      return logError("Unsupported format of the result file: " + resultFilename);
    }
  }
  else
    logInfo("No result file will be created");

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

  clock.toc();
  modelState = oms_modelState_simulation;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::simulate_asynchronous(void (*cb)(const char* cref, double time, oms_status_enu_t status))
{
  clock.tic();
  if (oms_modelState_simulation != modelState)
  {
    clock.toc();
    return logError_ModelInWrongState(this);
  }

  if (!system)
  {
    clock.toc();
    return logError("Model doesn't contain a system");
  }

  std::thread([=]{system->stepUntil(stopTime, cb);}).detach();
  emit(stopTime, true);
  clock.toc();

  return oms_status_ok;
}

oms_status_enu_t oms3::Model::simulate()
{
  clock.tic();
  if (oms_modelState_simulation != modelState)
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

oms_status_enu_t oms3::Model::stepUntil(double stopTime)
{
  clock.tic();
  if (oms_modelState_simulation != modelState)
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

oms_status_enu_t oms3::Model::terminate()
{
  if (oms_modelState_terminated == modelState)
    return oms_status_ok;

  if (oms_modelState_instantiated == modelState && oms_status_ok != system->initialize())
    return logError_Termination(system->getFullCref());

  if (!system)
    return logError("Model doesn't contain a system");

  if (oms_status_ok != system->terminate())
    return logError_Termination(system->getFullCref());

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  modelState = oms_modelState_terminated;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::reset()
{
  if (oms_modelState_simulation != modelState)
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

oms_status_enu_t oms3::Model::setLoggingInterval(double loggingInterval)
{
  if (loggingInterval < 0.0)
    this->loggingInterval = 0.0;
  this->loggingInterval = loggingInterval;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::registerSignalsForResultFile()
{
  if (!resultFile)
    return oms_status_ok;

  clock_id = resultFile->addSignal("wallTime", "wall-clock time [s]", SignalType_REAL);
  if (system)
    if (oms_status_ok != system->registerSignalsForResultFile(*resultFile))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::emit(double time, bool force)
{
  if (!resultFile)
    return oms_status_ok;
  if (!force && time < lastEmit + loggingInterval)
    return oms_status_ok;

  SignalValue_t wallTime;
  wallTime.realValue = clock.getElapsedWallTime();
  resultFile->updateSignal(clock_id, wallTime);
  if (system)
    if (oms_status_ok != system->updateSignals(*resultFile))
      return oms_status_error;

  resultFile->emit(time);
  lastEmit = time;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::setResultFile(const std::string& filename, int bufferSize)
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

oms_status_enu_t oms3::Model::addSignalsToResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->addSignalsToResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::removeSignalsFromResults(const char* regex)
{
  if (system)
    if (oms_status_ok != system->removeSignalsFromResults(regex))
      return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms3::Model::cancelSimulation_asynchronous()
{
  if (oms_modelState_simulation != modelState)
    return logError_ModelInWrongState(this);

  cancelSim = true;
  return oms_status_ok;
}

/* ************************************ */
/* oms2                                 */
/*                                      */
/*                                      */
/* ************************************ */

#include "CSVWriter.h"
#include "FMICompositeModel.h"
#include "Logging.h"
#include "MATWriter.h"
#include "Scope.h"
#include "ssd/Tags.h"
#include "TLMCompositeModel.h"
#include "StepSizeConfiguration.h"

#include <regex>
#include <thread>

#include <pugixml.hpp>
#include <boost/filesystem.hpp>

oms2::Model::Model(const oms2::ComRef& cref)
  : systemGeometry(), resultFilename(cref.toString() + "_res.mat"), resultFile(NULL), stepSizeConfiguration()
{
  logTrace();
  modelState = oms_modelState_instantiated;
}

oms2::Model::~Model()
{
  if (compositeModel)
  {
    reset(true);
    CompositeModel::DeleteModel(compositeModel);
  }
}

oms2::Model* oms2::Model::NewModel(oms_element_type_enu_t type, const oms2::ComRef& cref)
{
  oms2::Model* model = new oms2::Model(cref);

  if (oms_component_fmi == type)
    model->compositeModel = oms2::FMICompositeModel::NewModel(cref);
  else if (oms_component_tlm == type)
#if !defined(NO_TLM)
    model->compositeModel = oms2::TLMCompositeModel::NewModel(cref);
#else
    THROW_NO_TLM();
#endif
  if (!model->compositeModel)
  {
    delete model;
    model = NULL;
  }

  return model;
}

oms_status_enu_t oms2::Model::ParseString(const std::string& contents, char** ident)
{
  oms_element_type_enu_t modelType = oms_component_none_old;
  bool defaultExperiment = false;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_buffer(contents.c_str(), strlen(contents.c_str()));
  if (!result)
  {
    logError("ParseString failed (" + std::string(result.description()) + ")");
    return oms_status_error;
  }

  const pugi::xml_node root = doc.document_element();

  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if (name == "TLMModel" && modelType == oms_component_none_old)
      modelType = oms_component_tlm;
    else if (name == oms2::ssd::ssd_system && modelType == oms_component_none_old)
      modelType = oms_component_fmi;
    else if (name == oms2::ssd::ssd_default_experiment && !defaultExperiment)
      defaultExperiment = true;
    else
    {
      logError("wrong xml schema detected");
      return oms_status_error;
    }
  }

  if (modelType == oms_component_fmi)
  {
    const pugi::xml_node& node = root.child(oms2::ssd::ssd_system);
    // read model name
    std::string ident_;
    for (auto it = node.attributes_begin(); it != node.attributes_end(); ++it)
    {
      std::string name = it->name();
      if (name == "name")
        ident_ = it->value();
    }
    if (!ident_.empty())
    {
      *ident = (char*) malloc(strlen(ident_.c_str()) + 1);
      strcpy(*ident, ident_.c_str());
    }
    return oms_status_ok;
  }
  else if (modelType == oms_component_tlm)
  {
#if !defined(NO_TLM)
    logError("oms2::Model::ParseString: not implemented for TLM yet");
    return oms_status_error;
#else
    THROW_NO_TLM();
#endif
  }
  return oms_status_error;
}

oms2::Model* oms2::Model::LoadModel(const std::string& filename)
{
  oms_element_type_enu_t modelType = oms_component_none_old;
  bool defaultExperiment = false;

  pugi::xml_document doc;
  /* If the filename is a valid file then we reached via oms2_loadModel
   * Otherwise oms2_loadModelFromString is called.
   */
  if (boost::filesystem::exists(filename))
  {
    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result)
    {
      logError("loading \"" + std::string(filename) + "\" failed (" + std::string(result.description()) + ")");
      return NULL;
    }
  }
  else
  {
    pugi::xml_parse_result result = doc.load_buffer(filename.c_str(), strlen(filename.c_str()));
    if (!result)
    {
      logError("loadModel failed (" + std::string(result.description()) + ")");
      return NULL;
    }
  }

  const pugi::xml_node root = doc.document_element();

  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if (name == "TLMModel" && modelType == oms_component_none_old)
      modelType = oms_component_tlm;
    else if (name == oms2::ssd::ssd_system && modelType == oms_component_none_old)
      modelType = oms_component_fmi;
    else if (name == oms2::ssd::ssd_default_experiment && !defaultExperiment)
      defaultExperiment = true;
    else
    {
      logError("wrong xml schema detected");
      return NULL;
    }
  }

  oms2::CompositeModel* compositeModel = NULL;
  if (modelType == oms_component_fmi)
    compositeModel = oms2::FMICompositeModel::LoadModel(root.child(oms2::ssd::ssd_system));
  else if (modelType == oms_component_tlm)
#if !defined(NO_TLM)
    compositeModel = oms2::TLMCompositeModel::LoadModel(root.child("TLMModel"));
#else
    THROW_NO_TLM();
#endif

  if (!compositeModel)
    return NULL;

  // check if model already exists
  oms2::Model* model = oms2::Scope::GetInstance().getModel(compositeModel->getName(), false);
  if (model)
  {
    logInfo("Updating existing model \"" + model->getName() + "\"");
    oms2::Model::DeleteModel(model);
  }

  model = new oms2::Model(compositeModel->getName());
  model->compositeModel = compositeModel;

  if (defaultExperiment)
  {
    const pugi::xml_node& experiment = root.child(oms2::ssd::ssd_default_experiment);
    for (auto it = experiment.attributes_begin(); it != experiment.attributes_end(); ++it)
    {
      std::string name = it->name();
      if (name == "startTime")
        model->setStartTime(it->as_double());
      else if (name == "stopTime")
        model->setStopTime(it->as_double());
      else
      {
        logError("Unknown \"Experiment\" attribute: " + name);
        oms2::Model::DeleteModel(model);
        return NULL;
      }
    }
  }

  return model;
}

struct xmlStringWriter : pugi::xml_writer
{
  std::string result;
  virtual void write(const void* data, size_t size)
  {
    result += std::string(static_cast<const char*>(data), size);
  }
};

oms_status_enu_t oms2::Model::saveOrList(const std::string& filename, char** contents)
{
  pugi::xml_document doc;

  // generate XML declaration
  pugi::xml_node declarationNode = doc.append_child(pugi::node_declaration);
  declarationNode.append_attribute("version") = "1.0";
  declarationNode.append_attribute("encoding") = "UTF-8";
  pugi::xml_node ssd = doc.append_child(oms2::ssd::ssd_system_structure_description);

  // ssd:SystemStructureDescription
  ssd.append_attribute("name") = getName().toString().c_str();
  ssd.append_attribute("version") = "Draft20171219";
  //pugi::xml_node experiment = ssd.append_child(oms2::ssd::ssd_enumerations);
  //pugi::xml_node experiment = ssd.append_child(oms2::ssd::ssd_units);
  //pugi::xml_node experiment = ssd.append_child(oms2::ssd::ssd_annotations);

  pugi::xml_node ssd_System = ssd.append_child(oms2::ssd::ssd_system);
  ssd_System.append_attribute("name") = getName().toString().c_str();

  oms_status_enu_t status;
  switch (getType())
  {
    case oms_component_fmi:
      status = getFMICompositeModel()->save(ssd_System);
      break;

    case oms_component_tlm:
      logError("xml export isn't implemented yet for TLM composite models");
      status = oms_status_error;
      break;
  }

  if (oms_status_ok != status)
    return status;

  // ssd:DefaultExperiment
  pugi::xml_node ssd_DefaultExperiment = ssd.append_child(oms2::ssd::ssd_default_experiment);
  ssd_DefaultExperiment.append_attribute("startTime") = std::to_string(startTime).c_str();
  ssd_DefaultExperiment.append_attribute("stopTime") = std::to_string(stopTime).c_str();

  if (!filename.empty())
  {
    if (!doc.save_file(filename.c_str()))
    {
      logError("xml export failed for \"" + filename + "\" (model \"" + getName() + "\")");
      return oms_status_error;
    }
  }
  else
  {
    xmlStringWriter writer;

    doc.save(writer);
    *contents = (char*) malloc(strlen(writer.result.c_str()) + 1);
    strcpy(*contents, writer.result.c_str());
  }
  return oms_status_ok;
}

oms_status_enu_t oms2::Model::save(const std::string& filename)
{
  logTrace();
  return saveOrList(filename, NULL);
}

oms_status_enu_t oms2::Model::list(char** contents)
{
  logTrace();
  return saveOrList("", contents);
}

void oms2::Model::setLoggingSamples(int value)
{
  loggingInterval = (stopTime-startTime)/value;
}

int oms2::Model::getLoggingSamples()
{
  return (stopTime-startTime)/loggingInterval;
}

oms2::FMICompositeModel* oms2::Model::getFMICompositeModel()
{
  if (oms_component_fmi == getType())
    return dynamic_cast<FMICompositeModel*>(compositeModel);

  logError("[oms2::Model::getFMICompositeModel] \"" + getName() + "\" is not a FMI composite model.");
  return NULL;
}

#if !defined(NO_TLM)
oms2::TLMCompositeModel* oms2::Model::getTLMCompositeModel()
{
  if (oms_component_tlm == getType())
    return dynamic_cast<TLMCompositeModel*>(compositeModel);

  logError("[oms2::Model::getTLMCompositeModel] \"" + getName() + "\" is not a TLM composite model.");
  return NULL;
}

oms_status_enu_t oms2::Model::setTLMInitialValues(const oms2::SignalRef &ifc, std::vector<double> values)
{
  if(this->getType() != oms_component_tlm) {
    logError("Can only set initial TLM values on TLM models.");
    return oms_status_error;
  }

  TLMCompositeModel *tlmModel = this->getTLMCompositeModel();

  return tlmModel->setTLMInitialValues(ifc, values);
}
#endif

oms_status_enu_t oms2::Model::initialize()
{
  if (oms_modelState_instantiated != modelState)
  {
    logError("[oms2::Model::initialize] Model cannot be initialized, because it isn't in instantiate state.");
    return oms_status_error;
  }

  if (resultFile)
  {
    delete resultFile;
    resultFile = NULL;
  }

  if (!resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 4)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new oms3::CSVWriter(bufferSize);
    else if (".mat" == resulttype)
      resultFile = new oms3::MATWriter(bufferSize);
    else
      return logError("Unsupported format of the result file: " + resultFilename);
  }
  else
    resultFile = new oms3::VoidWriter(1);

  modelState = oms_modelState_initialization;
  oms_status_enu_t status = compositeModel->initialize(startTime, tolerance);

  if (oms_status_ok == status)
  {
    modelState = oms_modelState_simulation;

    if (resultFile)
    {
      logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

      // add all signals
      compositeModel->registerSignalsForResultFile(*resultFile);

      // create result file
      if (!resultFile->create(resultFilename, startTime, stopTime))
      {
        delete resultFile;
        resultFile = NULL;
        return logError("Creating result file failed");
      }

      // dump results
      compositeModel->emit(*resultFile);
    }
  }
  else
    modelState = oms_modelState_instantiated;

  return status;
}

oms_status_enu_t oms2::Model::reset(bool terminate)
{
  if (resultFile)
  {
    compositeModel->emit(*resultFile);
    resultFile->close();
    delete resultFile;
    resultFile = NULL;
  }

  if (!terminate)
  {
    if (!resultFilename.empty())
    {
      std::string resulttype;
      if (resultFilename.length() > 4)
        resulttype = resultFilename.substr(resultFilename.length() - 4);

      if (".csv" == resulttype)
        resultFile = new oms3::CSVWriter(bufferSize);
      else if (".mat" == resulttype)
        resultFile = new oms3::MATWriter(bufferSize);
      else
        return logError("Unsupported format of the result file: " + resultFilename);
    }
    else
      resultFile = new oms3::VoidWriter(1);
  }

  oms_status_enu_t status = compositeModel->reset(terminate);

  modelState = oms_modelState_instantiated;

  return status;
}

oms_status_enu_t oms2::Model::simulate()
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::simulate] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, stopTime, communicationInterval, loggingInterval, masterAlgorithm, false);
  return status;
}

oms_status_enu_t oms2::Model::doSteps(const int numberOfSteps)
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::doSteps] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->doSteps(*resultFile, numberOfSteps, communicationInterval, loggingInterval);
  return status;
}

oms_status_enu_t oms2::Model::stepUntil(const double timeValue)
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::stepUntil] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, timeValue, communicationInterval, loggingInterval, masterAlgorithm, false);
  return status;
}

oms_status_enu_t oms2::Model::simulate_asynchronous(void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  if (oms_modelState_simulation != modelState)
  {
    logError("[oms2::Model::simulate_asynchronous] Model cannot be simulated, because it isn't initialized.");
    return oms_status_error;
  }

  std::thread([=]{compositeModel->simulate_asynchronous(*resultFile, stopTime, communicationInterval, loggingInterval, cb);}).detach();

  return oms_status_ok;
}

oms_status_enu_t oms2::Model::simulate_realtime()
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::simulate_realtime] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, stopTime, communicationInterval, loggingInterval, masterAlgorithm, true);
  return status;
}

void oms2::Model::setResultFile(const std::string& value, unsigned int bufferSize)
{
  resultFilename = value;
  this->bufferSize = bufferSize;
  if (oms_modelState_instantiated != modelState)
  {
    if (resultFile)
    {
      delete resultFile;
      resultFile = NULL;
    }

    if (!resultFilename.empty())
    {
      std::string resulttype;
      if (resultFilename.length() > 4)
        resulttype = resultFilename.substr(resultFilename.length() - 4);

      if (".csv" == resulttype)
        resultFile = new oms3::CSVWriter(bufferSize);
      else if (".mat" == resulttype)
        resultFile = new oms3::MATWriter(bufferSize);
      else
      {
        logError("Unsupported format of the result file: " + resultFilename);
        return;
      }

      logInfo("Result file: " + resultFilename + " (bufferSize=" + std::to_string(bufferSize) + ")");

      // add all signals
      compositeModel->registerSignalsForResultFile(*resultFile);

      // create result file
      if (!resultFile->create(resultFilename, startTime, stopTime))
      {
        delete resultFile;
        resultFile = NULL;
        logError("Creating result file failed");
        return;
      }
    }
  }
}
