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

#include "DCPInfo.h"

#include "Logging.h"
#include "OMSString.h"

#include <dcp/xml/DcpSlaveDescriptionElements.hpp>
#include <dcp/zip/DcpSlaveReader.hpp>

oms::DCPInfo::DCPInfo(const std::string& path)
{
  this->dcpMajorVersion = 0;
  this->dcpMinorVersion = 0;
  this->dcpSlaveName = nullptr;
  this->uuid = nullptr;
  this->description = nullptr;
  this->author = nullptr;
  this->version = nullptr;
  this->copyright = nullptr;
  this->license = nullptr;
  this->generationTool = nullptr;
  this->generationDateAndTime = nullptr;
  this->variableNamingConvention = 0;
  this->path = allocateAndCopyString(path);

  this->update(getSlaveDescriptionFromDcpFile(1, 0, path));
}

oms::DCPInfo::~DCPInfo()
{
  if (this->dcpSlaveName) delete[] this->dcpSlaveName;
  if (this->uuid) delete[] this->uuid;
  if (this->description) delete[] this->description;
  if (this->author) delete[] this->author;
  if (this->version) delete[] this->version;
  if (this->copyright) delete[] this->copyright;
  if (this->license) delete[] this->license;
  if (this->generationTool) delete[] this->generationTool;
  if (this->generationDateAndTime) delete[] this->generationDateAndTime;
  if (this->path) delete[] this->path;
}

void oms::DCPInfo::update(std::shared_ptr<SlaveDescription_t> dcpSlaveDescription)
{
  this->dcpMajorVersion = dcpSlaveDescription->dcpMajorVersion;
  this->dcpMinorVersion = dcpSlaveDescription->dcpMinorVersion;
  this->dcpSlaveName = allocateAndCopyString(dcpSlaveDescription->dcpSlaveName);
  this->uuid = allocateAndCopyString(dcpSlaveDescription->uuid);  
  this->description = allocateAndCopyString(dcpSlaveDescription->description ? dcpSlaveDescription->description->c_str() : "");
  this->author = allocateAndCopyString(dcpSlaveDescription->author ? dcpSlaveDescription->author->c_str() : "");
  this->version = allocateAndCopyString(dcpSlaveDescription->version ? dcpSlaveDescription->version->c_str() : "");
  this->copyright = allocateAndCopyString(dcpSlaveDescription->copyright ? dcpSlaveDescription->copyright->c_str() : "");
  this->license = allocateAndCopyString(dcpSlaveDescription->license ? dcpSlaveDescription->license->c_str() : "");
  this->generationTool = allocateAndCopyString(dcpSlaveDescription->generationTool ? dcpSlaveDescription->generationTool->c_str() : "");
  this->generationDateAndTime = allocateAndCopyString(dcpSlaveDescription->generationDateAndTime ? dcpSlaveDescription->generationDateAndTime->c_str() : "");
  this->variableNamingConvention = (int)dcpSlaveDescription->variableNamingConvention;    
}

