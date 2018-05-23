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

#include "TLMCompositeModel.h"

#include "ComRef.h"
#include "Logging.h"
#include "Connection.h"
#include "TLMInterface.h"
#include "ExternalModel.h"
#include "TLMConnection.h"
#include "FMICompositeModel.h"
#include "OMTLMSimulatorLib.h"
#include "Scope.h"
#include <list>
#include <algorithm>
#include <iostream>
#include <thread>

#include <pugixml.hpp>

oms2::TLMCompositeModel::TLMCompositeModel(const ComRef& name)
  : CompositeModel(oms_component_tlm, name)
{
  logTrace();
  model = omtlm_newModel(name.c_str());
  omtlm_setLogLevel(model, 1);  /// \todo Make debug log level selectable by user
}

oms2::TLMCompositeModel::~TLMCompositeModel()
{
    //Cleanup TLM interfaces
    for(TLMInterface *ifc : interfaces) {
        delete ifc;
    }
    interfaces.clear();

    //Cleanup external models
    std::map<ComRef, ExternalModel*>::iterator it;
    for(it = externalModels.begin(); it != externalModels.end(); ++it) {
        delete it->second;
    }
    externalModels.clear();

    delete model;
}

oms2::TLMCompositeModel* oms2::TLMCompositeModel::NewModel(const ComRef& name)
{
  if (!name.isValidIdent())
  {
    logError("\"" + name + "\" is not a valid model name.");
    return NULL;
  }

  oms2::TLMCompositeModel *tlmModel = new oms2::TLMCompositeModel(name);
  return tlmModel;
}

oms_status_enu_t oms2::TLMCompositeModel::addFMIModel(oms2::FMICompositeModel *fmiModel)
{
  oms2::ComRef cref = fmiModel->getName();

  auto it = fmiModels.find(cref);
  if (it != fmiModels.end())
    return logError("An FMI submodel called \"" + cref + "\" is already added.");

  omtlm_addSubModel(model, cref.c_str(), "", "none"); //Startscript must be "none"

  fmiModels[cref] = fmiModel;

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addInterface(oms2::TLMInterface *ifc)
{
  if (std::find(interfaces.begin(), interfaces.end(), ifc) != interfaces.end())
    return logError("Interface " + ifc->getSignal().toString() + " is already added.");
  FMICompositeModel *pFMISubModel = 0;
  if(Scope::GetInstance().hasFMICompositeModel(ifc->getSubModelName())) {
    pFMISubModel = Scope::GetInstance().getFMICompositeModel(ifc->getSubModelName());
  }

  if(pFMISubModel) {
    if(ifc->getDimensions() == 1 &&
       (ifc->getCausality() == oms_causality_input || ifc->getCausality() == oms_causality_output) &&
       ifc->getSubSignals().size() != 1) {
      logError("Wrong number of variables for TLM interface (should be 1)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 1 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_no_interpolation &&
       ifc->getSubSignals().size() != 3) {
      logError("Wrong number of variables for TLM interface (should be 3)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 1 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_coarse_grained &&
       ifc->getSubSignals().size() != 4) {
      logError("Wrong number of variables for TLM interface (should be 4)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 1 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_fine_grained &&
       ifc->getSubSignals().size() != 23) {
      logError("Wrong number of variables for TLM interface (should be 23)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 2 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getSubSignals().size() != 9) {
      logError("Wrong number of variables for TLM interface (should be 9)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 3 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_no_interpolation &&
       ifc->getSubSignals().size() != 24) {
      logError("Wrong number of variables for TLM interface (should be 24)");

      return oms_status_error;
    }
    if(ifc->getDimensions() == 3 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_coarse_grained &&
       ifc->getSubSignals().size() != 26) {
      logError("Wrong number of variables for TLM interface (should be 26)");
      return oms_status_error;
    }
    if(ifc->getDimensions() == 3 &&
       ifc->getCausality() == oms_causality_bidir &&
       ifc->getInterpolationMethod() == oms_tlm_fine_grained &&
       ifc->getSubSignals().size() != 90) {
      logError("Wrong number of variables for TLM interface (should be 90)");
      return oms_status_error;
    }
  }

  if (externalModels.find(ifc->getSubModelName()) == externalModels.end() && (!pFMISubModel || fmiModels.find(ifc->getSubModelName()) == fmiModels.end()))
    return logError("Sub model for TLM interface does not exist in TLM composite model.");

  //Todo: Help function for this
  std::string causality = "Input";
  if(ifc->getCausality() == oms_causality_output)
    causality = "Output";
  else if(ifc->getCausality() == oms_causality_bidir)
    causality = "Bidirectional";

  int dimensions = ifc->getDimensions();
  if(ifc->getDimensions() == 2)
    dimensions = 3;
  else if(ifc->getDimensions() == 3)
    dimensions = 6;

  omtlm_addInterface(model,
                     ifc->getSubModelName().c_str(),
                     ifc->getName().c_str(),
                     dimensions,
                     causality.c_str(),
                     ifc->getDomain().c_str());

  interfaces.push_back(ifc);

  if(pFMISubModel)
    pFMISubModel->addTLMInterface(ifc);

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addInterface(std::string name,
                                                       int dimensions,
                                                       oms_causality_enu_t causality,
                                                       std::string domain,
                                                       oms_tlm_interpolation_t interpolation,
                                                       const oms2::ComRef &cref,
                                                       std::vector<SignalRef> &sigrefs)
{
  oms2::TLMInterface *ifc = new TLMInterface(cref,name,causality,domain,dimensions,interpolation,sigrefs);
  return addInterface(ifc);
}

oms_status_enu_t oms2::TLMCompositeModel::addExternalModel(oms2::ExternalModel *externalModel)
{
  auto it = externalModels.find(externalModel->getName());
  if (it != externalModels.end())
    return logError("An external model called \"" + externalModel->getName() + "\" is already added.");

  //Copy external model file to temporary directory
  std::string modelPath = Scope::GetInstance().getTempDirectory()+"/"+externalModel->getName();
#ifdef WIN32
  std::string cmd = "if not exists \""+modelPath+"\" mkdir \""+modelPath+"\"";
  system(cmd.c_str());
  cmd = "copy \""+externalModel->getModelPath()+"\" \""+modelPath;
  system(cmd.c_str());
#else
  std::string cmd = "mkdir -p \""+modelPath+"\"";
  system(cmd.c_str());
  cmd = "cp \""+externalModel->getModelPath()+"\" \""+modelPath+"\"";
  system(cmd.c_str());
#endif

  //Extract file name from path
  size_t i1 = externalModel->getModelPath().rfind('/', externalModel->getModelPath().length());
  size_t i2 = externalModel->getModelPath().rfind('\\', externalModel->getModelPath().length());
  size_t i = i1;
  if(i2>i1) {
    i = i2;   //We cannot use std::max with MSVC for some reason
  }
  if(i1 == std::string::npos) {
    i = i2;
  }
  else if(i2 == std::string::npos) {
    i = i1;
  }
  std::string fileName = externalModel->getModelPath().substr(i+1, externalModel->getModelPath().length() - i);
  modelPath = modelPath+"/"+fileName;

  omtlm_addSubModel(model,
                    externalModel->getName().c_str(),
                    modelPath.c_str(),
                    externalModel->getStartScript().c_str());

  externalModels[externalModel->getName()] = externalModel;

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addExternalModel(std::string modelFile, std::string startScript, const oms2::ComRef &cref)
{
  ExternalModel *externalModel = new ExternalModel(cref, modelFile, startScript);
  return addExternalModel(externalModel);
}


oms_status_enu_t oms2::TLMCompositeModel::addConnection(const SignalRef &signalA,
                                                      const SignalRef &signalB,
                                                      double delay,
                                                      double alpha,
                                                      double Zf,
                                                      double Zfr)
{
  return addConnection(oms2::TLMConnection(this->getName(),signalA,signalB,delay,alpha,Zf,Zfr));
}

oms_status_enu_t oms2::TLMCompositeModel::setSocketData(const std::string& address,
                                                        int managerPort,
                                                        int monitorPort)
{
#ifndef _WIN32
  omtlm_checkPortAvailability(&managerPort);
  omtlm_checkPortAvailability(&monitorPort);
#endif

  omtlm_setAddress(model, address);
  omtlm_setManagerPort(model, managerPort);
  omtlm_setMonitorPort(model, monitorPort);

  this->address = address;
  this->managerPort = managerPort;
  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::describe()
{
  omtlm_printModelStructure(model);
  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addConnection(const TLMConnection &con)
{
  if(std::find(connections.begin(), connections.end(), con) != connections.end())
    return logError("Connection between " + con.getSignalA().toString() + " and " + con.getSignalB().toString() + " is already added.");

  std::string interface1 = con.getSignalA().getCref().toString()+"."+con.getSignalA().getVar();
  std::string interface2 = con.getSignalB().getCref().toString()+"."+con.getSignalB().getVar();

  omtlm_addConnection(model,
                      interface1.c_str(),
                      interface2.c_str(),
                      con.getTimeDelay(),
                      con.getZf(),
                      con.getZfr(),
                      con.getAlpha());
  connections.push_back(con);

  for(TLMInterface* ifc : interfaces) {
    if(ifc->getSubModelName().toString() + "." + ifc->getName() == interface1 ||
       ifc->getSubModelName().toString() + "." + ifc->getName() == interface2) {
      ifc->setDelay(con.getTimeDelay());
    }
  }

  return oms_status_ok;
}

oms2::TLMCompositeModel* oms2::TLMCompositeModel::LoadModel(const pugi::xml_node& node)
{
  logError("oms2::TLMCompositeModel::LoadModel: not implemented yet");
  return NULL;
}

oms_status_enu_t oms2::TLMCompositeModel::initialize(double startTime, double tolerance)
{
  Model* pModel = oms2::Scope::GetInstance().getModel(getName());
  omtlm_setStartTime(model, startTime);
  omtlm_setNumLogStep(model, 1000); //Hard-coded for now

  //Initialize sub-models
  for(auto it = fmiModels.begin(); it!=fmiModels.end(); ++it) {
    Model* pSubModel = oms2::Scope::GetInstance().getModel(it->second->getName());
    pSubModel->setStartTime(startTime);
    pModel->setTolerance(pModel->getTolerance());
    pSubModel->initialize();
  }

  this->startTime = startTime;

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::reset()
{
  return logError("oms2::TLMCompositeModel::reset: not implemented yet");
}

oms_status_enu_t oms2::TLMCompositeModel::terminate()
{
  return logError("oms2::TLMCompositeModel::terminate: not implemented yet");
}

oms_status_enu_t oms2::TLMCompositeModel::simulate(ResultWriter &resultWriter, double stopTime, double communicationInterval, oms2::MasterAlgorithm masterAlgorithm)
{
  return logError("oms2::TLMCompositeModel::simulate: not implemented yet");
}

oms_status_enu_t oms2::TLMCompositeModel::doSteps(ResultWriter& resultWriter, const int numberOfSteps, double communicationInterval)
{
  return logError("oms2::TLMCompositeModel::doSteps: not implemented yet");
}

oms_status_enu_t oms2::TLMCompositeModel::stepUntil(ResultWriter &resultWriter, double stopTime, double communicationInterval, oms2::MasterAlgorithm masterAlgorithm, bool realtime_sync)
{
  if(fmiModels.empty() && externalModels.empty())
    logWarning("oms2::TLMCompositeModel::stepUntil: Simulating empty model...");

  logInfo("Starting submodel threads.");
  std::string server = address + ":" + std::to_string(managerPort);
  std::vector<std::thread*> fmiModelThreads;
  for(auto it = fmiModels.begin(); it!=fmiModels.end(); ++it)
  {
    Model* pModel = oms2::Scope::GetInstance().getModel(it->second->getName());
    ResultWriter *pWriter = pModel->getResultWriter();
    std::thread *t = new std::thread(&FMICompositeModel::simulateTLM, it->second, pWriter, stopTime, communicationInterval, server);
    fmiModelThreads.push_back(t);
  }

  logInfo("Starting OMTLMSimulator in main thread.");
  omtlm_setStopTime(model, stopTime);
  omtlm_simulate(model);

  for(size_t i=0; i<fmiModelThreads.size(); ++i)
    fmiModelThreads[i]->join();

  for(size_t i=0; i<fmiModelThreads.size(); ++i)
    delete fmiModelThreads[i];

  logInfo("Simulation of TLM composite model "+getName().toString()+" complete.");

  return oms_status_ok;
}

void oms2::TLMCompositeModel::simulate_asynchronous(ResultWriter& resultWriter, double stopTime, double communicationInterval, void (*cb)(const char* ident, double time, oms_status_enu_t status))
{
  logTrace();

  logError("oms2::TLMCompositeModel::simulate_asynchronous: Function is not implemented, yet.");
  cb(this->getName().c_str(), 0, oms_status_error);
}
