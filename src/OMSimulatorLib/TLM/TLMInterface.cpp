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
                                 const ComRef name,
                                 oms_causality_enu_t causality,
                                 const std::string domain,
                                 const int dimensions, oms_tlm_interpolation_t interpolation, const std::vector<SignalRef> &sigrefs)
  : sig(cref, name.toString())
{
  this->cref = cref;
  this->name = name;
  this->causality = causality;
  this->domain = domain;
  this->dimensions = dimensions;
  this->sigrefs = sigrefs;
  this->interpolationMethod = interpolation;
}

oms_status_enu_t oms2::TLMInterface::exportToSSD(pugi::xml_node &root) const
{
  pugi::xml_node nodeBus = root.append_child("OMSimulator:Bus");
  nodeBus.append_attribute("name") = name.toString().c_str();
  nodeBus.append_attribute("type") = "tlm";
  nodeBus.append_attribute("domain") = tlmDomainToString(domain,dimensions,causality).c_str();
  nodeBus.append_attribute("interpolation") = tlmInterpolationToString(interpolationMethod).c_str();
  pugi::xml_node nodeSignals = nodeBus.append_child("Signals");

  if(dimensions == 1 && causality != oms_causality_bidir) {
    oms_tlm_sigrefs_signal_t tlmrefs;
    pugi::xml_node ssd_Signal = nodeSignals.append_child("Signal");
    ssd_Signal.append_attribute("name") = sigrefs[tlmrefs.y].getVar().c_str();
    ssd_Signal.append_attribute("type") = "value";
  }
  else if(causality == oms_causality_bidir) {
    std::vector<std::string> names, tlmvars;
    if(dimensions == 1 && interpolationMethod == oms_tlm_no_interpolation) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state", "flow", "effort" };
    }
    else if(dimensions == 1 && interpolationMethod == oms_tlm_coarse_grained) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state", "flow", "wave", "impedance" };
    }
    else if(dimensions == 1 && interpolationMethod == oms_tlm_fine_grained) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state", "flow",
                  "wave1", "wave2", "wave3", "wave4", "wave5",
                  "wave6", "wave7", "wave8", "wave9", "wave10",
                  "time1", "time2", "time3", "time4", "time5",
                  "time6", "time7", "time8", "time9", "time10",
                  "impedance" };
    }
    else if(dimensions == 3 && interpolationMethod == oms_tlm_no_interpolation) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state1", "state2", "state3"
                  "A11",    "A12",    "A13",
                  "A21",    "A22",    "A23",
                  "A31",    "A32",    "A33",
                  "flow1",  "flow2",  "flow3",
                  "flow4",  "flow5",  "flow6",
                  "effort1","effort2","effort3",
                  "effort4","effort3","effort4"
                };
    }
    else if(dimensions == 3 && interpolationMethod == oms_tlm_coarse_grained) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state1", "state2", "state3"
                  "A1_1",   "A1_2",   "A1_3",
                  "A2_1",   "A2_2",   "A2_3",
                  "A3_1",   "A3_2",   "A3_3",
                  "flow1",  "flow2",  "flow3",
                  "flow4",  "flow5",  "flow6",
                  "wave1",  "wave2",  "wave3",
                  "wave4",  "wave3",  "wave4",
                  "impedance_t","impedance_r"
                };
    }
    else if(dimensions == 3 && interpolationMethod == oms_tlm_fine_grained) {
      for(const SignalRef &sigref : sigrefs) {
        names.push_back(sigref.getVar());
      }
      tlmvars = { "state1",   "state2",   "state3"
                  "A1_1",     "A1_2",     "A1_3",
                  "A2_1",     "A2_2",     "A2_3",
                  "A3_1",     "A3_2",     "A3_3",
                  "flow1",    "flow2",    "flow3",
                  "flow4",    "flow5",    "flow6",
                  "wave1_1",  "wave2_1",  "wave3_1",  "wave4_1",  "wave5_1",  "wave6_1",
                  "wave1_2",  "wave2_2",  "wave3_2",  "wave4_2",  "wave5_2",  "wave6_2",
                  "wave1_3",  "wave2_3",  "wave3_3",  "wave4_3",  "wave5_3",  "wave6_3",
                  "wave1_4",  "wave2_4",  "wave3_4",  "wave4_4",  "wave5_4",  "wave6_4",
                  "wave1_5",  "wave2_5",  "wave3_5",  "wave4_5",  "wave5_5",  "wave6_5",
                  "wave1_6",  "wave2_6",  "wave3_6",  "wave4_6",  "wave5_6",  "wave6_6",
                  "wave1_7",  "wave2_7",  "wave3_7",  "wave4_7",  "wave5_7",  "wave6_7",
                  "wave1_8",  "wave2_8",  "wave3_8",  "wave4_8",  "wave5_8",  "wave6_8",
                  "wave1_9",  "wave2_9",  "wave3_9",  "wave4_9",  "wave5_9",  "wave6_9",
                  "wave1_10", "wave2_10", "wave3_10", "wave4_10", "wave5_10", "wave6_10",
                  "time1",    "time2",    "time3",    "time4",    "time5",
                  "time6",    "time7",    "time8",    "time9",    "time10",
                  "impedance_t","impedance_r"
                };
    }

    for(size_t i=0; i<names.size(); ++i) {
      //Sub-connector
      pugi::xml_node ssd_SubConnector = nodeSignals.append_child("Signal");
      ssd_SubConnector.append_attribute("name") = names[i].c_str();
      ssd_SubConnector.append_attribute("type") = tlmvars[i].c_str();
    }
  }

  return oms_status_ok;
}


std::string oms2::TLMInterface::tlmDomainToString(std::string domain, int dimensions, oms_causality_enu_t causality) const
{
  if(domain == "Signal" && causality == oms_causality_input) {
    return "Signal_in";
  }
  else if(domain == "Signal" && causality == oms_causality_output) {
    return "Signal_out";
  }
  else if(domain != "Signal") {
    return domain+"_"+std::to_string(dimensions)+"D";
  }
  else {
    return "Undefined";
  }
}

std::string oms2::TLMInterface::tlmInterpolationToString(oms_tlm_interpolation_t method) const {
  switch(method) {
  case oms_tlm_no_interpolation:
    return "none";
  case oms_tlm_coarse_grained:
    return "coarsegrained";
  case oms_tlm_fine_grained:
    return "finegrained";
  }
  return "undefined";
}

oms2::ComRef oms2::TLMInterface::getFMUName() const
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

  this->id = plugin->RegisteTLMInterface(name.toString(),omtlm_dimensions,omtlm_causality,domain);
  if(this->id < 0) {
    logError("Failed to register TLM interface: "+name.toString());
    return oms_status_error;
  }

  return oms_status_ok;
}
