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

#ifndef _OMS_FMI_SUB_MODEL_H_
#define _OMS_FMI_SUB_MODEL_H_

#include "../Types.h"
#include "ComRef.h"
#include "ssd/ElementGeometry.h"

namespace oms2
{
  class FMISubModel
  {
  public:
    virtual oms_component_type_enu_t getType() const = 0;
    static void deleteSubModel(FMISubModel *model) {if (model) delete model;}

    const ComRef& getName() const {return cref;}
    void setGeometry(double x1, double y1, double x2, double y2) {this->geometry.setSizePosition(x1, y1, x2, y2);}
    void setGeometry(const oms2::ssd::ElementGeometry& geometry) {this->geometry = geometry;}
    oms2::ssd::ElementGeometry* getGeometry() {return &geometry;}

    oms_component_t* getComponent() {return &component;}

  protected:
    FMISubModel(const ComRef& cref);
    virtual ~FMISubModel();

  private:
    // stop the compiler generating methods copying the object
    FMISubModel(FMISubModel const& copy);            // not implemented
    FMISubModel& operator=(FMISubModel const& copy); // not implemented

  protected:
    ComRef cref;
    oms2::ssd::ElementGeometry geometry;
    oms_component_t component;
  };
}

#endif
