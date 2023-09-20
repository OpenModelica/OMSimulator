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
#include "ssd/Tags.h"
#include "Util.h"
#include "OMSFileSystem.h"

#include <iostream>
#include <map>
#include "miniunz.h"
#include <string>

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
      logError("invalid " + std::string(fileName) + " detected in file " + "\"" + std::string(filePath) + "\"" + " at line: " + std::to_string(ex.getLineNumber()) + " column: " + std::to_string(ex.getColumnNumber()) + ", " + std::string(msg));
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

oms_status_enu_t oms::XercesValidator::validateSSD(const char *ssd, const std::string& filePath)
{
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


  int dirname_length;
  char * path;
  int length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0)
  {
    path = (char*)malloc(length + 1);
    if (!path)
      abort();
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';

    // printf("executable path: %s\n", path);
    path[dirname_length] = '\0';
    free(path);
    // printf("  dirname: %s\n", path);
    // printf("  basename: %s\n", path + dirname_length + 1);
  }

  filesystem::path schemaRootPath (path);
  filesystem::path schemaFilePath = schemaRootPath / "../share/OMSimulator/schema/ssp/SystemStructureDescription.xsd";
  // std::cout << "schemaPath: " << schemaFilePath.generic_string() << "\n" << filesystem::absolute(schemaFilePath).generic_string() << "\n";
  XercesDOMParser domParser;

  // load the schema
  if (domParser.loadGrammar(schemaFilePath.generic_string().c_str(), Grammar::SchemaGrammarType) == NULL)
      return logError("could not load the ssd schema file: " + filesystem::absolute(schemaFilePath).generic_string());

  ParserErrorHandler parserErrorHandler("SystemStructure.ssd", filePath.c_str());

  domParser.setErrorHandler(&parserErrorHandler);
  domParser.cacheGrammarFromParse(true);
  domParser.setValidationScheme(XercesDOMParser::Val_Always);
  domParser.setDoNamespaces(true);
  domParser.setDoSchema(true);
  domParser.setValidationSchemaFullChecking(true);
  domParser.setValidationConstraintFatal(true);
  // domParser.setExternalNoNamespaceSchemaLocation(schemaFilePath); // set this for noNameSpace
  std::string ssdTargetNameSpacePath = "http://ssp-standard.org/SSP1/SystemStructureDescription " + schemaFilePath.generic_string();
  domParser.setExternalSchemaLocation(ssdTargetNameSpacePath.c_str());

  xercesc::MemBufInputSource pMemBufIS((const XMLByte*)ssd, std::string(ssd).size() , "ssdfile");
  domParser.parse(pMemBufIS);

  if (domParser.getErrorCount() > 0)
      return logError("SystemStructure.ssd does not conform to the SSP standard schema");

  return oms_status_ok;
}