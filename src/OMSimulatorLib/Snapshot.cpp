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
  doc.append_child(oms::ssp::Version1_0::snap_shot);
}

oms::Snapshot::~Snapshot()
{
}

pugi::xml_node oms::Snapshot::newResourceNode(const filesystem::path& filename)
{
  pugi::xml_node oms_snapshot = doc.document_element();
  pugi::xml_node node = oms_snapshot.find_child_by_attribute(oms::ssp::Version1_0::oms_file, "name", filename.generic_string().c_str());

  if (node)
  {
    logError("Node \"" + filename.generic_string() + "\" does already exist");
    return node.first_child();
  }

  pugi::xml_node new_node = oms_snapshot.append_child(oms::ssp::Version1_0::oms_file);
  new_node.append_attribute("name") = filename.generic_string().c_str();
  return new_node;
}

oms_status_enu_t oms::Snapshot::import(const char* snapshot)
{
  doc.reset();
  pugi::xml_parse_result result = doc.load_string(snapshot);
  if (!result)
    return logError("loading snapshot failed (" + std::string(result.description()) + ")");
  return oms_status_ok;
}

oms_status_enu_t oms::Snapshot::importResourceFile(const filesystem::path& filename, const filesystem::path& root)
{
  filesystem::path p = root / filename;

  pugi::xml_document tmp_doc;
  pugi::xml_parse_result result = tmp_doc.load_file(p.c_str());
  if (!result)
    return logError("loading resource \"" + p.generic_string() + "\" failed (" + std::string(result.description()) + ")");

  return importResourceNode(filename, tmp_doc.document_element());
}

oms_status_enu_t oms::Snapshot::importResourceMemory(const filesystem::path& filename, const char* contents)
{
  pugi::xml_document tmp_doc;
  pugi::xml_parse_result result = tmp_doc.load_string(contents);
  if (!result)
    return logError("loading resource \"" + filename.generic_string() + "\" failed (" + std::string(result.description()) + ")");

  return importResourceNode(filename, tmp_doc.document_element());
}

oms_status_enu_t oms::Snapshot::importResourceNode(const filesystem::path& filename, const pugi::xml_node& node)
{
  pugi::xml_node oms_snapshot = doc.document_element();
  pugi::xml_node oms_file = oms_snapshot.append_child(oms::ssp::Version1_0::oms_file);
  oms_file.append_attribute("name") = filename.generic_string().c_str();
  oms_file.append_copy(node);

  return oms_status_ok;
}

void oms::Snapshot::getResources(std::vector<std::string>& resources) const
{
  pugi::xml_node oms_snapshot = doc.document_element();
  for (const auto& it : oms_snapshot.children())
    resources.push_back(it.attribute("name").as_string());
}

pugi::xml_node oms::Snapshot::getResourceNode(const filesystem::path& filename) const
{
  pugi::xml_node oms_snapshot = doc.document_element();
  pugi::xml_node node = oms_snapshot.find_child_by_attribute(oms::ssp::Version1_0::oms_file, "name", filename.generic_string().c_str());

  if (node)
    return node.first_child();

  logError("Failed to find node \"" + filename.generic_string() + "\"");
  return node;
}

pugi::xml_node oms::Snapshot::operator[](const filesystem::path& filename)
{
  pugi::xml_node oms_snapshot = doc.document_element();
  pugi::xml_node node = oms_snapshot.find_child_by_attribute(oms::ssp::Version1_0::oms_file, "name", filename.generic_string().c_str());

  if (node)
    return node.first_child();

  return newResourceNode(filename);
}

void oms::Snapshot::debugPrintNode(const filesystem::path& filename) const
{
  pugi::xml_node node = getResourceNode(filename);

  if (node)
    node.print(std::cout, "  ");
}

void oms::Snapshot::debugPrintAll() const
{
  doc.save(std::cout, "  ");
}

pugi::xml_node oms::Snapshot::getTemplateResourceNodeSSD(const filesystem::path& filename)
{
  pugi::xml_node new_node = newResourceNode(filename);
  new_node.append_attribute("xmlns:ssc") = "http://ssp-standard.org/SSP1/SystemStructureCommon";
  new_node.append_attribute("xmlns:ssd") = "http://ssp-standard.org/SSP1/SystemStructureDescription";
  new_node.append_attribute("xmlns:ssv") = "http://ssp-standard.org/SSP1/SystemStructureParameterValues";
  new_node.append_attribute("xmlns:ssm") = "http://ssp-standard.org/SSP1/SystemStructureParameterMapping";
  new_node.append_attribute("xmlns:ssb") = "http://ssp-standard.org/SSP1/SystemStructureSignalDictionary";
  new_node.append_attribute("xmlns:oms") = "https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd";
  // new_node.append_attribute("name") = this->getCref().c_str();
  new_node.append_attribute("version") = "1.0";

  return new_node;
}

pugi::xml_node oms::Snapshot::getTemplateResourceNodeSSV(const filesystem::path& filename)
{
  pugi::xml_node new_node = newResourceNode(filename);
  pugi::xml_node node_parameterset = new_node.append_child(oms::ssp::Version1_0::ssv::parameter_set);
  node_parameterset.append_attribute("xmlns:ssc") = "http://ssp-standard.org/SSP1/SystemStructureCommon";
  node_parameterset.append_attribute("xmlns:ssv") = "http://ssp-standard.org/SSP1/SystemStructureParameterValues";
  node_parameterset.append_attribute("version") = "1.0";
  node_parameterset.append_attribute("name") = "parameters";
  pugi::xml_node node_parameters = node_parameterset.append_child(oms::ssp::Version1_0::ssv::parameters);

  return node_parameters;
}

oms_status_enu_t oms::Snapshot::writeDocument(char** contents)
{
  struct xmlStringWriter : pugi::xml_writer
  {
    std::string result;
    virtual void write(const void *data, size_t size)
    {
      result += std::string(static_cast<const char *>(data), size);
    }
  };

  xmlStringWriter writer;

  doc.save(writer);
  *contents = (char*) malloc(strlen(writer.result.c_str()) + 1);
  if (!*contents)
  {
    logError("Out of memory");
    return oms_status_fatal;
  }
  strcpy(*contents, writer.result.c_str());

  return oms_status_ok;
}