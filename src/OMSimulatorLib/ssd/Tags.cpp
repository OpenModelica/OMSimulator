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
const char* oms::simulation_information                 = "oms:SimulationInformation"; // for version 1.0 ssd:SimulationInformation should be handled in vendorSpecific annotation

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
const char* oms::ssd::ssd_annotation                   = "ssd:Annotation";
const char* oms::ssd::ssd_annotations                  = "ssd:Annotations";
const char* oms::ssd::ssd_component                    = "ssd:Component";
const char* oms::ssd::ssd_connection                   = "ssd:Connection";
const char* oms::ssd::ssd_connection_geometry          = "ssd:ConnectionGeometry";
const char* oms::ssd::ssd_connections                  = "ssd:Connections";
const char* oms::ssd::ssd_connector                    = "ssd:Connector";
const char* oms::ssd::ssd_connector_geometry           = "ssd:ConnectorGeometry";
const char* oms::ssd::ssd_connectors                   = "ssd:Connectors";
const char* oms::ssd::ssd_default_experiment           = "ssd:DefaultExperiment";
const char* oms::ssd::ssd_element_geometry             = "ssd:ElementGeometry";
const char* oms::ssd::ssd_elements                     = "ssd:Elements";
const char* oms::ssd::ssd_enumerations                 = "ssd:Enumerations";
const char* oms::ssd::ssd_simulation_information       = "ssd:SimulationInformation"; // for version "Draft2018021"
const char* oms::ssd::ssd_system                       = "ssd:System";
const char* oms::ssd::ssd_system_structure_description = "ssd:SystemStructureDescription";
const char* oms::ssd::ssd_units                        = "ssd:Units";
