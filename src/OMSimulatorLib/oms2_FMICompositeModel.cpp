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
#include "oms2_FMUWrapper.h"
#include "oms2_SignalRef.h"

oms2::FMICompositeModel::FMICompositeModel(const ComRef& name)
  : oms2::Model(name)
{
  logTrace();
}

oms2::FMICompositeModel::~FMICompositeModel()
{
  // free memory if no one else does
  for (auto it=subModels.begin(); it != subModels.end(); it++)
    oms2::FMISubModel::deleteSubModel(it->second);
}

oms2::FMICompositeModel* oms2::FMICompositeModel::newModel(const ComRef& name)
{
  if (!name.isValidIdent())
  {
    logError("\"" + name + "\" is not a valid model name.");
    return NULL;
  }

  oms2::FMICompositeModel *model = new oms2::FMICompositeModel(name);
  return model;
}

oms_status_t oms2::FMICompositeModel::instantiateFMU(const std::string& filename, const oms2::ComRef& cref)
{
  if (!cref.isValidIdent())
    return oms_status_error;

  // check if cref is already used
  auto it = subModels.find(cref);
  if (it != subModels.end())
  {
    logError("A submodel called \"" + cref + "\" is already instantiated.");
    return oms_status_error;
  }

  oms2::FMUWrapper* subModel = oms2::FMUWrapper::newSubModel(name + cref, filename);
  if (!subModel)
    return oms_status_error;

  if (components)
  {
    delete[] components;
    components = NULL;
  }

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_t oms2::FMICompositeModel::instantiateTable(const std::string& filename, const oms2::ComRef& cref)
{
  if (components)
  {
    delete[] components;
    components = NULL;
  }

  logError("[oms2::FMICompositeModel::instantiateTable] not implemented yet");
  return oms_status_error;
}

oms_status_t oms2::FMICompositeModel::setRealParameter(const oms2::SignalRef& sr, double value)
{
  auto it = subModels.find(sr.getCref().last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + sr.getCref() + "\" found.");
    return oms_status_error;
  }

  if (oms_component_fmu != it->second->getType())
  {
    logError("[oms2::FMICompositeModel::setRealParameter] can only be used for FMUs");
    return oms_status_error;
  }

  FMUWrapper* fmu = dynamic_cast<FMUWrapper*>(it->second);
  return fmu->setRealParameter(sr.getVar(), value);
}

oms_status_t oms2::FMICompositeModel::addConnection(const oms2::SignalRef& sigA, const oms2::SignalRef& sigB)
{
  return addConnection(oms2::Connection(sigA, sigB));
}

oms_status_t oms2::FMICompositeModel::addConnection(const oms2::Connection& connection)
{
  /// \todo check the connection
  connections.push_back(connection);
  return oms_status_ok;
}

oms2::FMISubModel* oms2::FMICompositeModel::getSubModel(const oms2::ComRef& cref)
{
  auto it = subModels.find(cref.last());
  if (it == subModels.end())
  {
    logError("No submodel called \"" + cref + "\" found.");
    return NULL;
  }

  return it->second;
}

void oms2::FMICompositeModel::updateComponents()
{
  logTrace();

  if (components)
    delete[] components;

  components = new oms_component_t*[subModels.size() + 1];
  components[subModels.size()] = NULL;

  int i=0;
  for (auto& it : subModels)
    components[i++] = it.second->getComponent();
}
