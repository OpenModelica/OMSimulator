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
#include "Logging.h"

oms2::Scope::Scope()
{
}

oms2::Scope::~Scope()
{
  // free memory if no one else did it
  for (auto it=fmiModels.begin(); it != fmiModels.end(); it++)
    oms2::FMICompositeModel::deleteModel(it->second);
}

oms2::Scope& oms2::Scope::getInstance()
{
  // the only instance
  static Scope scope;
  return scope;
}

oms_status_t oms2::Scope::newFMIModel(const std::string& name)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  if (scope.fmiModels.find(name) != scope.fmiModels.end())
  {
    logError("A model called \"" + name + "\" is already in the scope.");
    return oms_status_error;
  }

  scope.fmiModels[name] = oms2::FMICompositeModel::newModel(name);
  return oms_status_ok;
}

oms_status_t oms2::Scope::unloadModel(const std::string& name)
{
  Scope& scope = oms2::Scope::getInstance();

  // check if name is in scope
  auto it = scope.fmiModels.find(name);
  if (it == scope.fmiModels.end())
  {
    logError("There is no model called \"" + name + "\" in the scope.");
    return oms_status_error;
  }

  oms2::FMICompositeModel::deleteModel(it->second);
  scope.fmiModels.erase(it);

  return oms_status_ok;
}
