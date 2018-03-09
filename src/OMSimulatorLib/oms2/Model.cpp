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

#include "FMICompositeModel.h"
#include "TLMCompositeModel.h"
#include "Logging.h"

#include <regex>

oms2::Model::Model(const oms2::ComRef& cref)
  : systemGeometry()
{
  logTrace();

  compositeModel = NULL;

  startTime = 0.0;
  stopTime = 1.0;
  resultFile = cref.toString() + "_res.mat";
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
    model->compositeModel = oms2::FMICompositeModel::newModel(cref);
  else if (oms_component_tlm == type)
    model->compositeModel = oms2::TLMCompositeModel::newModel(cref);

  if (!model->compositeModel)
  {
    delete model;
    model = NULL;
  }

  return model;
}

oms2::FMICompositeModel* oms2::Model::getFMICompositeModel()
{
  if (oms_component_fmi == getType())
    return dynamic_cast<FMICompositeModel*>(compositeModel);
  return NULL;
}

oms2::TLMCompositeModel* oms2::Model::getTLMCompositeModel()
{
  if (oms_component_tlm == getType())
    return dynamic_cast<TLMCompositeModel*>(compositeModel);
  return NULL;
}
