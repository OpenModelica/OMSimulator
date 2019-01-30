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
 * even the implied warranty of  MERCHANTABILITY or FITNESS
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
#include "Types.h"

#include <pugixml.hpp>

namespace oms
{
  class Component;
  class System;

  class Model
  {
  public:
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
    oms_status_enu_t list(const ComRef& cref, char** contents);
    oms_status_enu_t addSystem(const ComRef& cref, oms_system_enu_t type);
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD(const pugi::xml_node& node);
    oms_status_enu_t exportToFile(const std::string& filename) const;
    void copyResources(bool copy_resources) {this->copy_resources = copy_resources;}
    bool copyResources() {return copy_resources;}

    oms::Element** getElements() {return &elements[0];}
    oms_status_enu_t getAllResources(std::vector<std::string>& resources) const;

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t simulate_asynchronous(void (*cb)(const char* cref, double time, oms_status_enu_t status));
    oms_status_enu_t simulate();
    oms_status_enu_t stepUntil(double stopTime);
    oms_status_enu_t terminate();
    oms_status_enu_t reset();

    oms_modelState_enu_t getModelState() const {return modelState;}

    oms_status_enu_t setStartTime(double value);
    double getStartTime() const {return startTime;}
    oms_status_enu_t setStopTime(double value);
    double getStopTime() const {return stopTime;}

    oms_status_enu_t setLoggingInterval(double loggingInterval);
    double getLoggingInterval() const {return loggingInterval;}
    oms_status_enu_t setResultFile(const std::string& filename, int bufferSize);
    oms_status_enu_t emit(double time, bool force=false);
    oms_status_enu_t setStepAndRollIterator(double stepSize,unsigned int rollBackIt, double maxError, double normError);
    oms_status_enu_t addSignalsToResults(const char* regex);
    oms_status_enu_t removeSignalsFromResults(const char* regex);

    oms_status_enu_t cancelSimulation_asynchronous();
    bool cancelSimulation() const {return cancelSim;}

    bool validState(int validStates) const {return (modelState & validStates);}

    bool isIsolatedFMUModel() const {return isolatedFMU;}
    void setIsolatedFMUModel() {isolatedFMU = true;}

  private:
    Model(const ComRef& cref, const std::string& tempDir);

    // stop the compiler generating methods copying the object
    Model(Model const& copy);            ///< not implemented
    Model& operator=(Model const& copy); ///< not implemented

    oms_status_enu_t registerSignalsForResultFile();

  private:
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
    std::string resultFilename;             ///< default <name>_res.mat
    Clock clock;
    unsigned int clock_id;
    unsigned int h_id;
    unsigned int roll_iter_id;
    unsigned int max_error_id;
    unsigned int error_id;
    double stepSize = 1e-4; // Default system stepsize. So first logging step works.
    double maxError = 0.0;
    double normError = 0.0;
    unsigned int rollBackIt = 0;

    bool cancelSim;
    bool isolatedFMU = false;
  };
}

#endif
