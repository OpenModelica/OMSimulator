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

#include "ExternalModel.h"
#include "Logging.h"
#include "Element.h"
#include "ExternalModelInfo.h"
#include "Types.h"
#include "ssd/Tags.h"

#include <cstring>
#include <map>
#include <iostream>

oms::ExternalModel::ExternalModel(const oms::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript)
  : oms::Component(cref, oms_component_external, parentSystem, path), externalModelInfo(path, startscript)
{
}

oms::ExternalModel::~ExternalModel()
{
}

oms::ExternalModel* oms::ExternalModel::NewComponent(const oms::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  oms::ExternalModel* model = new oms::ExternalModel(cref, parentSystem, path, startscript);
  return model;
}

oms_status_enu_t oms::ExternalModel::setRealParameter(const std::string &var, double value)
{
  std::map<std::string, oms::Option<double>>::iterator it;
  it = realParameters.find(var);

  if(it != realParameters.end()) {
    it->second.setValue(value);
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::ExternalModel::getRealParameter(const std::string &var, double &value)
{
  std::map<std::string, oms::Option<double>>::iterator it;
  it = realParameters.find(var);

  if(it != realParameters.end()) {
    value = it->second.getValue();
    return oms_status_ok;
  }

  return oms_status_error;
}

oms_status_enu_t oms::ExternalModel::exportToSSD(pugi::xml_node& node, pugi::xml_node& ssvNode) const
{
  pugi::xml_node annotations_node = node.append_child(oms::ssp::Draft20180219::ssd::annotations);
  pugi::xml_node annotation_node = annotations_node.append_child(oms::ssp::Version1_0::ssc::annotation);
  annotation_node.append_attribute("type") = oms::ssp::Draft20180219::annotation_type;
  pugi::xml_node oms_annotation_node = annotation_node.append_child(oms::ssp::Version1_0::oms_annotations);

  if (tlmbusconnectors[0])
  {
    pugi::xml_node oms_buses_node = oms_annotation_node.append_child(oms::ssp::Version1_0::oms_buses);
    for (const auto& tlmbusconnector : tlmbusconnectors)
      if (tlmbusconnector)
        tlmbusconnector->exportToSSD(oms_buses_node);
  }

  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("source") = this->getPath().c_str();
  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node siminfo_node = oms_annotation_node.append_child(oms::ssp::Version1_0::simulation_information);

  pugi::xml_node externalmodel_node = siminfo_node.append_child(oms::ssp::Draft20180219::external_model);
  externalmodel_node.append_attribute("startscript") = externalModelInfo.getStartScript().c_str();

  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::instantiate()
{
  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::initialize()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms::ExternalModel::terminate()
{
  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::reset()
{
  return logError_NotImplemented;
}

oms_status_enu_t oms::ExternalModel::registerSignalsForResultFile(ResultWriter& resultFile)
{
  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::updateSignals(ResultWriter& resultWriter)
{
  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::addSignalsToResults(const char* regex)
{
  //return logError_NotImplemented;
  return oms_status_ok;
}

oms_status_enu_t oms::ExternalModel::removeSignalsFromResults(const char* regex)
{
  //return logError_NotImplemented;
  return oms_status_ok;
}
