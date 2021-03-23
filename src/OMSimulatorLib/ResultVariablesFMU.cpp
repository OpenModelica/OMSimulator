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
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "ResultVariablesFMU.h"


oms::ResultVariables::~ResultVariables()
{
  delete reals.value;
  delete reals.vr;
  delete reals.id;

  delete integers.value;
  delete integers.vr;
  delete integers.id;

  delete booleans.value;
  delete booleans.vr;
  delete booleans.id;
}

oms_status_enu_t oms::ResultVariables::allocVarArrays(unsigned int nReals, unsigned int nInts, unsigned int nBools)
{
  reals.nvr = nReals;
  reals.value = new double[nReals];
  reals.vr = new unsigned int[nReals];
  reals.id = new unsigned int[nReals];

  integers.nvr = nInts;
  integers.value = new int[nInts];
  integers.vr = new unsigned int[nInts];
  integers.id = new unsigned int[nInts];

  booleans.nvr = nBools;
  booleans.value = new bool[nBools];
  booleans.vr = new unsigned int[nBools];
  booleans.id = new unsigned int[nBools];

  return oms_status_ok;
}


/**
 * @brief Register result variables.
 *
 * Set value references and id's for arrays with reals, ints and bools.
 *
 * @param allVariables        Reference to all variables vector.
 * @param exportVariables     Reference to boolean vector if variable should be exported.
 * @return oms_status_enu_t 
 */
oms_status_enu_t oms::ResultVariables::registerVariables(const std::vector<Variable> &allVariables, const std::vector<bool> &exportVariables)
{
  unsigned int jRealVars = 0;
  unsigned int jIntVars = 0;
  unsigned int jBoolVars = 0;
  for (unsigned int i=0; i<allVariables.size(); ++i)
  {
    if (!exportVariables[i])
      continue;

    auto const &var = allVariables[i];
    if (var.isTypeReal())
    {
      reals.vr[jRealVars] = var.getValueReference();
      reals.id[jRealVars] = i;
      jRealVars++;
    }
    else if (var.isTypeInteger())
    {
      integers.vr[jIntVars] = var.getValueReference();
      integers.id[jIntVars] = i;
      jIntVars++;
    }
    else if (var.isTypeBoolean())
    {
      booleans.vr[jBoolVars] = var.getValueReference();
      booleans.id[jBoolVars] = i;
      jBoolVars++;
    }
  }

  return oms_status_ok;
}
