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

#include "LsDae.h"

#include "Logging.h"
#include "OMSFileSystem.h"

#include <pugixml.hpp>
#include <sstream>

/**
 * \brief The value references in a whitespace-separated `dependencies` list.
 */
static std::vector<uint32_t> parseValueReferenceList(const char* text)
{
  std::vector<uint32_t> vrs;
  if (!text)
    return vrs;
  std::istringstream in(text);
  unsigned long vr;
  while (in >> vr)
    vrs.push_back(static_cast<uint32_t>(vr));
  return vrs;
}

bool oms::LsDaeManifest::load(const std::string& unzippedFmuPath)
{
  const filesystem::path manifest = filesystem::path(unzippedFmuPath) / filesystem::path(path());
  if (!filesystem::exists(manifest))
    return false;   // no DAE formulation, which is the ordinary case

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(manifest.string().c_str());
  if (!result)
  {
    logWarning("fmi-ls-dae: could not read \"" + manifest.string() + "\": " + std::string(result.description()));
    return false;
  }

  const pugi::xml_node root = doc.child("fmiDAEManifest");
  if (!root)
  {
    logWarning("fmi-ls-dae: \"" + manifest.string() + "\" has no <fmiDAEManifest> element");
    return false;
  }

  version = root.attribute("fmi-ls:fmi-ls-version").as_string();

  const pugi::xml_node enable = root.child("EnableDAEParameter");
  if (!enable)
  {
    logWarning("fmi-ls-dae: the manifest names no <EnableDAEParameter>, so the FMU cannot be switched into DAE mode");
    return false;
  }
  enableVr = enable.attribute("valueReference").as_uint();

  for (const pugi::xml_node& node : root.child("AlgebraicVariables").children("AlgebraicVariable"))
    algebraicVrs.push_back(node.attribute("valueReference").as_uint());

  // The manifest's <ModelStructure> replaces the model description's: no
  // <ContinuousStateDerivative> is the fully implicit form, where the residuals
  // cover the state rows as well.
  const pugi::xml_node structure = root.child("ModelStructure");
  for (const pugi::xml_node& node : structure.children("ContinuousStateDerivative"))
    continuousStateDerivatives.push_back(node.attribute("valueReference").as_uint());

  for (const pugi::xml_node& node : structure.children("Residual"))
  {
    residualVrs.push_back(node.attribute("valueReference").as_uint());
    residualDependencies.push_back(parseValueReferenceList(node.attribute("dependencies").value()));
  }

  if (residualVrs.empty())
  {
    logWarning("fmi-ls-dae: the manifest lists no <Residual>, so there is no DAE system to solve");
    return false;
  }

  valid = true;
  return true;
}

bool oms::LsDaeAcausalManifest::load(const std::string& unzippedFmuPath)
{
  const filesystem::path manifest = filesystem::path(unzippedFmuPath) / filesystem::path(path());
  if (!filesystem::exists(manifest))
    return false;   // not an acausal component

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(manifest.string().c_str());
  if (!result)
  {
    logWarning("fmi-ls-dae-acausal: could not read \"" + manifest.string() + "\": " + std::string(result.description()));
    return false;
  }

  const pugi::xml_node root = doc.child("fmiDAEAcausalManifest");
  if (!root)
  {
    logWarning("fmi-ls-dae-acausal: \"" + manifest.string() + "\" has no <fmiDAEAcausalManifest> element");
    return false;
  }

  version = root.attribute("fmi-ls:fmi-ls-version").as_string();
  for (const pugi::xml_node& node : root.child("ConnectorResiduals").children("ConnectorResidual"))
    connectorResiduals.push_back({node.attribute("valueReference").as_uint(),
                                  node.attribute("terminal").as_string(),
                                  node.attribute("variableName").as_string()});
  for (const pugi::xml_node& node : root.child("ConnectorResiduals").children("StreamResidual"))
    streamResiduals.push_back({node.attribute("valueReference").as_uint(),
                               node.attribute("terminal").as_string(),
                               node.attribute("variableName").as_string()});

  valid = true;
  return true;
}

bool oms::Terminals::load(const std::string& unzippedFmuPath)
{
  const filesystem::path file = filesystem::path(unzippedFmuPath) / filesystem::path(path());
  if (!filesystem::exists(file))
    return false;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(file.string().c_str());
  if (!result)
  {
    logWarning("could not read \"" + file.string() + "\": " + std::string(result.description()));
    return false;
  }

  // Only the top-level terminals: a nested terminal is not a connector an
  // acausal connection is made between.
  for (const pugi::xml_node& node : doc.child("fmiTerminalsAndIcons").child("Terminals").children("Terminal"))
  {
    Terminal terminal;
    terminal.name = node.attribute("name").as_string();
    terminal.terminalKind = node.attribute("terminalKind").as_string();
    for (const pugi::xml_node& member : node.children("TerminalMemberVariable"))
      terminal.members.push_back({member.attribute("variableName").as_string(),
                                  member.attribute("memberName").as_string(),
                                  member.attribute("variableKind").as_string()});
    for (const pugi::xml_node& member : node.children("TerminalStreamMemberVariable"))
      terminal.streamMembers.push_back({member.attribute("inStreamMemberName").as_string(),
                                        member.attribute("outStreamMemberName").as_string(),
                                        member.attribute("inStreamVariableName").as_string(),
                                        member.attribute("outStreamVariableName").as_string()});
    terminals.push_back(terminal);
  }

  // The aliases, which a member may be: e.g. b.p of a component with a.p = b.p.
  pugi::xml_document md;
  const filesystem::path modelDescription = filesystem::path(unzippedFmuPath) / filesystem::path("modelDescription.xml");
  if (md.load_file(modelDescription.string().c_str()))
    for (const pugi::xml_node& variable : md.child("fmiModelDescription").child("ModelVariables").children())
      for (const pugi::xml_node& alias : variable.children("Alias"))
        aliasOf[alias.attribute("name").as_string()] = variable.attribute("name").as_string();
  return true;
}

std::string oms::Terminals::resolveAlias(const std::string& variableName) const
{
  const auto it = aliasOf.find(variableName);
  return it == aliasOf.end() ? variableName : it->second;
}
