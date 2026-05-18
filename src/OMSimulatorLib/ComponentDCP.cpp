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

#include "ComponentDCP.h"

#include "Flags.h"
#include "Logging.h"
#include "Model.h"
#include "OMSFileSystem.h"
#include "ssd/Tags.h"
#include "System.h"
#include "SystemWC.h"
#include "Scope.h"
#include "dcp/zip/DcpSlaveReader.hpp"
#include "dcp/log/OstreamLog.hpp"
#include "dcp/logic/DcpManagerMaster.hpp"

#include <fmi4c.h>
#include <regex>
#include <unordered_set>
#include <cmath>
#include <iostream>


oms::ComponentDCP::ComponentDCP(const ComRef& cref, System* parentSystem, const std::string& dcpPath)
    : oms::Component(cref, oms_component_dcp, parentSystem, dcpPath), fmuInfo(dcpPath)
{
}

oms::ComponentDCP::~ComponentDCP()
{
}

oms::Component* oms::ComponentDCP::NewComponent(const oms::ComRef& cref, oms::System* parentSystem, const std::string& dcpPath, std::string replaceComponent)
{
    if (!cref.isValidIdent())
    {
        logError_InvalidIdent(cref);
        return NULL;
    }

    if (!parentSystem)
    {
        logError_InternalError;
        return NULL;
    }
    // replaceComponent string will be used to avoid name conflicts when replacing a fmu with oms_replaceSubModel(), the default is ""

    filesystem::path temp_root(parentSystem->getModel().getTempDirectory());
    filesystem::path temp_temp = temp_root / "temp";
    filesystem::path relDCPPath = parentSystem->copyResources() ? (filesystem::path("resources") / (parentSystem->getUniqueID() + "_" + replaceComponent + std::string(cref) + ".dcp")) : filesystem::path(dcpPath);
    filesystem::path absDCPPath = temp_root / relDCPPath;

    ComponentDCP* component = new ComponentDCP(cref, parentSystem, relDCPPath.generic_string());

    /* parse the modeldescription.xml at top level to get the GUID to check whether instance already exist
   * so we don't need to unpack the fmu, and also parse start values before instantiating fmu's
  */
    std::string guid_ = "";
    filesystem::path slaveDescriptionPath;
    /*
  * check for modeldescription path from file system or temp directory
  * because when importingSnapshot the path will be resources/0001_tank1.fmu
  */

    if (parentSystem->copyResources())
        slaveDescriptionPath = dcpPath;
    else
        slaveDescriptionPath = parentSystem->getModel().getTempDirectory() / filesystem::path(dcpPath);

    component->values.parseSlaveDescription(dcpPath, guid_);

    // update DCP info
    // TODO: Is this needed? (dcp)
    component->fmuInfo.update(oms_component_dcp, nullptr);

    // create a list of all variables using fmi4c variable structure
    component->desc = getSlaveDescriptionFromDcpFile(1, 0, dcpPath).get();

    int numVars = component->desc->Variables.size();
    component->allVariables.reserve(numVars);
    component->exportVariables.reserve(numVars);
    for (unsigned int i = 0; i < numVars; ++i)
    {
        oms::Variable v(component->desc, i);
         //logInfo("vars: " + std::string(v.getCref().c_str()));
         if (v.getIndex() != i)
         {

             logError("Index mismatch " + std::to_string(v.getIndex()) + " != " + std::to_string(i) + ".\nPlease report the problem to the dev team: https://github.com/OpenModelica/OMSimulator/issues/new?assignees=&labels=&template=bug_report.md");
             delete component;
             return NULL;
         }

         component->allVariables.push_back(v);
         component->exportVariables.push_back(true);
    }

    // create some special variable maps
     for (auto const& v : component->allVariables)
     {
         if (v.isInput())
             component->inputs.push_back(v.getIndex());
         else if (v.isOutput())
         {
             component->outputs.push_back(v.getIndex());
             component->outputsGraph.addNode(Connector(oms_causality_output, v.getType(), v.getCref(), component->getFullCref()));
         }
         else if (v.isParameter())
             component->parameters.push_back(v.getIndex());
         else if (v.isCalculatedParameter())
             component->calculatedParameters.push_back(v.getIndex());

         if (v.isInitialUnknown())
             component->initialUnknownsGraph.addNode(Connector(v.getCausality(), v.getType(), v.getCref(), component->getFullCref()));

         component->exportVariables.push_back(v.isInput() || v.isOutput());
    }

    // create connectors
    while (component->connectors.size() > 0 && NULL == component->connectors.back())
        component->connectors.pop_back();

    int j = 1;
    int size = 1 + component->inputs.size();
    for (const auto& i : component->inputs)
        component->connectors.push_back(new Connector(oms_causality_input, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref(), j++/(double)size));
    j = 1;
    size = 1 + component->outputs.size();
    for (const auto& i : component->outputs)
        component->connectors.push_back(new Connector(oms_causality_output, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref(), j++/(double)size));
    for (const auto& i : component->parameters)
        component->connectors.push_back(new Connector(oms_causality_parameter, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref()));
    for (const auto& i : component->calculatedParameters)
        component->connectors.push_back(new Connector(oms_causality_calculatedParameter, component->allVariables[i].getType(), component->allVariables[i].getCref(), component->getFullCref()));
    component->connectors.push_back(NULL);
    component->element.setConnectors(&component->connectors[0]);

    if (oms_status_ok != component->initializeDependencyGraph_outputs())
    {
        logError(std::string(cref) + ": Couldn't initialize dependency graph for simulation unknowns.");
        delete component;
        return NULL;
    }

    // set units to connector
    for (auto &connector : component->connectors)
    {
        if (connector)
        {
            oms::ComRef connectorCref = connector->getName();
            std::string unitName = component->values.getUnitFromModeldescription(connectorCref);
            if (!unitName.empty())
                connector->connectorUnits[unitName] = component->values.modeldescriptionUnitDefinitions[unitName];

            // get enumerationTypes
            std::string enumType = component->values.getEnumerationTypeFromModeldescription(connectorCref);
            if (!enumType.empty())
                connector->enumerationName[connectorCref] = enumType;
         }
    }

    return component;
}

oms::Component* oms::ComponentDCP::NewComponent(const pugi::xml_node& node, oms::System* parentSystem, const std::string& sspVersion, const Snapshot& snapshot, std::string variantName)
{
    ComRef cref = ComRef(node.attribute("name").as_string());
    std::string type = node.attribute("type").as_string();
    std::string source = node.attribute("source").as_string();

    if (type != "application/x-fmu-sharedlibrary" && !type.empty())
    {
        logError("Unexpected component type: " + type);
        return NULL;
    }

    oms::ComponentDCP* component = dynamic_cast<oms::ComponentDCP*>(oms::ComponentDCP::NewComponent(cref, parentSystem, source));
    if (!component)
        return NULL;

    for (const auto& connector : component->connectors)
        if (connector)
            delete connector;
    component->connectors.clear();
    for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string name = it->name();
        if(name == oms::ssp::Draft20180219::ssd::connectors)
        {
            // get the ssdNode to parse UnitDefinitions in "SystemStructure.ssd"
            pugi::xml_node ssdNode = snapshot.getResourceNode(variantName);
            component->values.importUnitDefinitions(ssdNode);
            // import connectors
            for(pugi::xml_node_iterator itConnectors = (*it).begin(); itConnectors != (*it).end(); ++itConnectors)
            {
                component->connectors.push_back(oms::Connector::NewConnector(*itConnectors, sspVersion, component->getFullCref()));
                // set units to connector
                if ((*itConnectors).child(oms::ssp::Version1_0::ssc::real_type))
                {
                    std::string unitName = (*itConnectors).child(oms::ssp::Version1_0::ssc::real_type).attribute("unit").as_string();
                    if (!unitName.empty())
                        component->connectors.back()->connectorUnits[unitName] = component->values.modeldescriptionUnitDefinitions[unitName];
                }
                // set enumeration definitions
                if ((*itConnectors).child(oms::ssp::Version1_0::ssc::enumeration_type))
                {
                    std::string enumTypeName = (*itConnectors).child(oms::ssp::Version1_0::ssc::enumeration_type).attribute("name").as_string();
                    if (!enumTypeName.empty())
                        component->connectors.back()->enumerationName[component->connectors.back()->getName().c_str()] = enumTypeName;

                    // give priority to enum definitions in ssd over modeldescription.xml, it is possible the user might have manually change values in ssd file
                    component->values.importEnumerationDefinitions(ssdNode, enumTypeName);
                }
            }
        }
        else if(name == oms::ssp::Draft20180219::ssd::element_geometry)
        {
            oms::ssd::ElementGeometry geometry;
            geometry.importFromSSD(*it);
            component->setGeometry(geometry);
        }
        else if(name == oms::ssp::Version1_0::ssd::parameter_bindings)
        {
            // set parameter bindings associated with the component
            Values resources;
            std::string tempdir = parentSystem->getModel().getTempDirectory();
            resources.importFromSnapshot(*it, sspVersion, snapshot, variantName);
            component->values.parameterResources.push_back(resources);
        }
        else
        {
            logError_WrongSchema(name);
            delete component;
            return NULL;
        }
    }

    component->connectors.push_back(NULL);
    component->element.setConnectors(&component->connectors[0]);

    return component;
}

oms_status_enu_t oms::ComponentDCP::addSignalsToResults(const char *regex)
{
    std::regex exp(regex);
    for (unsigned int i=0; i<allVariables.size(); ++i)
    {
        if (exportVariables[i])
            continue;

        auto const &var = allVariables[i];
        if(regex_match(std::string(getFullCref() + var.getCref()), exp))
        {
            //logInfo("added \"" + std::string(getFullCref() + var.getCref()) + "\" to results");
            exportVariables[i] = true;
        }
    }

    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::exportToSSD(pugi::xml_node &node, Snapshot &snapshot, std::string variantName) const
{
    // std::regex exp(regex);
    // for (unsigned int i=0; i<allVariables.size(); ++i)
    // {
    //     if (!exportVariables[i])
    //         continue;

    //     auto const &var = allVariables[i];
    //     if(regex_match(std::string(getFullCref() + var.getCref()), exp))
    //     {
    //         //logInfo("removed \"" + std::string(getFullCref() + var.getCref()) + "\" from results");
    //         exportVariables[i] = false;
    //     }
    // }

    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::initialize()
{
    //Nothing to be done (I think?)
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::instantiate()
{
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::registerSignalsForResultFile(ResultWriter &resultFile)
{
    logInfo("Enter registerSignalsForResultFile for " + std::string(getFullCref())); //DCP debug

    // Not sure if this is needed
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::removeSignalsFromResults(const char *regex)
{
    // Not sure if this is needed
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::reset()
{
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::terminate()
{
    return oms_status_ok;
}

oms_status_enu_t oms::ComponentDCP::updateSignals(ResultWriter &resultWriter)
{
    //Not sure if this is needed
    return oms_status_ok;
}

void oms::ComponentDCP::getFilteredSignals(std::vector<Connector> &filteredSignals) const
{
    // TODO: Implement (dcp)
}

oms::Variable *oms::ComponentDCP::getVariable(const ComRef &cref)
{
    // TODO: Implement (dcp)
    return nullptr;
}

oms_status_enu_t oms::ComponentDCP::initializeDependencyGraph_outputs()
{
    // TODO: Does this work? (dcp)

    if (outputsGraph.getEdges().connections.size() > 0)
    {
        logError(std::string(getCref()) + ": " + getPath() + " is already initialized.");
        return oms_status_error;
    }

    // if (!startIndex)
    // {
    //   logDebug(std::string(getCref()) + ": " + getPath() + " no dependencies");
    //   return oms_status_ok;
    // }

    // get the output dependencies
    int i = 0;
    for (const auto &it : values.modelStructureOutputs)
    {
        Variable& output = allVariables[outputs[i]];
        // no dependencies
        if (it.second.empty() && values.modelStructureOutputDependencyExist[it.first])
        {
            logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " has no dependencies");
        }
        // dependency attribute not provided in modeldescription.xml, all output depends on all inputs
        else if (it.second.empty() && !values.modelStructureOutputDependencyExist[it.first])
        {
            logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on all");
            for (const auto& j : inputs)
                outputsGraph.addEdge(allVariables[j].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
        }
        else
        {
            for (const auto &index : it.second)
            {
                if (index < 1 || index > allVariables.size())
                {
                    logWarning("Output " + std::string(output) + " has bad dependency on variable with index " + std::to_string(index) + " which couldn't be resolved");
                    return logError(std::string(getCref()) + ": erroneous dependencies detected in modelDescription.xml");
                }
                logDebug(std::string(getCref()) + ": " + getPath() + " output " + std::string(output) + " depends on " + std::string(allVariables[index - 1]));
                outputsGraph.addEdge(allVariables[index - 1].makeConnector(this->getFullCref()), output.makeConnector(this->getFullCref()));
            }
        }
        i = i + 1;
    }

    return oms_status_ok;
}
