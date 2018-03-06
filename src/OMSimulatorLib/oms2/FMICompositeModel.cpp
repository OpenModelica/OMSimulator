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

#include "FMICompositeModel.h"

#include "../Types.h"
#include "ComRef.h"
#include "FMUWrapper.h"
#include "Logging.h"
#include "SignalRef.h"

oms2::FMICompositeModel::FMICompositeModel(const ComRef& name)
  : oms2::Model(oms_component_fmi, name)
{
  logTrace();
  connections.push_back(NULL);
  components = NULL;
}

oms2::FMICompositeModel::~FMICompositeModel()
{
  logTrace();

  // free memory if no one else does
  deleteComponents();

  for (auto& connection : connections)
    if (connection)
      delete connection;

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

oms_status_enu_t oms2::FMICompositeModel::addFMU(const std::string& filename, const oms2::ComRef& cref)
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

  oms2::ComRef parent = getName();
  oms2::FMUWrapper* subModel = oms2::FMUWrapper::newSubModel(parent + cref, filename);
  if (!subModel)
    return oms_status_error;

  deleteComponents();

  subModels[cref] = subModel;
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::addTable(const std::string& filename, const oms2::ComRef& cref)
{
  deleteComponents();

  logError("[oms2::FMICompositeModel::addTable] not implemented yet");
  return oms_status_error;
}

oms_status_enu_t oms2::FMICompositeModel::deleteSubModel(const oms2::ComRef& cref)
{
  auto it = subModels.find(cref);
  if (it == subModels.end())
  {
    logError("No sub-model called \"" + cref + "\" instantiated.");
    return oms_status_error;
  }
  oms2::FMISubModel::deleteSubModel(it->second);
  subModels.erase(it);

  // delete associated connections
  for (int i=0; i<connections.size()-1; ++i)
  {
    if (!connections[i])
    {
      logError("[oms2::FMICompositeModel::deleteSubModel] null pointer");
      return oms_status_error;
    }
    else if(connections[i]->getSignalA().getCref() == cref)
    {
      delete connections[i];
      connections.pop_back();   // last element is always NULL
      connections[i] = connections.back();
      connections.back() = NULL;
      i--;
    }
    else if(connections[i]->getSignalB().getCref() == cref)
    {
      delete connections[i];
      connections.pop_back();   // last element is always NULL
      connections[i] = connections.back();
      connections.back() = NULL;
      i--;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::setRealParameter(const oms2::SignalRef& sr, double value)
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

oms2::Connection* oms2::FMICompositeModel::getConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::ComRef parent = getName();
  for (auto& it : connections)
    if (it && it->isEqual(parent, conA, conB))
      return it;
  return NULL;
}

oms_status_enu_t oms2::FMICompositeModel::addConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  /// \todo check the connection
  oms2::ComRef parent = getName();
  connections.back() = new oms2::Connection(parent, conA, conB);
  connections.push_back(NULL);
  return oms_status_ok;
}

oms_status_enu_t oms2::FMICompositeModel::deleteConnection(const oms2::SignalRef& conA, const oms2::SignalRef& conB)
{
  oms2::ComRef parent = getName();
  for (auto& it : connections)
  {
    if (it && it->isEqual(parent, conA, conB))
    {
      delete it;

      connections.pop_back();   // last element is always NULL
      it = connections.back();
      connections.back() = NULL;

      return oms_status_ok;
    }
  }

  return oms_status_error;
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

void oms2::FMICompositeModel::deleteComponents()
{
  logTrace();

  if (this->components)
  {
    delete[] components;
    components = NULL;
  }
}

void oms2::FMICompositeModel::updateComponents()
{
  logTrace();

  deleteComponents();

  components = new oms2::Element*[subModels.size() + 1];
  components[subModels.size()] = NULL;

  int i=0;
  for (auto& it : subModels)
    components[i++] = it.second->getElement();
}

oms_status_enu_t oms2::FMICompositeModel::renameSubModel(const oms2::ComRef& identOld, const oms2::ComRef& identNew)
{
  oms2::ComRef identOld_ = identOld.last();
  oms2::ComRef identNew_ = identNew.last();

  if (!identNew_.isValidIdent())
  {
    logError("Identifier \"" + identNew + "\" is invalid.");
    return oms_status_error;
  }

  if (!identOld_.isValidIdent())
  {
    logError("Identifier \"" + identOld + "\" is invalid.");
    return oms_status_error;
  }

  // check if identNew is in scope
  auto it = subModels.find(identNew_);
  if (it != subModels.end())
  {
    logError("A sub-model called \"" + identNew.toString() + "\" is already in scope.");
    return oms_status_error;
  }

  // check if identOld is in scope
  it = subModels.find(identOld_);
  if (it == subModels.end())
  {
    logError("There is no sub-model called \"" + identOld.toString() + "\" in scope.");
    return oms_status_error;
  }

  it->second->setName(identNew);
  subModels[identNew_] = it->second;
  subModels.erase(it);

  return oms_status_ok;
}

oms2::Element** oms2::FMICompositeModel::getElements()
{
  logTrace();

  if (components)
    return components;

  updateComponents();
  return components;
}
