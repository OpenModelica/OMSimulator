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
  // set the document with the root node <oms:snapshot>
  oms_snapshot = doc.append_child(oms::ssp::Version1_0::snap_shot);
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
  pugi::xml_document snapshotdoc;
  pugi::xml_parse_result result = snapshotdoc.load_file(filename.c_str());
  if (!result)
    return logError("loading resource \"" + filename.generic_string() + "\" failed (" + std::string(result.description()) + ")");

  pugi::xml_node oms_file = oms_snapshot.append_child(oms::ssp::Version1_0::oms_file);
  oms_file.append_attribute("name") = naive_uncomplete(filename, root).generic_string().c_str();
  oms_file.append_copy(snapshotdoc.document_element());

  //debugPrintFile(naive_uncomplete(filename, root).generic_string());
  return oms_status_ok;
}

oms_status_enu_t oms::Snapshot::importResourcesMemory(const std::string & filename, const pugi::xml_node &node)
{
  if (node)
  {
    oms_snapshot.append_copy(node);
  }
  //debugPrintFile(filename);
  return oms_status_ok;
}

void oms::Snapshot::getResources(std::vector<std::string> & resources)
{
  for (const auto & it: oms_snapshot.children())
  {
    resources.push_back(it.attribute("name").as_string());
  }
}

pugi::xml_node oms::Snapshot::getNode(const std::string& filename)
{
  return oms_snapshot.find_child_by_attribute(oms::ssp::Version1_0::oms_file, "name", filename.c_str());
}

void oms::Snapshot::debugPrintFile(const std::string& filename)
{
  pugi::xml_node oms_node = oms_snapshot.find_child_by_attribute(oms::ssp::Version1_0::oms_file, "name", filename.c_str());
  if (!oms_node)
  {
    logError("Failed to find \"" + filename + "\" in the snapshot");
  }

  oms_node.print(std::cout);
}

void oms::Snapshot::printSnapshot() const
{
  doc.save(std::cout);
}