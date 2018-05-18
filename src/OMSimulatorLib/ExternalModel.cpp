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

#include "ExternalModel.h"
#include "Logging.h"
#include "Element.h"
#include "Types.h"

#include <cstring>
#include <map>
#include <iostream>


oms_status_enu_t oms2::ExternalModel::setRealParameter(const std::string &var, double value)
{
  std::map<std::string, oms2::Option<double>>::iterator it;
  it = realParameters.find(var);

  if(it != realParameters.end()) {
    it->second.setValue(value);
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms2::ExternalModel::getRealParameter(const std::string &var, double &value)
{
  std::map<std::string, oms2::Option<double>>::iterator it;
  it = realParameters.find(var);

  if(it != realParameters.end()) {
    value = it->second.getValue();
    return oms_status_ok;
  }

  return oms_status_error;
}

oms2::ExternalModel::ExternalModel(const oms2::ComRef &cref, const std::string &filename, const std::string &startScript)
  : element(oms_component_external, cref)
{
  logTrace();

  this->filename = filename;
  this->startScript = startScript;
  this->cref = cref;
}



//oms2::ExternalModel::ExternalModel(oms2::ExternalModel& rhs)
//{
//  geometry.x1 = rhs.getGeometry()->x1;
//  geometry.y1 = rhs.getGeometry()->y1;
//  geometry.x2 = rhs.getGeometry()->x2;
//  geometry.y2 = rhs.getGeometry()->y2;
//  component.name() = rhs.getName();

//  component.type = oms_component_external;
//  component.interfaces = NULL;

//  this->filename = filename;
//  this->startScript = startScript;
//  this->cref = cref;
//}

//oms2::ExternalModel::ExternalModel(const oms2::ExternalModel& rhs)
//{
//  geometry.x1 = rhs.getGeometry()->x1;
//  geometry.y1 = rhs.getGeometry()->y1;
//  geometry.x2 = rhs.getGeometry()->x2;
//  geometry.y2 = rhs.getGeometry()->y2;
//  component.name() = rhs.getName();

//  component.type = oms_component_external;
//  component.interfaces = NULL;

//  this->filename = filename;
//  this->startScript = startScript;
//  this->cref = cref;
//}

//oms2::ExternalModel& oms2::ExternalModel::operator=(const oms2::ExternalModel& rhs)
//{
//  // check for self-assignment
//  if(&rhs == this)
//    return *this;

//  geometry.x1 = rhs.getGeometry()->x1;
//  geometry.y1 = rhs.getGeometry()->y1;
//  geometry.x2 = rhs.getGeometry()->x2;
//  geometry.y2 = rhs.getGeometry()->y2;
//  component.name() = rhs.getName();

//  component.type = oms_component_external;
//  component.interfaces = NULL;

//  this->filename = filename;
//  this->startScript = startScript;
//  this->cref = cref;

//  return *this;
//}
