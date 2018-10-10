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

#ifndef _OMS2_TABLE_H_
#define _OMS2_TABLE_H_

#include "ComRef.h"
#include "FMISubModel.h"
#include "FMUInfo.h"
#include "Logging.h"
#include "Option.h"
#include "ResultReader.h"
#include "ResultWriter.h"
#include "Variable.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <pugixml.hpp>

namespace oms2
{
  class Table : public FMISubModel
  {
  public:
    static Table* newSubModel(const ComRef& cref, const std::string& filename);

    oms_status_enu_t enterInitialization(const double time);
    oms_status_enu_t exitInitialization();
    oms_status_enu_t doStep(double stopTime);

    oms_status_enu_t reset(bool terminate) {return oms_status_ok;}

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    oms_element_type_enu_t getType() const { return oms_component_table_old; }

    const std::string getPath() const {return path;}

    oms_status_enu_t getReal(const std::string& var, double& realValue, const double time);

    oms2::Variable* getVariable(const std::string& signal);

    oms_status_enu_t setReal(const oms2::SignalRef& sr, double value);
    oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value);
    oms_status_enu_t setRealInputDerivatives(const SignalRef &sr, int order, double value);
    oms_status_enu_t setInteger(const oms2::SignalRef& sr, int integerValue);
    oms_status_enu_t getInteger(const oms2::SignalRef& sr, int& integerValue);
    oms_status_enu_t setBoolean(const oms2::SignalRef& sr, bool booleanValue);
    oms_status_enu_t getBoolean(const oms2::SignalRef& sr, bool& booleanValue);

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter) {return oms_status_ok;}
    oms_status_enu_t emit(ResultWriter& resultWriter) {return oms_status_ok;}

    void addSignalsToResults(const std::string& regex) {logWarning("addSignalsToResults not implemented for tables");}
    void removeSignalsFromResults(const std::string& regex) {logWarning("removeSignalsFromResults not implemented for tables");}

  private:
    Table(const ComRef& cref, const std::string& filename);
    ~Table();

    std::string path;
    std::vector<oms2::Variable> outputs;
    ResultReader *resultReader;
    std::unordered_map<std::string, ResultReader::Series*> series;
    double time;
  };
}

#endif
