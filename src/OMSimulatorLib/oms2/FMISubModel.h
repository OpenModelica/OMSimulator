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

#ifndef _OMS2_FMI_SUB_MODEL_H_
#define _OMS2_FMI_SUB_MODEL_H_

#include "../Types.h"
#include "ComRef.h"
#include "DirectedGraph.h"
#include "Element.h"
#include "ssd/ElementGeometry.h"
#include "Variable.h"

class ResultWriter;

namespace oms2
{
  class FMISubModel
  {
  public:
    virtual oms_status_enu_t exportToSSD(pugi::xml_node& root) const = 0;

    oms_element_type_enu_t getType() {return element.getType();}
    static void deleteSubModel(FMISubModel *model) {if (model) delete model;}

    void setName(const ComRef& name) {element.setName(name);}
    void setGeometry(const oms2::ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}

    const ComRef getName() const {return oms2::ComRef(element.getName());}
    const oms2::ssd::ElementGeometry* getGeometry() {return element.getGeometry();}
    oms2::Element* getElement() {return &element;}

    virtual oms2::Variable* getVariable(const std::string& signal) = 0;

    virtual oms_status_enu_t enterInitialization(const double time) = 0;
    virtual oms_status_enu_t exitInitialization() = 0;

    virtual oms_status_enu_t reset() = 0;
    virtual oms_status_enu_t terminate() = 0;
    virtual oms_status_enu_t doStep(double stopTime) = 0;

    const DirectedGraph& getInitialUnknownsGraph() const {return initialUnknownsGraph;}
    const DirectedGraph& getOutputsGraph() const {return outputsGraph;}

    virtual oms_status_enu_t setReal(const oms2::SignalRef& sr, double value) = 0;
    virtual oms_status_enu_t getReal(const oms2::SignalRef& sr, double& value) = 0;
    virtual oms_status_enu_t setRealInputDerivatives(const oms2::SignalRef& sr, int order, double value) = 0;

    virtual oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter) = 0;
    virtual oms_status_enu_t emit(ResultWriter& resultWriter) = 0;

  protected:
    FMISubModel(oms_element_type_enu_t type, const ComRef& cref);
    virtual ~FMISubModel();

  private:
    // stop the compiler generating methods copying the object
    FMISubModel(FMISubModel const& copy);            ///< not implemented
    FMISubModel& operator=(FMISubModel const& copy); ///< not implemented

  protected:
    oms2::Element element;

    DirectedGraph initialUnknownsGraph;
    DirectedGraph outputsGraph;
  };
}

#endif
