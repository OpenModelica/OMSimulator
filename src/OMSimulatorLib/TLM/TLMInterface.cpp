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

#include "TLMInterface.h"
#include "Logging.h"

oms2::TLMInterface::TLMInterface(const oms2::ComRef &cref,
                                 const std::string name,
                                 oms_causality_enu_t causality,
                                 const std::string domain,
                                 const int dimensions, oms_tlm_interpolation_t interpolation, const std::vector<SignalRef> &sigrefs)
  : sig(cref, name)
{
  this->cref = cref;
  this->name = name;
  this->causality = causality;
  this->domain = domain;
  this->dimensions = dimensions;
  this->sigrefs = sigrefs;
  this->interpolationMethod = interpolation;
}

oms2::ComRef oms2::TLMInterface::getFMUName()
{
  return sigrefs[0].getCref();
}

std::vector<oms2::SignalRef> oms2::TLMInterface::getSubSignalSet(std::vector<int> ids)
{
  std::vector<oms2::SignalRef> retval;
  for(int i : ids) {
    retval.push_back(sigrefs[i]);
  }
  return retval;
}

oms_status_enu_t oms2::TLMInterface::doRegister(TLMPlugin *plugin)
{

  //OMTLMSimulator uses degrees of freedom as "dimensions",
  //so convert to this:
  int omtlm_dimensions = dimensions;

  if(dimensions == 2) omtlm_dimensions = 3;
  if(dimensions == 3) omtlm_dimensions = 6;

  std::string omtlm_causality = "Bidirectional";

  if(causality == oms_causality_output) omtlm_causality = "Output";
  else if(causality == oms_causality_input)  omtlm_causality = "Input";

  this->id = plugin->RegisteTLMInterface(name,omtlm_dimensions,omtlm_causality,domain);
  if(this->id < 0) {
    logError("Failed to register TLM interface: "+name);
    return oms_status_error;
  }

  return oms_status_ok;
}
