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

#ifndef _OMS2_FMU_INFO_H_
#define _OMS2_FMU_INFO_H_

#include "Types.h"

#include <string>
#include <fmilib.h>

namespace oms2
{
  /**
   * \brief FMU info
   */
  class FMUInfo : protected oms_fmu_info_t
  {
  public:
    FMUInfo(const std::string& path);
    ~FMUInfo();

    oms_status_enu_t setKind(fmi2_import_t* fmu);
    oms_status_enu_t update(fmi2_import_t* fmu);

    std::string getPath() const {return std::string(path);}
    oms_fmi_kind_enu_t getKind() const {return fmiKind;}

  private:
    // methods to copy the object
    FMUInfo(const FMUInfo& rhs);            ///< not implemented
    FMUInfo& operator=(const FMUInfo& rhs); ///< not implemented
  };
}

#endif
