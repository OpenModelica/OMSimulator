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
#include <list>
#include <algorithm>
#include <iostream>

#include <pugixml.hpp>

oms2::TLMCompositeModel::TLMCompositeModel(const ComRef& name)
  : CompositeModel(oms_component_tlm, name)
{
  logTrace();
  model = omtlm_newModel(name.c_str());
}

oms2::TLMCompositeModel::~TLMCompositeModel()
{
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
  {
    logError("An FMI submodel called \"" + cref + "\" is already added.");
    return oms_status_error;
  }

  omtlm_addSubModel(model, cref.c_str(), "", "");

  fmiModels[cref] = fmiModel;

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addInterface(oms2::TLMInterface &interface)
{
  if(std::find(interfaces.begin(), interfaces.end(), interface) != interfaces.end()) {
    logError("Interface "+interface.getSignal().toString()+" is already added.");
    return oms_status_error;
  }

  //Todo: Help function for this
  std::string causality = "Input";
  if(interface.getCausality() == oms_tlm_causality_output) {
    causality = "Output";
  }
  else if(interface.getCausality() == oms_tlm_causality_bidirecitonal) {
    causality = "Bidirectional";
  }

  omtlm_addInterface(model,
                     interface.getSubModelName().c_str(),
                     interface.getName().c_str(),
                     interface.getDimensions(),
                     causality.c_str(),
                     interface.getDomain().c_str());

  interfaces.push_back(interface);

  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addInterface(std::string name,
                                                   int dimensions,
                                                   oms_tlm_causality_t causality,
                                                   std::string domain,
                                                   const oms2::ComRef &cref)
{
  oms2::TLMInterface interface(cref,name,causality,domain,dimensions);
  return addInterface(interface);
}

oms_status_enu_t oms2::TLMCompositeModel::addExternalModel(oms2::ExternalModel *externalModel)
{
  auto it = externalModels.find(externalModel->getName());
  if (it != externalModels.end())
  {
    logError("An external model called \"" + externalModel->getName() + "\" is already added.");
    return oms_status_error;
  }

  omtlm_addSubModel(model,
                    externalModel->getName().c_str(),
                    externalModel->getModelPath().c_str(),
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

oms_status_enu_t oms2::TLMCompositeModel::describe()
{
  omtlm_printModelStructure(model);
  return oms_status_ok;
}

oms_status_enu_t oms2::TLMCompositeModel::addConnection(const TLMConnection &con)
{
  if(std::find(connections.begin(), connections.end(), con) != connections.end()) {
    logError("Connection between "+con.getSignalA().toString()+
             " and "+con.getSignalB().toString()+" is already added.");
    return oms_status_error;
  }

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
  return oms_status_ok;
}

oms2::TLMCompositeModel* oms2::TLMCompositeModel::LoadModel(const pugi::xml_node& node)
{
  logError("oms2::TLMCompositeModel::LoadModel: not implemented yet");
  return NULL;
}
