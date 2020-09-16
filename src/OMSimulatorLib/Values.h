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

#ifndef _OMS_VALUES_H_
#define _OMS_VALUES_H_

#include "ComRef.h"
#include "Types.h"
#include <pugixml.hpp>
#include <map>

namespace oms
{
  class Values
  {
  public:
    Values();
    ~Values();

    oms_status_enu_t setReal(const ComRef& cref, double value);
    oms_status_enu_t setInteger(const ComRef& cref, int value);
    oms_status_enu_t setBoolean(const ComRef& cref, bool value);

    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD(const pugi::xml_node& node, const std::string& sspVersion, const std::string& tempdir);
    oms_status_enu_t deleteStartValue(const ComRef& cref);

    oms_status_enu_t exportToSSV(pugi::xml_node& ssvNode) const;
    oms_status_enu_t exportStartValuesHelper(pugi::xml_node& node) const;
    oms_status_enu_t importStartValuesHelper(pugi::xml_node& parameters);
    oms_status_enu_t parseModelDescription(const char *filename);

    std::map<ComRef, double> realStartValues;  ///< parameters and start values defined before instantiating the FMU
    std::map<ComRef, int> integerStartValues;  ///< parameters and start values defined before instantiating the FMU
    std::map<ComRef, bool> booleanStartValues; ///< parameters and start values defined before instantiating the FMU

    std::map<ComRef, double> realValues;  ///< real input values defined after initialization
    std::map<ComRef, int> integerValues;  ///< integer input values defined after initialization
    std::map<ComRef, bool> booleanValues;  ///< boolean input values defined after initialization

    std::map<ComRef, double> modelDescriptionRealStartValues;  ///< real start values read from modelDescription.xml
    std::map<ComRef, int> modelDescriptionIntegerStartValues;  ///< integer start values read from modelDescription.xml
    std::map<ComRef, bool> modelDescriptionBooleanStartValues;  ///< boolean start values read from modelDescription.xml

  };
}

#endif
