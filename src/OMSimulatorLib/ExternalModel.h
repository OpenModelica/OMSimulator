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

#ifndef _OMS_EXTERNAL_MODEL_H_
#define _OMS_EXTERNAL_MODEL_H_

#include "Component.h"
#include "ComRef.h"
#include "Element.h"
#include "Option.h"
#include "Types.h"
#include "Variable.h"
#include <vector>
#include <map>

namespace oms3
{
  class ExternalModel : public Component
  {
  public:
    ~ExternalModel();
    static ExternalModel* NewComponent(const oms3::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript);

    oms_status_enu_t addTLMBus(const oms3::ComRef& cref, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
    oms3::TLMBusConnector *getTLMBusConnector(const oms3::ComRef &cref);
    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    const std::string& getStartScript() const {return startscript;}
    const std::map<std::string, oms3::Option<double>>& getRealParameters() const {return realParameters;}

    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();

  protected:
    ExternalModel(const oms3::ComRef& cref, System* parentSystem, const std::string& path, const std::string& startscript);

    // stop the compiler generating methods copying the object
    ExternalModel(ExternalModel const& copy);            ///< not implemented
    ExternalModel& operator=(ExternalModel const& copy); ///< not implemented

  private:
    std::string startscript;
    std::map<std::string, oms3::Option<double>> realParameters;
    std::vector<oms3::TLMBusConnector*> tlmbusconnectors;
  };
}

namespace oms2
{
  class ExternalModel
  {
  public:
    ExternalModel(const ComRef& cref,
                  const std::string& filename,
                  const std::string& startScript);

    oms_element_type_enu_t getType() const {return oms_component_external_old;}


    void setName(const ComRef& name) {element.setName(name);}
    void setGeometry(const oms2::ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}

    const ComRef getName() const {return oms2::ComRef(element.getName());}
    const oms2::ssd::ElementGeometry* getGeometry() {return element.getGeometry();}
    oms2::Element* getElement() {return &element;}

    oms_status_enu_t setRealParameter(const std::string& var, double value);
    oms_status_enu_t getRealParameter(const std::string& var, double& value);
    const std::string& getModelPath() const {return filename;}
    const std::string& getStartScript() const {return startScript;}
    const std::map<std::string, oms3::Option<double>>& getRealParameters() const {return realParameters;}

  protected:
    ComRef cref;
    oms2::Element element;

  private:

    std::string filename;
    std::string startScript;

    std::map<std::string, oms3::Option<double>> realParameters;
  };
}

#endif
