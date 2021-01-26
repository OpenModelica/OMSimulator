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

#include "Tags.h"

const char* oms::ssp::Draft20180219::annotation_type                   = "org.openmodelica";
const char* oms::ssp::Draft20180219::tlm_master                        = "oms:TlmMaster";
const char* oms::ssp::Draft20180219::bus                               = "oms:Bus";
const char* oms::ssp::Draft20180219::signals                           = "oms:Signals";
const char* oms::ssp::Draft20180219::signal                            = "oms:Signal";
const char* oms::ssp::Draft20180219::bus_connections                   = "oms:Connections";
const char* oms::ssp::Draft20180219::bus_connection                    = "oms:Connection";
const char* oms::ssp::Draft20180219::external_model                    = "oms:ExternalModel";

const char* oms::ssp::Draft20180219::ssd::annotation                   = "ssd:Annotation";
const char* oms::ssp::Draft20180219::ssd::annotations                  = "ssd:Annotations";
const char* oms::ssp::Draft20180219::ssd::component                    = "ssd:Component";
const char* oms::ssp::Draft20180219::ssd::connection                   = "ssd:Connection";
const char* oms::ssp::Draft20180219::ssd::connection_geometry          = "ssd:ConnectionGeometry";
const char* oms::ssp::Draft20180219::ssd::connections                  = "ssd:Connections";
const char* oms::ssp::Draft20180219::ssd::connector                    = "ssd:Connector";
const char* oms::ssp::Draft20180219::ssd::connector_geometry           = "ssd:ConnectorGeometry";
const char* oms::ssp::Draft20180219::ssd::connectors                   = "ssd:Connectors";
const char* oms::ssp::Draft20180219::ssd::default_experiment           = "ssd:DefaultExperiment";
const char* oms::ssp::Draft20180219::ssd::element_geometry             = "ssd:ElementGeometry";
const char* oms::ssp::Draft20180219::ssd::elements                     = "ssd:Elements";
const char* oms::ssp::Draft20180219::ssd::enumerations                 = "ssd:Enumerations";
const char* oms::ssp::Draft20180219::ssd::simulation_information       = "ssd:SimulationInformation";
const char* oms::ssp::Draft20180219::ssd::system                       = "ssd:System";
const char* oms::ssp::Draft20180219::ssd::system_structure_description = "ssd:SystemStructureDescription";
const char* oms::ssp::Draft20180219::ssd::units                        = "ssd:Units";

// version 1.0
const char* oms::ssp::Version1_0::simulation_information               = "oms:SimulationInformation"; // simulation information must be handled in a vendor specific annotation
const char* oms::ssp::Version1_0::FixedStepMaster                      = "oms:FixedStepMaster"; // WC-System
const char* oms::ssp::Version1_0::VariableStepSolver                   = "oms:VariableStepSolver"; // SC-System
const char* oms::ssp::Version1_0::oms_annotations                      = "oms:Annotations"; // root node for all oms_annotations
const char* oms::ssp::Version1_0::oms_buses                            = "oms:Buses";
const char* oms::ssp::Version1_0::oms_signalFilter                     = "oms:SignalFilter";
const char* oms::ssp::Version1_0::oms_Variable                         = "oms:Variable";

const char* oms::ssp::Version1_0::snap_shot                            = "oms:snapshot";
const char* oms::ssp::Version1_0::ssd_file                             = "oms:ssd_file";
const char* oms::ssp::Version1_0::ssv_file                             = "oms:ssv_file";
const char* oms::ssp::Version1_0::ssm_file                             = "oms:ssm_file";

const char* oms::ssp::Version1_0::ssd::parameter_bindings              = "ssd:ParameterBindings";
const char* oms::ssp::Version1_0::ssd::parameter_binding               = "ssd:ParameterBinding";
const char* oms::ssp::Version1_0::ssd::parameter_values                = "ssd:ParameterValues";
const char* oms::ssp::Version1_0::ssd::parameter_mapping               = "ssd:ParameterMapping";

const char* oms::ssp::Version1_0::ssv::parameter_set                   = "ssv:ParameterSet";
const char* oms::ssp::Version1_0::ssv::parameters                      = "ssv:Parameters";
const char* oms::ssp::Version1_0::ssv::parameter                       = "ssv:Parameter";

const char* oms::ssp::Version1_0::ssc::real_type                       = "ssc:Real";
const char* oms::ssp::Version1_0::ssc::integer_type                    = "ssc:Integer";
const char* oms::ssp::Version1_0::ssc::boolean_type                    = "ssc:Boolean";
const char* oms::ssp::Version1_0::ssc::string_type                     = "ssc:String";
const char* oms::ssp::Version1_0::ssc::enumeration_type                = "ssc:Enumeration";
const char* oms::ssp::Version1_0::ssc::binary_type                     = "ssc:Binary";

const char* oms::ssp::Version1_0::ssv::real_type                       = "ssv:Real";
const char* oms::ssp::Version1_0::ssv::integer_type                    = "ssv:Integer";
const char* oms::ssp::Version1_0::ssv::boolean_type                    = "ssv:Boolean";
const char* oms::ssp::Version1_0::ssv::string_type                     = "ssv:String";
const char* oms::ssp::Version1_0::ssv::enumeration_type                = "ssv:Enumeration";
const char* oms::ssp::Version1_0::ssv::binary_type                     = "ssv:Binary";

const char* oms::ssp::Version1_0::ssc::annotation                      = "ssc:Annotation";

const char* oms::ssp::Version1_0::ssm::parameter_mapping               = "ssm:ParameterMapping";
const char* oms::ssp::Version1_0::ssm::parameter_mapping_entry         = "ssm:MappingEntry";
