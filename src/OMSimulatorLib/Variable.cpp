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

#include "Variable.h"
#include "oms2/Logging.h"
#include "Settings.h"
#include "FMUWrapper.h"
#include "Util.h"

#include <fmilib.h>
#include <JM/jm_portability.h>

#include <iostream>
#include <string>

Variable::Variable(fmi2_import_variable_t *var, FMUWrapper* fmuInstance)
  : fmuInstance(fmuInstance), is_state(false)
{
  // extract the attributes
  name = fmi2_import_get_variable_name(var);
  description = fmi2_import_get_variable_description(var) ? fmi2_import_get_variable_description(var) : "";
  trim(description);
  fmuInstanceName = fmuInstance->getFMUInstanceName();
  vr = fmi2_import_get_variable_vr(var);
  causality = fmi2_import_get_causality(var);
  initialProperty = fmi2_import_get_initial(var);
  baseType = fmi2_import_get_variable_base_type(var);
}

Variable::~Variable()
{
}

FMUWrapper* Variable::getFMUInstance() const
{
  return fmuInstance;
}

bool operator==(const Variable& v1, const Variable& v2)
{
  return v1.name == v2.name &&
    v1.fmuInstanceName == v2.fmuInstanceName &&
    v1.vr == v2.vr;
}
bool operator!=(const Variable& v1, const Variable& v2)
{
  return !(v1 == v2);
}
