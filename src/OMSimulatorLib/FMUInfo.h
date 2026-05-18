/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_FMU_INFO_H_
#define _OMS_FMU_INFO_H_

#include "OMSimulator/Types.h"

#include <string>
#include <fmi4c.h>
namespace oms
{
  /**
   * \brief FMU info
   */
  class FMUInfo : protected oms_fmu_info_t
  {
  public:
    FMUInfo(const std::string& path);
    ~FMUInfo();

    void update(oms_component_enu_t componentType, fmiHandle* fmi4c);

    std::string getPath() const {return std::string(path);}
    oms_fmi_kind_enu_t getKind() const {return fmiKind;}

    bool getCanInterpolateInputs() const {return canInterpolateInputs;}
    bool getCanGetAndSetFMUstate() const {return canGetAndSetFMUstate;}
    unsigned int getMaxOutputDerivativeOrder() const {return maxOutputDerivativeOrder;}
    bool getProvidesDirectionalDerivative() const {return providesDirectionalDerivative;}
    std::string getGenerationTool() const {return std::string(generationTool);}

  private:
    void updateFMI2Info(fmiHandle *fmi4c);
    void updateFMI3Info(fmiHandle *fmi4c);
    void updateDCPInfo();

    // methods to copy the object
    FMUInfo(const FMUInfo& rhs);            ///< not implemented
    FMUInfo& operator=(const FMUInfo& rhs); ///< not implemented
  };
}

#endif
