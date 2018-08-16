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

#include "Table.h"

#include "Connector.h"
#include "Logging.h"
#include "Option.h"
#include "Scope.h"
#include "SignalRef.h"
#include "ssd/Tags.h"
#include "Variable.h"

#include <boost/filesystem.hpp>

#include <unordered_map>

oms2::Table::Table(const oms2::ComRef& cref, const std::string& filename)
  : oms2::FMISubModel(oms_component_table, cref), path(filename), resultReader(NULL)
{
}

oms2::Table::~Table()
{
  for (auto it=series.begin(); it != series.end(); it++)
    ResultReader::deleteSeries(&it->second);
  series.clear();

  if (resultReader)
    delete resultReader;
}

oms2::Table* oms2::Table::newSubModel(const oms2::ComRef& cref, const std::string& filename)
{
  if (!cref.isValidQualified())
  {
    logError("\"" + cref + "\" is not a valid model name.");
    return NULL;
  }

  if (!boost::filesystem::exists(filename))
  {
    logError("Specified file name does not exist: \"" + filename + "\"");
    return NULL;
  }

  oms2::Table *model = new oms2::Table(cref, filename);

  model->resultReader = ResultReader::newReader(filename.c_str());
  if (!model->resultReader)
  {
    logError("Could not load lookup table: " + filename);
    delete model;
    return NULL;
  }

  for (auto &v : model->resultReader->getAllSignals())
  {
    oms2::Variable var(oms2::SignalRef(model->getName(), v), fmi2_causality_enu_output, fmi2_initial_enu_exact, false, oms_signal_type_real);
    model->outputs.push_back(var);
  }
  return model;
}

oms_status_enu_t oms2::Table::enterInitialization(const double time)
{
  this->time = time;
  return oms_status_ok;
}

oms_status_enu_t oms2::Table::exitInitialization()
{
  return oms_status_ok;
}

oms_status_enu_t oms2::Table::doStep(double stopTime)
{
  this->time = stopTime;
  return oms_status_ok;
}

oms_status_enu_t oms2::Table::exportToSSD(pugi::xml_node& root) const
{
  oms_status_enu_t status = oms_status_ok;

  oms2::ComRef cref = element.getName();
  pugi::xml_node subModel = root.append_child(oms2::ssd::ssd_component);
  subModel.append_attribute("name") = cref.last().toString().c_str();

  subModel.append_attribute("type") = "application/x-table";

  const std::string& path = getPath();
  subModel.append_attribute("source") = path.c_str();

  // export ssd:ElementGeometry
  status = element.getGeometry()->exportToSSD(subModel);
  if (oms_status_ok != status)
    return status;

  // export ssd:Connectors
  oms2::Connector** connectors = element.getConnectors();
  if (connectors)
  {
    pugi::xml_node connectorsNode = subModel.append_child(oms2::ssd::ssd_connectors);
    for (int i=0; connectors[i]; ++i)
    {
      status = connectors[i]->exportToSSD(connectorsNode);
      if (oms_status_ok != status)
        return status;
    }
  }
  return status;
}

oms2::Variable* oms2::Table::getVariable(const std::string& var)
{
  for (auto &v : outputs)
    if (v.getName() == var)
      return &v;

  return NULL;
}

oms_status_enu_t oms2::Table::getReal(const std::string& var, double& realValue, const double time)
{
  logTrace();
  if (!resultReader)
    logError("oms2::Table::getReal: This table isn't initialized properly.");

  if (series.find(var) == series.end())
    series[var] = resultReader->getSeries(var.c_str());

  for (int i=1; i<series[var]->length; ++i)
  {
    if (series[var]->time[i-1] == time)
    {
      realValue = series[var]->value[i-1];
      return oms_status_ok;
    }
    else if (series[var]->time[i-1] <= time && series[var]->time[i] >= time)
    {
      double m = (series[var]->value[i] - series[var]->value[i-1]) / (series[var]->time[i] - series[var]->time[i-1]);
      realValue = series[var]->value[i-1] + (time - series[var]->time[i-1]) * m;
      return oms_status_ok;
    }
  }

  logError("oms2::Table::getReal: Time out of range.");
  realValue = 0.0;
  return oms_status_error;
}

oms_status_enu_t oms2::Table::setReal(const oms2::SignalRef& sr, double value)
{
  return logError("[oms2::Table::setReal] failed");
}

oms_status_enu_t oms2::Table::getReal(const oms2::SignalRef& sr, double& value)
{
  return getReal(sr.getVar(), value, time);
}

oms_status_enu_t oms2::Table::setRealInputDerivatives(const oms2::SignalRef &sr, int order, double value)
{
  return logError("[oms2::Table::setRealInputDerivatives] failed");
}

oms_status_enu_t oms2::Table::setInteger(const oms2::SignalRef& sr, int integerValue)
{
  return logError("[oms2::Table::setInteger] failed (not (yet) implemented)");
}

oms_status_enu_t oms2::Table::getInteger(const oms2::SignalRef& sr, int& integerValue)
{

  return logError("[oms2::Table::getInteger] failed (not (yet) implemented)");
}

oms_status_enu_t oms2::Table::setBoolean(const oms2::SignalRef& sr, bool booleanValue)
{
  return logError("[oms2::Table::setInteger] failed (not (yet) implemented)");
}

oms_status_enu_t oms2::Table::getBoolean(const oms2::SignalRef& sr, bool& booleanValue)
{
  return logError("[oms2::Table::getInteger] failed (not (yet) implemented)");
}
