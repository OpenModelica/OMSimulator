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

#ifndef _OMS2_COMPOSITE_MODEL_H_
#define _OMS2_COMPOSITE_MODEL_H_

#include "../Types.h"
#include "ComRef.h"
#include "Element.h"
#include "ssd/ElementGeometry.h"
#include "ssd/SystemGeometry.h"

#include <string>

class ResultWriter;

namespace oms2
{
  class CompositeModel
  {
  public:
    virtual oms_element_type_enu_t getType() = 0;

    static void DeleteModel(CompositeModel *model) {if (model) delete model;}

    const ComRef getName() const {return oms2::ComRef(element.getName());}
    const oms2::ssd::ElementGeometry* getGeometry() {return element.getGeometry();}
    oms2::Element* getElement() {return &element;}

    void setName(const ComRef& name) {element.setName(name);}
    void setGeometry(const oms2::ssd::ElementGeometry& geometry) {element.setGeometry(&geometry);}

    virtual oms_status_enu_t initialize(double startTime, double tolerance) = 0;
    virtual oms_status_enu_t terminate() = 0;
    virtual oms_status_enu_t simulate(ResultWriter& resultWriter, double stopTime, double communicationInterval) = 0;

    virtual oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultWriter) = 0;
    virtual oms_status_enu_t emit(ResultWriter& resultWriter) = 0;

  protected:
    CompositeModel(oms_element_type_enu_t type, const ComRef& cref);
    virtual ~CompositeModel();

  private:
    // stop the compiler generating methods copying the object
    CompositeModel(CompositeModel const& copy);            ///< not implemented
    CompositeModel& operator=(CompositeModel const& copy); ///< not implemented

  protected:
    oms2::Element element;
  };
}

#endif
