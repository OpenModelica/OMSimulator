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

#include "XercesValidator.h"
#include "whereami.h"

#include "Logging.h"
#include "OMSFileSystem.h"
#include "OMSString.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

using namespace xercesc_3_2;
class ParserErrorHandler : public ErrorHandler
{
public:
  const char *fileName = "";
  const char *filePath = "";
  ParserErrorHandler(const char *fileName_, const char *filePath_)
  {
    fileName = fileName_;
    filePath = filePath_;
  }
private:
  void reportParseException(const SAXParseException &ex)
  {
    char *msg = XMLString::transcode(ex.getMessage());
    logWarning("invalid \"" + std::string(fileName) + "\""  + " detected in file " + "\"" + std::string(filePath) + "\"" + " at line: " + std::to_string(ex.getLineNumber()) + " column: " + std::to_string(ex.getColumnNumber()) + ", " + std::string(msg));
    XMLString::release(&msg);
  }
public:
  void warning(const SAXParseException &ex)
  {
    reportParseException(ex);
  }
  void error(const SAXParseException &ex)
  {
    reportParseException(ex);
  }
  void fatalError(const SAXParseException &ex)
  {
    reportParseException(ex);
  }
  void resetErrors()
  {
  }
};

oms::XercesValidator::XercesValidator()
{
}

oms::XercesValidator::~XercesValidator()
{
  XMLPlatformUtils::Terminate();
}

std::string oms::XercesValidator::getExecutablePath()
{
  int dirname_length;
  int length = wai_getModulePath(NULL, 0, &dirname_length);

  if (length == 0)
    logError("Path to the current module could not be detected.");

  char* path = (char*)malloc(length + 1);

  if (wai_getModulePath(path, length, &dirname_length) == 0)
    logError("Path to the current module could not be detected.");

  path[dirname_length] = '\0';
  std::string executablePath = path;
  free(path);

  return executablePath;
}

oms_status_enu_t oms::XercesValidator::validateSSP(const char *ssd, const std::string& filePath)
{

  std::string extension = filesystem::path(filePath).extension().generic_string();

  if (extension != ".ssp" && extension != ".ssv" && extension != ".ssm")
    return logWarning("filename extension must be \".ssp\" or \".ssv\" or \".ssm\" ; no other formats are supported for SSP validation");

  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException &toCatch)
  {
    char *message = XMLString::transcode(toCatch.getMessage());
    logError("Xerces error during initialization: "+ std::string(message));
    XMLString::release(&message);
    return oms_status_error;
  }

  std::string path = getExecutablePath();

  if (path.empty())
    return logError("executable path could not be found");

  filesystem::path schemaRootPath (path);
  filesystem::path schemaSSDPath, schemaSSVPath, schemaSSMPath, schemaSSCPath;

  schemaSSDPath = schemaRootPath / "../share/OMSimulator/schema/ssp/SystemStructureDescription.xsd";
  schemaSSVPath = schemaRootPath / "../share/OMSimulator/schema/ssp/SystemStructureParameterValues.xsd";
  schemaSSMPath = schemaRootPath / "../share/OMSimulator/schema/ssp/SystemStructureParameterMapping.xsd";
  schemaSSCPath = schemaRootPath / "../share/OMSimulator/schema/ssp/SystemStructureCommon.xsd";

  // std::cout << "schemaPath: " << schemaFilePath.generic_string() << "\n" << filesystem::absolute(schemaFilePath).generic_string() << "\n";

  // this is done if we run the OMSimulator using python extension (e.g) python3 test.py and in this case the executable path is the dll path
  // for windows and mingw "libOMSimulator.dll" is put in "install/bin" directory and for linux and other platforms
  // the shared libraries are put in "install/lib/x86_64-linux-gnu/", so to find the schema location we have to move two directories back
  if (!filesystem::exists(schemaSSDPath))
  {
    // schema path from OMSimulator standalone builds
    schemaSSDPath = schemaRootPath / "../../share/OMSimulator/schema/ssp/SystemStructureDescription.xsd";
    schemaSSVPath = schemaRootPath / "../../share/OMSimulator/schema/ssp/SystemStructureParameterValues.xsd";
    schemaSSMPath = schemaRootPath / "../../share/OMSimulator/schema/ssp/SystemStructureParameterMapping.xsd";
    schemaSSCPath = schemaRootPath / "../../share/OMSimulator/schema/ssp/SystemStructureCommon.xsd";
  }

  // search schema path from top superproject OpenModelica location, shared libs are put in "build/lib/x86_64-linux-gnu/omc/libOMSimulator.so"
  if (!filesystem::exists(schemaSSDPath))
  {
    schemaSSDPath = schemaRootPath / "../../../share/OMSimulator/schema/ssp/SystemStructureDescription.xsd";
    schemaSSVPath = schemaRootPath / "../../../share/OMSimulator/schema/ssp/SystemStructureParameterValues.xsd";
    schemaSSMPath = schemaRootPath / "../../../share/OMSimulator/schema/ssp/SystemStructureParameterMapping.xsd";
    schemaSSCPath = schemaRootPath / "../../../share/OMSimulator/schema/ssp/SystemStructureCommon.xsd";
  }

  //check schema path location in python pip package, the schemas are copied to "OMSimulator/schema"
  if (!filesystem::exists(schemaSSDPath))
  {
    schemaSSDPath = schemaRootPath / "schema/ssp/SystemStructureDescription.xsd";
    schemaSSVPath = schemaRootPath / "schema/ssp/SystemStructureParameterValues.xsd";
    schemaSSMPath = schemaRootPath / "schema/ssp/SystemStructureParameterMapping.xsd";
    schemaSSCPath = schemaRootPath / "schema/ssp/SystemStructureCommon.xsd";
  }


  XercesDOMParser domParser;

  // load the schema
  if (domParser.loadGrammar(schemaSSDPath.generic_string().c_str(), Grammar::SchemaGrammarType) == NULL)
    return logWarning("could not load the ssd schema file: " + filesystem::absolute(schemaSSDPath).generic_string() + ", hence validation of ssd file will not be perfomed according to SSP Standard");

  std::string sspVariant = "";

  if (extension == ".ssp")
    sspVariant = "SystemStructureDescription";
  else if (extension == ".ssv")
    sspVariant = "SystemStructureParameterValues";
  else if (extension == ".ssm")
    sspVariant = "SystemStructureParameterMapping";

  ParserErrorHandler parserErrorHandler(sspVariant.c_str(), filePath.c_str());

  domParser.setErrorHandler(&parserErrorHandler);
  domParser.cacheGrammarFromParse(true);
  domParser.setValidationScheme(XercesDOMParser::Val_Always);
  domParser.setDoNamespaces(true);
  domParser.setDoSchema(true);
  domParser.setValidationSchemaFullChecking(true);
  domParser.setValidationConstraintFatal(true);
  // domParser.setExternalNoNamespaceSchemaLocation(schemaFilePath); // set this for noNameSpace
  std::string ssdTargetNameSpacePath = "http://ssp-standard.org/SSP1/SystemStructureDescription " + schemaSSDPath.generic_string();
  ssdTargetNameSpacePath = ssdTargetNameSpacePath + " http://ssp-standard.org/SSP1/SystemStructureParameterValues " + schemaSSVPath.generic_string();
  ssdTargetNameSpacePath = ssdTargetNameSpacePath + " http://ssp-standard.org/SSP1/SystemStructureParameterMapping " + schemaSSMPath.generic_string();
  ssdTargetNameSpacePath = ssdTargetNameSpacePath + " http://ssp-standard.org/SSP1/SystemStructureCommon " + schemaSSCPath.generic_string();

  domParser.setExternalSchemaLocation(ssdTargetNameSpacePath.c_str());

  if (strlen(ssd) > 0)
  {
    xercesc::MemBufInputSource pMemBufIS((const XMLByte *)ssd, std::string(ssd).size(), "ssdfile");
    domParser.parse(pMemBufIS);
  }
  else
  {
    domParser.parse(filePath.c_str());
  }

  if (domParser.getErrorCount() > 0)
    return logWarning( "\"" + sspVariant + "\"" + " does not conform to the SSP standard schema");

  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::validateFMU(const char *modeldescription, const std::string& filePath)
{
  std::string relativeFilePath = filesystem::relative(filePath).generic_string();
  std::string extension = filesystem::path(filePath).extension().generic_string();

  if (extension != ".fmu")
    return logWarning("filename extension must be \".fmu\" ; no other formats are supported for fmu validation");

  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException &toCatch)
  {
    char *message = XMLString::transcode(toCatch.getMessage());
    logError("Xerces error during initialization: "+ std::string(message));
    XMLString::release(&message);
    return oms_status_error;
  }

  std::string path = getExecutablePath();

  if (path.empty())
    return logError("executable path could not be found");

  filesystem::path schemaRootPath (path);
  filesystem::path schemaFmiModeldescriptionPath;

  schemaFmiModeldescriptionPath = schemaRootPath / "../share/OMSimulator/schema/fmi2/fmi2ModelDescription.xsd";

  // this is done if we run the OMSimulator using python extension (e.g) python3 test.py and in this case the executable path is the dll path
  // for windows and mingw "libOMSimulator.dll" is put in "install/bin" directory and for linux and other platforms
  // the shared libraries are put in "install/lib/x86_64-linux-gnu/", so to find the schema location we have to move two directories back
  if (!filesystem::exists(schemaFmiModeldescriptionPath))
  {
    // schema path from standalone OMSimulator build
    schemaFmiModeldescriptionPath = schemaRootPath / "../../share/OMSimulator/schema/fmi2/fmi2ModelDescription.xsd";
  }

  // schema path from top superproject OpenModelica location, shared libs are put in "build/lib/x86_64-linux-gnu/omc/libOMSimulator.so"
  if (!filesystem::exists(schemaFmiModeldescriptionPath))
  {
    schemaFmiModeldescriptionPath = schemaRootPath / "../../../share/OMSimulator/schema/fmi2/fmi2ModelDescription.xsd";
  }

  //check schema path location in python pip package, the schemas are copied to "OMSimulator/schema"
  if (!filesystem::exists(schemaFmiModeldescriptionPath))
  {
    schemaFmiModeldescriptionPath = schemaRootPath / "schema/fmi2/fmi2ModelDescription.xsd";
  }

  XercesDOMParser domParser;

  // load the schema
  if (domParser.loadGrammar(schemaFmiModeldescriptionPath.generic_string().c_str(), Grammar::SchemaGrammarType) == NULL)
    return logWarning("could not load the FMI schema file: " + filesystem::absolute(schemaFmiModeldescriptionPath).generic_string() + ", hence validation of \"modeldescription.xml\" with FMI 2.0 standard will not be performed");

  ParserErrorHandler parserErrorHandler("modeldescription.xml", relativeFilePath.c_str());

  domParser.setErrorHandler(&parserErrorHandler);
  domParser.cacheGrammarFromParse(true);
  domParser.setValidationScheme(XercesDOMParser::Val_Always);
  domParser.setDoNamespaces(true);
  domParser.setDoSchema(true);
  domParser.setValidationSchemaFullChecking(true);
  domParser.setValidationConstraintFatal(true);
  domParser.setExternalNoNamespaceSchemaLocation(schemaFmiModeldescriptionPath.generic_string().c_str()); // set this for noNameSpace

  xercesc::MemBufInputSource pMemBufIS((const XMLByte *)modeldescription, std::string(modeldescription).size(), "modeldescriptionfile");
  domParser.parse(pMemBufIS);

  if (domParser.getErrorCount() > 0)
    return logWarning("\"modeldescription.xml\" does not conform to the FMI-2.0 standard schema");

  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::validateSRMD(const std::string &filePath) {
  // Validate file extension
  oms_status_enu_t status = isSupportedExtension(filePath, {".srmd"});
  if (status != oms_status_ok)
    return status;

  // Initialize Xerces
  status = initializeXerces();
  if (status != oms_status_ok)
    return status;

  // Retrieve schema paths
  std::map<std::string, std::string> schemaPaths;
  const std::vector<std::tuple<std::string, std::string, std::string>> schemaFiles = {
      {"SystemStructureCommon.xsd","http://ssp-standard.org/SSP1/SystemStructureCommon", "ssp"},
      {"SSPTraceabilityCommon.xsd","http://ssp-standard.org/SSPTraceability1/SSPTraceabilityCommon", "ssp"},
      {"SimulationResourceMetaData.xsd","http://ssp-standard.org/SSPTraceability1/SimulationResourceMetaData", "ssp"},
  };
  status = resolveSchemaPaths(schemaPaths, schemaFiles);
  if (status != oms_status_ok)
    return status;

  // Configure the parser
  XercesDOMParser domParser;

  std::string relativeFilePath = filesystem::relative(filePath).generic_string();
  ParserErrorHandler parserErrorHandler("SimulationResourceMetaData", relativeFilePath.c_str());
  domParser.setErrorHandler(&parserErrorHandler);

  status = loadSchema(domParser, schemaPaths);
  if (status != oms_status_ok)
    return status;

  //iterate and print all

  // Parse the provided XML
  status = parseXML(domParser, filePath);
  if (status != oms_status_ok)
    return status;

  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::isSupportedExtension(const std::string &filePath, const std::vector<std::string> &validExtensions) {
  std::string extension = filesystem::path(filePath).extension().generic_string();
  if (std::find(validExtensions.begin(), validExtensions.end(), extension) == validExtensions.end()) {
    logWarning("Unsupported file extension: " + extension);
    return oms_status_warning;
  }
  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::initializeXerces() {
  static bool initialized = false; // Tracks if Xerces has already been initialized
  if (initialized) {
    return oms_status_ok;
  }

  try {
    XMLPlatformUtils::Initialize();
    initialized = true; // Mark Xerces as initialized
    return oms_status_ok;
  } catch (const XMLException &toCatch) {
    char *message = XMLString::transcode(toCatch.getMessage());
    logError("Xerces initialization error: " + std::string(message));
    XMLString::release(&message);
    return oms_status_error;
  }
}

oms_status_enu_t oms::XercesValidator::resolveSchemaPaths(std::map<std::string, std::string> &paths, const std::vector<std::tuple<std::string, std::string, std::string>> &schemaFiles) {
  std::string basePath = getExecutablePath();
  if (basePath.empty()) {
    logError("Executable path could not be found.");
    return oms_status_error;
  }

  for (const auto& [file, ns, dir] : schemaFiles) {
    const std::vector<std::string> potentialRoots = {
      "../share/OMSimulator/schema/" + dir,
      "../../share/OMSimulator/schema/" + dir,
      "../../../share/OMSimulator/schema/" + dir,
      "schema/" + dir
    };

    for (const auto &root : potentialRoots) {
      filesystem::path path = filesystem::path(basePath) / root / file;
      if (filesystem::exists(path)) {
        paths[ns] = path.generic_string();
      }
    }

    if (paths.find(ns) == paths.end()) {
      logWarning("Schema file not found: " + file);
      return oms_status_warning;
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::loadSchema(XercesDOMParser &parser, const std::map<std::string, std::string> &schemaPaths) {
  for (const auto &[namespaceUri, path] : schemaPaths) {
    if (parser.loadGrammar(path.c_str(), Grammar::SchemaGrammarType, true) == nullptr) {
      logWarning("Could not load schema file: " + path);
      return oms_status_warning;
    }
  }

  std::string schemaLocation;
  for (const auto &[namespaceUri, path] : schemaPaths) {
    schemaLocation += namespaceUri + " " + path + " ";
  }

  parser.setExternalSchemaLocation(schemaLocation.c_str());
  parser.cacheGrammarFromParse(true);
  parser.setValidationScheme(XercesDOMParser::Val_Always);
  parser.setDoNamespaces(true);
  parser.setDoSchema(true);
  parser.setValidationSchemaFullChecking(true);
  parser.setValidationConstraintFatal(true);

  return oms_status_ok;
}

oms_status_enu_t oms::XercesValidator::parseXML(XercesDOMParser &parser, const std::string &filePath) {
  try {
    parser.parse(filePath.c_str());

    if (parser.getErrorCount() > 0) {
      logWarning("XML parse error: validation failed.");
      return oms_status_warning;
    }

    return oms_status_ok;
  } catch (const XMLException &e) {
    char *message = XMLString::transcode(e.getMessage());
    logError("Xerces parse error: " + std::string(message));
    XMLString::release(&message);
    return oms_status_error;
  } catch (const DOMException &e) {
    char *message = XMLString::transcode(e.msg);
    logError("DOM parse error: " + std::string(message));
    XMLString::release(&message);
    return oms_status_error;
  }
}
