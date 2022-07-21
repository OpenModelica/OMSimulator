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

#ifndef _OMS_EXTERNAL_MODEL_H_
#define _OMS_EXTERNAL_MODEL_H_

#include "Component.h"
#include "ComRef.h"
#include "Element.h"
#include "ExternalModelInfo.h"
#include "Option.h"
#include "ResultWriter.h"
#include "Snapshot.h"
#include "Types.h"
#include "Variable.h"
#include <vector>
#include <map>

namespace oms
{
  class ExternalModel : public Component
  {
  public:
    ~ExternalModel();
    static ExternalModel* NewComponent(const oms::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript);

    const ExternalModelInfo *getExternalModelInfo() const {return &(this->externalModelInfo);}

    Variable* getVariable(const ComRef& cref) {logError_NotImplemented; return NULL;}
    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    const std::string getStartScript() const {return externalModelInfo.getStartScript();}
    const std::map<std::string, oms::Option<double>>& getRealParameters() const {return realParameters;}

    oms_status_enu_t exportToSSD(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const;
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();

    oms_status_enu_t updateDependencyGraphs() {return oms_status_error;}

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultWriter);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    void getFilteredSignals(std::vector<Connector>& filteredSignals) const {}

  protected:
    ExternalModel(const oms::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript);

    // stop the compiler generating methods copying the object
    ExternalModel(ExternalModel const& copy);            ///< not implemented
    ExternalModel& operator=(ExternalModel const& copy); ///< not implemented

  private:
    ExternalModelInfo externalModelInfo;
    std::map<std::string, oms::Option<double>> realParameters;
  };
}

#endif
