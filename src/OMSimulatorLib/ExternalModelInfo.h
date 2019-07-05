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

#ifndef _OMS_EXTERNAL_MODEL_INFO_H_
#define _OMS_EXTERNAL_MODEL_INFO_H_

#include "Types.h"

#include <string>

namespace oms
{
  /**
   * \brief External model info
   */
  class ExternalModelInfo : protected oms_external_tlm_model_info_t
  {
  public:
    ExternalModelInfo(const std::string& path, const std::string& startScript);
    ~ExternalModelInfo();

    std::string getPath() const {return std::string(path);}
    std::string getStartScript() const {return std::string(startScript);}

  private:
    // methods to copy the object
    ExternalModelInfo(const ExternalModelInfo& rhs);            ///< not implemented
    ExternalModelInfo& operator=(const ExternalModelInfo& rhs); ///< not implemented
  };
}

#endif
