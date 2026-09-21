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
