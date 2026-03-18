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

#ifndef _OMS_VALUES_H_
#define _OMS_VALUES_H_

#include "ComRef.h"
#include "OMSFileSystem.h"
#include "Snapshot.h"
#include "OMSimulator/Types.h"

#include <map>
#include <pugixml.hpp>

namespace oms
{
  class Values
  {
  public:
    Values();
    ~Values();

    oms_status_enu_t setBoolean(const ComRef& cref, bool value);
    oms_status_enu_t setInteger(const ComRef& cref, int value);
    oms_status_enu_t setReal(const ComRef& cref, double value);
    oms_status_enu_t setString(const ComRef& cref, const std::string& value);
    oms_status_enu_t setUnit(const ComRef& cref, const std::string& value);
    void setUnitDefinitions(const ComRef& cref);
    void getFilteredUnitDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& unitDefinitions);
    void getFilteredEnumerationDefinitionsToSSD(std::map<std::string, std::map<std::string, std::string>>& enumerationDefinitions);
    void updateModelDescriptionRealStartValue(const ComRef& cref, double value);
    void updateModelDescriptionIntegerStartValue(const ComRef& cref, int value);
    void updateModelDescriptionBooleanStartValue(const ComRef& cref, bool value);
    void updateModelDescriptionStringStartValue(const ComRef& cref, std::string value);

    void updateModelDescriptionVariableUnit(const ComRef& cref, const std::string& value);
    void copyModelDescriptionUnitToResources(Values& value);

    oms_status_enu_t getBoolean(const ComRef& cref, bool& value);
    oms_status_enu_t getInteger(const ComRef& cref, int& value);
    oms_status_enu_t getReal(const ComRef& cref, double& value);
    oms_status_enu_t getString(const ComRef& cref, std::string& value);

    oms_status_enu_t setBooleanResources(const ComRef& cref, bool value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t setIntegerResources(const ComRef& cref, int value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t setRealResources(const ComRef& cref, double value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t setStringResources(const ComRef& cref, const std::string& value, const ComRef& fullCref, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t setUnitResources(const ComRef& cref, const std::string& value, const ComRef& fullCref);

    oms_status_enu_t getBooleanResources(const ComRef& cref, bool& value, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t getIntegerResources(const ComRef& cref, int& value, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t getRealResources(const ComRef& cref, double& value, bool externalInput, oms_modelState_enu_t modelState);
    oms_status_enu_t getStringResources(const ComRef& cref, std::string& value, bool externalInput, oms_modelState_enu_t modelState);

    oms_status_enu_t getBooleanFromModeldescription(const ComRef& cref, bool& value);
    oms_status_enu_t getIntegerFromModeldescription(const ComRef& cref, int& value);
    oms_status_enu_t getRealFromModeldescription(const ComRef& cref, double& value);
    oms_status_enu_t getStringFromModeldescription(const ComRef& cref, std::string& value);

    std::string getUnit(ComRef& cref) const;
    std::string getUnitFromModeldescription(ComRef& cref) const;
    std::string getEnumerationTypeFromModeldescription(ComRef& cref) const;
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSnapshot(const pugi::xml_node& node, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName);
    oms_status_enu_t importFromSnapshot(const Snapshot& snapshot, const std::string& ssvFilePath, const std::string& ssmFilename);
    oms_status_enu_t deleteStartValue(const ComRef& cref);
    oms_status_enu_t updateOrDeleteStartValueInReplacedComponent(Values& value, const ComRef& owner, std::vector<std::string>& warningList);
    oms_status_enu_t deleteStartValueInResources(const ComRef& cref);

    oms_status_enu_t deleteReferencesInSSD(const std::string& filename);
    oms_status_enu_t deleteResourcesInSSP(const std::string& filename);

    oms_status_enu_t exportToSSV(pugi::xml_node& ssvNode) const;

    oms_status_enu_t exportUnitDefinitions(Snapshot &snapshot, std::string filename, std::string variantName) const;
    oms_status_enu_t exportUnitDefinitionsToSSVTemplate(Snapshot &snapshot, std::string filename);
    void importUnitDefinitions(const pugi::xml_node& node);
    void importEnumerationDefinitions(const pugi::xml_node& node, std::string& enumTypeName);

    void exportToSSVTemplate(pugi::xml_node& ssvNode, const ComRef& cref);  ///< start values read from modelDescription.xml and creates a ssv template
    void exportReduceSSV(pugi::xml_node& ssvNode, const ComRef& cref);  ///< reduced SSV file which contains only the referenced crefs in parametermapping

    oms_status_enu_t exportToSSMTemplate(pugi::xml_node& ssmNode, const ComRef& cref);  ///< start values read from modelDescription.xml and creates a ssm template

    oms_status_enu_t parseModelDescription(const filesystem::path& root, std::string& guid_); ///< path without the filename, i.e. modelDescription.xml
    oms_status_enu_t parseModelDescriptionFmi3(const filesystem::path& root, std::string& guid_); ///< path without the filename, i.e. modelDescription.xml
    oms_status_enu_t parseSlaveDescription(const std::string &dcpPath, std::string &guid_);

    oms_status_enu_t rename(const oms::ComRef& oldCref, const oms::ComRef& newCref);
    oms_status_enu_t renameInResources(const oms::ComRef& oldCref, const oms::ComRef& newCref);

    void exportParameterBindings(pugi::xml_node& node, Snapshot& snapshot, std::string variantName) const;

    bool hasResources(); ///< returns if the system or subsystem or submodule have parameter resources either as ssv or inline

  private:
    oms_status_enu_t exportStartValuesHelper(pugi::xml_node& node) const;
    void exportParameterMappingInline(pugi::xml_node& node) const;
    void exportParameterMappingToSSM(pugi::xml_node& node) const;
    oms_status_enu_t importStartValuesHelper(const pugi::xml_node& parameters);

    void parseModelStructureDependencies(std::string &dependencies, std::vector<int>& dependencyList);

    void importParameterMapping(const pugi::xml_node& parameterMapping);
    oms::ComRef getMappedCrefEntry(const ComRef& cref) const;
    bool isEntryReferencedInSSM(const ComRef& cref) const;
    bool empty() const;

  public:
    std::map<ComRef, bool> booleanStartValues;        ///< parameters and start values defined before instantiating the FMU
    std::map<ComRef, double> realStartValues;         ///< parameters and start values defined before instantiating the FMU
    std::map<ComRef, int> integerStartValues;         ///< parameters and start values defined before instantiating the FMU
    std::map<ComRef, std::string> stringStartValues;  ///< parameters and start values defined before instantiating the FMU

    std::map<ComRef, bool> booleanValues;        ///< input values defined after initialization
    std::map<ComRef, double> realValues;         ///< input values defined after initialization
    std::map<ComRef, int> integerValues;         ///< input values defined after initialization
    std::map<ComRef, std::string> stringValues;  ///< input values defined after initialization

    std::map<ComRef, bool> modelDescriptionBooleanStartValues;        ///< start values read from modelDescription.xml
    std::map<ComRef, double> modelDescriptionRealStartValues;         ///< start values read from modelDescription.xml
    std::map<ComRef, int> modelDescriptionIntegerStartValues;         ///< start values read from modelDescription.xml
    std::map<ComRef, std::string> modelDescriptionStringStartValues;  ///< start values read from modelDescription.xml

    std::map<int, std::vector<int>> modelStructureOutputs;            ///< output and its dependencies from <ModelStructure>
    std::map<int, std::vector<int>> modelStructureDerivatives;        ///< derivatives and its dependencies from <ModelStructure>
    std::map<int, std::vector<int>> modelStructureInitialUnknowns;    ///< initialUnknowns and its dependencies from <ModelStructure>

    std::map<int, bool> modelStructureOutputDependencyExist;
    std::map<int, bool> modelStructureDerivativesDependencyExist;
    std::map<int, bool> modelStructureInitialUnknownsDependencyExist;

    std::map<ComRef, std::string> modelDescriptionVariableUnits;  ///< variable units read from modeldescription.xml
    std::map<ComRef, std::string> variableUnits;  ///< variable units set by user

    uint8_t dcpId;

    struct unitDefinitionsToExport
    {
      std::string unitName;
      std::string unitValue;
      std::map<std::string, std::string> baseUnit;
      bool exportUnit;
    };
    std::vector<unitDefinitionsToExport> unitDefinitionsToExportInSSP; ///< list of unitDefinitions to be exported in ssp

    std::map<std::string, std::map<std::string, std::string>> modeldescriptionUnitDefinitions;  ///< <UnitDefinitions> list read from modeldescription.xml

    std::map<std::string, std::map<std::string, std::string>> modeldescriptionTypeDefinitions;  ///< <TypeDefinitions> list read from modeldescription.xml
    std::map<std::string, std::string> modeldescriptionEnumeration;  ///< enumeration declared type list list read from modeldescription.xml

    std::multimap<ComRef, ComRef> mappedEntry;  ///< parameter names and values provided in the parameter source are to be mapped to the parameters of the component or system

    std::vector<Values> parameterResources; ///< list of parameter resources provided inline or .ssv files
    std::map<std::string, Values> allresources; ///< mapped resources either inline or ssv
    std::string ssmFile = ""; ///< mapped ssm files associated with ssv files;
    bool linkResources = true;
  };
}

#endif
