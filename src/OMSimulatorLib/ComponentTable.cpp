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

#include "ComponentTable.h"

#include "Logging.h"
#include "Model.h"
#include "ssd/Tags.h"
#include "System.h"
#include <OMSBoost.h>
#include <RegEx.h>

oms::ComponentTable::ComponentTable(const ComRef& cref, System* parentSystem, const std::string& path)
  : oms::Component(cref, oms_component_table, parentSystem, path), resultReader(NULL)
{
}

oms::ComponentTable::~ComponentTable()
{
  for (auto it=series.begin(); it != series.end(); it++)
    ResultReader::deleteSeries(&it->second);
  series.clear();

  if (resultReader)
    delete resultReader;
}

oms::Component* oms::ComponentTable::NewComponent(const oms::ComRef& cref, oms::System* parentSystem, const std::string& path)
{
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if (!parentSystem)
  {
    logError_InternalError;
    return NULL;
  }

  std::string extension = "";
  if (path.length() > 4)
    extension = path.substr(path.length() - 4);

  boost::filesystem::path temp_root(parentSystem->getModel()->getTempDirectory());
  boost::filesystem::path relPath = boost::filesystem::path("resources") / (std::string(cref) + extension);
  boost::filesystem::path absPath = temp_root / relPath;

  ComponentTable* component = new ComponentTable(cref, parentSystem, "resources/" + std::string(cref) + extension);

  if (parentSystem->copyResources())
    boost::filesystem::copy_file(boost::filesystem::path(path), absPath, boost::filesystem::copy_option::overwrite_if_exists);

  component->resultReader = ResultReader::newReader(path.c_str());
  if (!component->resultReader)
  {
    logError("Could not load lookup table: " + path);
    delete component;
    return NULL;
  }

  int i = 1;
  int size = 1 + component->resultReader->getAllSignals().size();
  for (auto const &signal : component->resultReader->getAllSignals())
  {
    component->exportSeries[component->getFullCref() + ComRef(signal)] = true;
    component->connectors.back() = new Connector(oms_causality_output, oms_signal_type_real, ComRef(signal), i++/(double)size);
    component->connectors.push_back(NULL);
  }
  component->element.setConnectors(&component->connectors[0]);

  return component;
}

oms::Component* oms::ComponentTable::NewComponent(const pugi::xml_node& node, oms::System* parentSystem)
{
  ComRef cref = ComRef(node.attribute("name").as_string());
  std::string type = node.attribute("type").as_string();
  std::string source = node.attribute("source").as_string();

  if (type != "application/table")
  {
    logError("Unexpected component type: " + type);
    return NULL;
  }

  oms::ComponentTable* component = dynamic_cast<oms::ComponentTable*>(oms::ComponentTable::NewComponent(cref, parentSystem, source));
  if (!component)
    return NULL;

  // delete connectors
  for (const auto& connector : component->connectors)
    if (connector)
      delete connector;
  component->connectors.clear();

  for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
  {
    std::string name = it->name();
    if(name == oms::ssd::ssd_connectors)
    {
      // import connectors
      for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
        component->connectors.push_back(oms::Connector::NewConnector(*itConnectors));
    }
    else if(name == oms::ssd::ssd_element_geometry)
    {
      oms::ssd::ElementGeometry geometry;
      geometry.importFromSSD(*it);
      component->setGeometry(geometry);
    }
    else
    {
      logError_WrongSchema(name);
      delete component;
      return NULL;
    }
  }

  component->connectors.push_back(NULL);
  component->element.setConnectors(&component->connectors[0]);

  return component;
}

oms_status_enu_t oms::ComponentTable::exportToSSD(pugi::xml_node& node) const
{
  node.append_attribute("name") = this->getCref().c_str();
  node.append_attribute("type") = "application/table";
  node.append_attribute("source") = getPath().c_str();
  pugi::xml_node node_connectors = node.append_child(oms::ssd::ssd_connectors);

  if (element.getGeometry())
    element.getGeometry()->exportToSSD(node);

  for (const auto& connector : connectors)
    if (connector)
      if (oms_status_ok != connector->exportToSSD(node_connectors))
        return oms_status_error;
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::instantiate()
{
  time = getModel()->getStartTime();
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::reset()
{
  time = getModel()->getStartTime();
  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::getReal(const oms::ComRef& cref, double& value)
{
  if (!resultReader)
    logError("the table isn't initialized properly");

  if (series.find(cref) == series.end())
    series[cref] = resultReader->getSeries(cref.c_str());

  for (int i=1; i<series[cref]->length; ++i)
  {
    if (series[cref]->time[i-1] == time)
    {
      value = series[cref]->value[i-1];
      return oms_status_ok;
    }
    else if (series[cref]->time[i-1] <= time && series[cref]->time[i] >= time)
    {
      double m = (series[cref]->value[i] - series[cref]->value[i-1]) / (series[cref]->time[i] - series[cref]->time[i-1]);
      value = series[cref]->value[i-1] + (time - series[cref]->time[i-1]) * m;
      return oms_status_ok;
    }
  }

  logError("out of range (cref=" + std::string(cref) + ", time=" + std::to_string(time) + ")");
  value = 0.0;
  return oms_status_error;
}

oms_status_enu_t oms::ComponentTable::registerSignalsForResultFile(ResultWriter& resultFile)
{
  resultFileMapping.clear();

  for (unsigned int i=0; i<resultReader->getAllSignals().size(); ++i)
  {
    if (!exportSeries[ComRef(resultReader->getAllSignals()[i])])
      continue;

    std::string name = std::string(getFullCref()) + "." + resultReader->getAllSignals()[i];

    unsigned int ID = resultFile.addSignal(name, "lookup table", SignalType_REAL);
    resultFileMapping[ID] = i;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::updateSignals(ResultWriter& resultWriter)
{
  for (auto const &it : resultFileMapping)
  {
    unsigned int ID = it.first;
    const std::string& var = resultReader->getAllSignals()[it.second];
    SignalValue_t value;

    if (oms_status_ok != getReal(var, value.realValue))
      return logError("failed to fetch variable " + std::string(getFullCref()) + "." + std::string(var));
    resultWriter.updateSignal(ID, value);
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::addSignalsToResults(const char* regex)
{
  oms_regex exp(regex);
  for (auto& x: exportSeries)
  {
    if (x.second)
      continue;

    if (regex_match(std::string(x.first), exp))
    {
      //logInfo("added \"" + std::string(x.first) + "\" to results");
      x.second = true;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::ComponentTable::removeSignalsFromResults(const char* regex)
{
  oms_regex exp(regex);
  for (auto& x: exportSeries)
  {
    if (!x.second)
      continue;

    if (regex_match(std::string(x.first), exp))
    {
      //logInfo("removed \"" + std::string(x.first) + "\" from results");
      x.second = false;
    }
  }

  return oms_status_ok;
}
