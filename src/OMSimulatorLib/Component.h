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

#ifndef _OMS_COMPONENT_H_
#define _OMS_COMPONENT_H_

#include "ComRef.h"
#include "Types.h"
#include "Element.h"
#include <pugixml.hpp>

namespace oms3
{
  class Component
  {
  public:
    virtual ~Component();

    const ComRef& getName() const {return cref;}
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms3::Element* getElement() {return &element;}
    oms3::Connector *getConnector(const ComRef &cref);

  protected:

  protected:
    Component(const ComRef& cref);

    // stop the compiler generating methods copying the object
    Component(Component const&);            ///< not implemented
    Component& operator=(Component const&); ///< not implemented

  private:
    oms3::Element element;
    oms3::ComRef cref;
    std::vector<oms3::Connector*> connectors;
  };
}

#endif
