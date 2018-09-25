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

#ifndef _OMS2_MODEL_H_
#define _OMS2_MODEL_H_

#include "ComRef.h"
#include "Types.h"
#include "Element.h"

#include <pugixml.hpp>

namespace oms3
{
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
    const ComRef& getName() const {return cref;}
    System* getSystem(const ComRef& cref);
    oms_status_enu_t rename(const ComRef& cref);
    oms_status_enu_t list(const ComRef& cref, char** contents);
    oms_status_enu_t addSystem(const ComRef& cref, oms_system_enu_t type);
    oms_status_enu_t exportToSSD(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD(const pugi::xml_node& node);
    oms_status_enu_t exportToFile(const std::string& filename) const;

    oms3::Element** getElements() {return &elements[0];}

  private:
    Model(const ComRef& cref, const std::string& tempDir);

    // stop the compiler generating methods copying the object
    Model(Model const& copy);            ///< not implemented
    Model& operator=(Model const& copy); ///< not implemented

  private:
    ComRef cref;
    System* system = NULL;
    std::string tempDir;

    std::vector<oms3::Element*> elements;
  };
}

#include "ResultWriter.h"
#include "Pkg_oms2.h"
#include "Element.h"
#include "CompositeModel.h"
#include "FMICompositeModel.h"
#include "TLMCompositeModel.h"
#include "ssd/SystemGeometry.h"

#include <string>

namespace oms2
{
  class Model
  {
  public:
    /**
     * NewModel() is used instead of a constructor to make sure that only
     * instances with valid names can be created.
     */
    static Model* NewModel(oms_element_type_enu_t type, const ComRef& cref);
    static oms_status_enu_t ParseString(const std::string& contents, char** ident);
    static Model* LoadModel(const std::string& filename);
    static void DeleteModel(Model *model) {if (model) delete model;}

    oms_status_enu_t save(const std::string& filename);
    oms_status_enu_t list(char** contents);

    void setStartTime(double value) {startTime = value;}
    double getStartTime() const {return startTime;}
    void setStopTime(double value) {stopTime = value;}
    double getStopTime() const {return stopTime;}
    void setCommunicationInterval(double value) {communicationInterval = value;}
    void setLoggingInterval(double value) {loggingInterval = value;}
    double getLoggingInterval() const {return loggingInterval;}
    void setLoggingSamples(int value);
    int getLoggingSamples();
    double getCommunicationInterval() const {return communicationInterval;}
    void setResultFile(const std::string& value, unsigned int bufferSize);
    const std::string& getResultFile() const {return resultFilename;}
    ResultWriter *getResultWriter() const {return resultFile;}
    void setMasterAlgorithm(MasterAlgorithm value) {masterAlgorithm = value;}
    MasterAlgorithm getMasterAlgorithm() const {return masterAlgorithm;}
    void setTolerance(double value) {tolerance = value;}
    double getTolerance() const {return tolerance;}

    FMICompositeModel* getFMICompositeModel();
#if !defined(NO_TLM)
    TLMCompositeModel* getTLMCompositeModel();
#endif

    oms_element_type_enu_t getType() {return compositeModel->getType();}
    oms2::Element* getElement() {return compositeModel->getElement();}
    const ComRef getName() const {return compositeModel->getName();}
    void setName(const ComRef& name) {compositeModel->setName(name);}

    oms_status_enu_t setTLMInitialValues(const SignalRef& ifc, std::vector<double> value);

    oms_status_enu_t describe() { return compositeModel->describe(); }
    oms_status_enu_t initialize();
    oms_status_enu_t reset(bool terminate=false);
    oms_status_enu_t simulate();
    oms_status_enu_t simulate_asynchronous(void (*cb)(const char* ident, double time, oms_status_enu_t status));
    oms_status_enu_t simulate_realtime();
    oms_status_enu_t doSteps(const int numberOfSteps);
    oms_status_enu_t stepUntil(const double timeValue);

  private:
    Model(const oms2::ComRef& cref);
    ~Model();

    // stop the compiler generating methods copying the object
    Model(Model const& copy);            ///< not implemented
    Model& operator=(Model const& copy); ///< not implemented

    oms_status_enu_t saveOrList(const std::string& filename, char** contents);

  private:
    oms2::ssd::SystemGeometry systemGeometry;
    CompositeModel* compositeModel = NULL;

    double startTime = 0.0;                 ///< experiment, default 0.0
    double stopTime = 1.0;                  ///< experiment, default 1.0
    double tolerance = 1.0e-4;              ///< experiment, default 1.0e-4
    double communicationInterval = 1.0e-2;  ///< experiment, default 1.0e-2
    double loggingInterval = 0.0;           ///< experiment, default 0.0
    std::string resultFilename;             ///< experiment, default <name>_res.mat
    unsigned int bufferSize = 1;
    ResultWriter *resultFile = NULL;
    MasterAlgorithm masterAlgorithm = MasterAlgorithm::STANDARD;  ///< master algorithm for FMI co-simulation, default MasterAlgorithm::STANDARD

    oms_modelState_enu_t modelState;  ///< internal model state, e.g. initialization state
  };
}

#endif
