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

#include "Snapshot.h"

#include "Logging.h"

#include <iostream>

oms::Snapshot::Snapshot()
{
}

oms::Snapshot::~Snapshot()
{
}

oms_status_enu_t oms::Snapshot::importSnapshot(const char* snapshot)
{
  return logError_NotImplemented;
}

oms_status_enu_t oms::Snapshot::importResourcesFile(const filesystem::path& filename, const filesystem::path& root)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
    return logError("loading resource \"" + filename.generic_string() + "\" failed (" + std::string(result.description()) + ")");

  this->resources[filename].load_file(filename.c_str());
  //debugPrintFile(filename);
  return oms_status_ok;
}

oms_status_enu_t oms::Snapshot::importResourcesMemory(const filesystem::path& filename, const char* file)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load(file);
  if (!result)
    return logError("loading resource \"" + filename.generic_string() + "\" failed (" + std::string(result.description()) + ")");

  this->resources[filename].load(file);
  //debugPrintFile(filename);
  return oms_status_ok;
}

oms_status_enu_t oms::Snapshot::getResources(const filesystem::path& filename, pugi::xml_node& node)
{
  auto entry = this->resources.find(filename);
  if (this->resources.end() == entry)
    return logError("Failed to find \"" + filename.generic_string() + "\" in the snapshot");

  node = entry->second.document_element();
  return oms_status_ok;
}

void oms::Snapshot::debugPrintFile(const filesystem::path& filename)
{
  auto entry = this->resources.find(filename);
  if (this->resources.end() == entry)
    logError("Failed to find \"" + filename.generic_string() + "\" in the snapshot");
  else
    entry->second.save(std::cout);
}
