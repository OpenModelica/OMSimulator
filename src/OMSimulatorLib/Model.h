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

#ifndef _OMS_MODEL_H_
#define _OMS_MODEL_H_

#include "Clock.h"
#include "ComRef.h"
#include "Element.h"
#include "ResultWriter.h"
#include "OMSimulator/Types.h"
#include "Snapshot.h"
#include "Values.h"
#include <assert.h>
#include <pugixml.hpp>

#include <ctpl_stl.h>
#include <tuple>

namespace oms
{
  class Component;
  class System;

  class Model
  {
  public: // methods
    ~Model();

    /**
     * NewModel() is used instead of a constructor to make sure that only
     * instances with valid names can be created.
     */
    static Model* NewModel(const ComRef& cref);

    const ComRef& getCref() const {return cref;}
    System* getSystem(const ComRef& cref);
    oms_status_enu_t delete_(const ComRef& cref);
    Component* getComponent(const ComRef& cref);
    System* getTopLevelSystem() const {return system;}
    std::string getTempDirectory() const {return tempDir;}
    oms_status_enu_t rename(const ComRef& cref);
    oms_status_enu_t rename(const ComRef& cref, const ComRef& newCref);
    oms_status_enu_t list(const ComRef& cref, char** contents);
    oms_status_enu_t listVariants(const ComRef& cref, char** contents);
    oms_status_enu_t addSystem(const ComRef& cref, oms_system_enu_t type);
    oms_status_enu_t newResources(const ComRef& cref);
    oms_status_enu_t addResources(const ComRef& cref, const std::string& path);
    oms_status_enu_t deleteReferencesInSSD(const ComRef& cref);
    oms_status_enu_t deleteResourcesInSSP(const std::string& filename);
    oms_status_enu_t duplicateVariant(const ComRef& crefA, const ComRef& crefB);
    oms_status_enu_t activateVariant(const ComRef& crefA, const ComRef& crefB);
    oms_status_enu_t referenceResources(const ComRef& cref, const std::string& ssmFile);
    oms_status_enu_t reduceSSV(const std::string& ssvfile, const std::string& ssmfile, const std::string& filepath);
    oms_status_enu_t exportToSSD(Snapshot& snapshot) const;
    oms_status_enu_t exportSnapshot(const ComRef& cref, char** contents);
    oms_status_enu_t exportSSVTemplate(const ComRef& cref, const std::string& filename);
    oms_status_enu_t exportSSMTemplate(const ComRef& cref, const std::string& filename);
    void exportSignalFilter(Snapshot& snapshot) const;
    void exportUnitDefinitionsToSSD(pugi::xml_node& node) const;
    void exportEnumerationDefinitionsToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSnapshot(const Snapshot& snapshot);
    oms_status_enu_t importSnapshot(const char* snapshot, char** newCref);
    oms_status_enu_t importSignalFilter(const std::string& filename, const Snapshot& snapshot);
    oms_status_enu_t exportToFile(const std::string& filename) const;
    oms_system_enu_t getSystemType(const pugi::xml_node& node, const std::string& sspVersion);
    oms_system_enu_t getSystemTypeHelper(const pugi::xml_node& node, const std::string& sspVersion);
    oms_status_enu_t updateParameterBindingsToSSD(pugi::xml_node& node, bool isTopSystemOrModel) const;
    void copyResources(bool copy_resources) {this->copy_resources = copy_resources;}
    bool copyResources() {return copy_resources;}

    oms::Element** getElements() {return system ? &elements[0] : nullptr;}
    void writeAllResourcesToFilesystem(std::vector<std::string>& resources, Snapshot& snapshot) const;

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t simulate();
    oms_status_enu_t doStep();
    oms_status_enu_t stepUntil(double stopTime);
    oms_status_enu_t terminate();
    oms_status_enu_t reset();

    oms_modelState_enu_t getModelState() const {return modelState;}

    oms_status_enu_t setStartTime(double value);
    double getStartTime() const {return startTime;}
    oms_status_enu_t setStopTime(double value);
    double getStopTime() const {return stopTime;}
    double getTime() const;

    oms_status_enu_t setLoggingInterval(double loggingInterval);
    double getLoggingInterval() const {return loggingInterval;}
    oms_status_enu_t setResultFile(const std::string& filename, int bufferSize);
    oms_status_enu_t getResultFile(char** filename, int* bufferSize);
    oms_status_enu_t emit(double time, bool force=false, bool* emitted=NULL);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);
    std::string escapeSpecialCharacters(const std::string& regex);
    oms_status_enu_t addAlias(const ComRef& alias, const ComRef& rhs, oms_signal_type_enu_t type);

    bool validState(int validStates) const {return (modelState & validStates);}

    bool isIsolatedFMUModel() const {return isolatedFMU;}
    void setIsolatedFMUModel() {isolatedFMU = true;}

    bool useThreadPool() {return (pool != nullptr);}
    ctpl::thread_pool& getThreadPool() {assert(pool); return *pool;}

    oms_status_enu_t loadSnapshot(const pugi::xml_node& node);

    std::vector<std::string> importedResources;  ///< list of imported resources from ssp

    std::map<ComRef, char*> ssdVariants;  ///< list of all variants copied when user create a new variant using oms_duplicateVariant()

    std::string getVariantName() {return variantName;}

  private: // methods
    Model(const ComRef& cref, const std::string& tempDir);

    // stop the compiler generating methods copying the object
    Model(Model const& copy);            ///< not implemented
    Model& operator=(Model const& copy); ///< not implemented

    oms_status_enu_t registerSignalsForResultFile();

  private: // attributes
    ComRef cref;
    System* system = NULL;
    std::string tempDir;

    std::vector<oms::Element*> elements;
    bool copy_resources = true;

    oms_modelState_enu_t modelState = oms_modelState_virgin;

    // ssd:DefaultExperiment
    double startTime = 0.0;
    double stopTime = 1.0;

    ResultWriter* resultFile = NULL;
    double lastEmit;
    double loggingInterval = 0.0;
    int bufferSize = 10;
    Clock clock;

    Values values;

    std::string resultFilename; ///< default <name>_res.mat
    std::string signalFilterFilename = "resources/signalFilter.xml";

    std::string variantName = "SystemStructure.ssd";  ///< default name

    std::vector<std::string> externalResources;  ///< list of external ssv or ssm resources from filesystem

  std::map<ComRef, std::tuple<ComRef, oms_signal_type_enu_t, int>> aliases;                 ///< list of aliases: (rhs, type, result_file_id)

    bool isolatedFMU = false;

    ctpl::thread_pool* pool = nullptr;

    ComRef new_root_cref;
  };
}

#endif
