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

#ifndef _OMS_COMPONENT_TABLE_H_
#define _OMS_COMPONENT_TABLE_H_

#include "Component.h"
#include "ComRef.h"
#include "ResultReader.h"
#include <fmilib.h>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace oms3
{
  class ComponentTable : public Component
  {
  public:
    ~ComponentTable();

    static Component* NewComponent(const oms3::ComRef& cref, System* parentSystem, const std::string& path);
    static Component* NewComponent(const pugi::xml_node& node, System* parentSystem);

    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();

  protected:
    ComponentTable(const ComRef& cref, System* parentSystem, const std::string& path);

    // stop the compiler generating methods copying the object
    ComponentTable(ComponentTable const& copy);            ///< not implemented
    ComponentTable& operator=(ComponentTable const& copy); ///< not implemented

  private:
    ResultReader* resultReader;
    std::unordered_map<std::string, ResultReader::Series*> series;
  };
}

#endif
