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
  delete realResultVars.value;
  delete realResultVars.vr;
  delete realResultVars.id;

  delete intResultVars.value;
  delete intResultVars.vr;
  delete intResultVars.id;

  delete boolResultVars.value;
  delete boolResultVars.vr;
  delete boolResultVars.id;
}

oms_status_enu_t oms::ResultVariables::allocVarArrays(unsigned int nReals, unsigned int nInts, unsigned int nBools)
{
  realResultVars.nvr = nReals;
  realResultVars.value = new double[nReals];
  realResultVars.vr = new unsigned int[nReals];
  realResultVars.id = new unsigned int[nReals];

  intResultVars.nvr = nInts;
  intResultVars.value = new int[nInts];
  intResultVars.vr = new unsigned int[nInts];
  intResultVars.id = new unsigned int[nInts];

  boolResultVars.nvr = nBools;
  boolResultVars.value = new bool[nBools];
  boolResultVars.vr = new unsigned int[nBools];
  boolResultVars.id = new unsigned int[nBools];

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
      realResultVars.vr[jRealVars] = var.getValueReference();
      realResultVars.id[jRealVars] = i;
      jRealVars++;
    }
    else if (var.isTypeInteger())
    {
      intResultVars.vr[jIntVars] = var.getValueReference();
      intResultVars.id[jIntVars] = i;
      jIntVars++;
    }
    else if (var.isTypeBoolean())
    {
      boolResultVars.vr[jBoolVars] = var.getValueReference();
      boolResultVars.id[jBoolVars] = i;
      jBoolVars++;
    }
  }

  return oms_status_ok;
}
