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
#include "MATWriter.h"
#include "FMICompositeModel.h"
#include "TLMCompositeModel.h"
#include "Logging.h"
#include "ssd/Tags.h"

#include <regex>
#include <thread>

#include <pugixml.hpp>

oms2::Model::Model(const oms2::ComRef& cref)
  : systemGeometry(), resultFilename(cref.toString() + "_res.mat")
{
  logTrace();
  modelState = oms_modelState_instantiated;
}

oms2::Model::~Model()
{
  if (compositeModel)
    CompositeModel::DeleteModel(compositeModel);
}

oms2::Model* oms2::Model::NewModel(oms_element_type_enu_t type, const oms2::ComRef& cref)
{
  oms2::Model* model = new oms2::Model(cref);

  if (oms_component_fmi == type)
    model->compositeModel = oms2::FMICompositeModel::NewModel(cref);
  else if (oms_component_tlm == type)
    model->compositeModel = oms2::TLMCompositeModel::NewModel(cref);

  if (!model->compositeModel)
  {
    delete model;
    model = NULL;
  }

  return model;
}

oms2::Model* oms2::Model::LoadModel(const std::string& filename)
{
  oms_element_type_enu_t modelType = oms_component_none;
  bool defaultExperiment = false;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
  {
    logError("loading \"" + std::string(filename) + "\" failed (" + std::string(result.description()) + ")");
    return NULL;
  }

  const pugi::xml_node root = doc.document_element();

  for(pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
  {
    std::string name = it->name();
    if (name == "TLMModel" && modelType == oms_component_none)
      modelType = oms_component_tlm;
    else if (name == oms2::ssd::ssd_system && modelType == oms_component_none)
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
    compositeModel = oms2::TLMCompositeModel::LoadModel(root.child("TLMModel"));

  if (!compositeModel)
    return NULL;

  oms2::Model* model = new oms2::Model(compositeModel->getName());
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

oms_status_enu_t oms2::Model::save(const std::string& filename)
{
  logTrace();
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

  if (!doc.save_file(filename.c_str()))
  {
    logError("xml export failed for \"" + filename + "\" (model \"" + getName() + "\")");
    return oms_status_error;
  }
  return oms_status_ok;
}

oms2::FMICompositeModel* oms2::Model::getFMICompositeModel()
{
  if (oms_component_fmi == getType())
    return dynamic_cast<FMICompositeModel*>(compositeModel);

  logError("[oms2::Model::getFMICompositeModel] \"" + getName() + "\" is not a FMI composite model.");
  return NULL;
}

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
    if (resultFilename.length() > 5)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(1);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(1024);
    else
      return logError("Unsupported format of the result file: " + resultFilename);
  }

  modelState = oms_modelState_initialization;
  oms_status_enu_t status = compositeModel->initialize(startTime, tolerance);

  if (oms_status_ok == status)
  {
    modelState = oms_modelState_simulation;

    if (resultFile)
    {
      logInfo("Result file: " + resultFilename);

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

oms_status_enu_t oms2::Model::reset()
{

  if (resultFile)
  {
    resultFile->close();
    delete resultFile;
    resultFile = NULL;
  }

  if (!resultFilename.empty())
  {
    std::string resulttype;
    if (resultFilename.length() > 5)
      resulttype = resultFilename.substr(resultFilename.length() - 4);

    if (".csv" == resulttype)
      resultFile = new CSVWriter(1);
    else if (".mat" == resulttype)
      resultFile = new MATWriter(1024);
    else
      return logError("Unsupported format of the result file: " + resultFilename);
  }

  oms_status_enu_t status = compositeModel->reset();

  modelState = oms_modelState_instantiated;

  return status; // 2018-04-24: Careful, return value is fixed to oms_status_ok
}

oms_status_enu_t oms2::Model::terminate()
{
  if (oms_modelState_initialization != modelState && oms_modelState_simulation != modelState)
  {
    logError("[oms2::Model::terminate] Model cannot be terminated, because it isn't in simulation state.");
    return oms_status_error;
  }

  modelState = oms_modelState_initialization;

  oms_status_enu_t status = compositeModel->terminate();
  if (oms_status_ok == status)
  {
    modelState = oms_modelState_instantiated;
    logInfo("Simulation finished.");
  }

  if (resultFile)
  {
    resultFile->close();
    delete resultFile;
    resultFile = NULL;
  }

  return status;
}

oms_status_enu_t oms2::Model::simulate()
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::simulate] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, stopTime, communicationInterval, masterAlgorithm, false);
  return status;
}

oms_status_enu_t oms2::Model::doSteps(const int numberOfSteps)
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::doSteps] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->doSteps(*resultFile, numberOfSteps, communicationInterval);
  return status;
}

oms_status_enu_t oms2::Model::stepUntil(const double timeValue)
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::stepUntil] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, timeValue, communicationInterval, masterAlgorithm, false);
  return status;
}

oms_status_enu_t oms2::Model::simulate_asynchronous(void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  if (oms_modelState_simulation != modelState)
  {
    logError("[oms2::Model::simulate_asynchronous] Model cannot be simulated, because it isn't initialized.");
    return oms_status_error;
  }

  std::thread([=]{compositeModel->simulate_asynchronous(*resultFile, stopTime, communicationInterval, cb);}).detach();

  return oms_status_ok;
}

oms_status_enu_t oms2::Model::simulate_realtime()
{
  if (oms_modelState_simulation != modelState)
    return logError("[oms2::Model::simulate_realtime] Model cannot be simulated, because it isn't initialized.");

  oms_status_enu_t status = compositeModel->stepUntil(*resultFile, stopTime, communicationInterval, masterAlgorithm, true);
  return status;
}

void oms2::Model::setResultFile(const std::string& value)
{
  resultFilename = value;
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
      if (resultFilename.length() > 5)
        resulttype = resultFilename.substr(resultFilename.length() - 4);

      if (".csv" == resulttype)
        resultFile = new CSVWriter(1);
      else if (".mat" == resulttype)
        resultFile = new MATWriter(1024);
      else
      {
        logError("Unsupported format of the result file: " + resultFilename);
        return;
      }

      logInfo("Result file: " + resultFilename);

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
