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

#ifndef _OMS_XERCES_VALIDATOR_H_
#define _OMS_XERCES_VALIDATOR_H_

#include "ComRef.h"
#include "OMSFileSystem.h"
#include "Snapshot.h"
#include "OMSimulator/Types.h"

#include <map>
#include <xercesc/parsers/XercesDOMParser.hpp>

XERCES_CPP_NAMESPACE_USE

namespace oms
{
  class XercesValidator
  {
  public:
    XercesValidator();
    ~XercesValidator();
    oms_status_enu_t validateSSP(const char * ssd, const std::string& filePath);
    oms_status_enu_t validateFMU(const char * modeldescription, const std::string& filePath);
    std::string getExecutablePath();
    oms_status_enu_t validateSRMD(const std::string &filePath);

  private:
    oms_status_enu_t isSupportedExtension(const std::string &filePath, const std::vector<std::string> &validExtensions);
    oms_status_enu_t initializeXerces();
    oms_status_enu_t resolveSchemaPaths(std::map<std::string, std::string> &paths, const std::vector<std::tuple<std::string, std::string, std::string>> &schemaFiles);
    oms_status_enu_t loadSchema(XercesDOMParser &parser, const std::map<std::string, std::string> &schemaPaths);
    oms_status_enu_t parseXML(XercesDOMParser &parser, const std::string &filePath);
  };
}

#endif
