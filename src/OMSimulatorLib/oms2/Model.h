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

#include "../Types.h"
#include "../ResultWriter.h"
#include "Pkg_oms2.h"
#include "ComRef.h"
#include "Element.h"
#include "CompositeModel.h"
#include "FMICompositeModel.h"
#include "TLMCompositeModel.h"
#include "ssd/SystemGeometry.h"

#include <string>

#include <pugixml.hpp>

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
    static Model* LoadModel(const std::string& filename);
    static void DeleteModel(Model *model) {if (model) delete model;}

    oms_status_enu_t save(const std::string& filename);

    void setStartTime(double value) {startTime = value;}
    double getStartTime() const {return startTime;}
    void setStopTime(double value) {stopTime = value;}
    double getStopTime() const {return stopTime;}
    void setCommunicationInterval(double value) {communicationInterval = value;}
    double getCommunicationInterval() const {return communicationInterval;}
    void setResultFile(const std::string& value) {resultFilename = value;}
    const std::string& getResultFile() const {return resultFilename;}
    void setMasterAlgorithm(MasterAlgorithm value) {masterAlgorithm = value;}
    MasterAlgorithm getMasterAlgorithm() const {return masterAlgorithm;}

    FMICompositeModel* getFMICompositeModel();
    TLMCompositeModel* getTLMCompositeModel();

    oms_element_type_enu_t getType() {return compositeModel->getType();}
    oms2::Element* getElement() {return compositeModel->getElement();}
    const ComRef getName() const {return compositeModel->getName();}
    void setName(const ComRef& name) {compositeModel->setName(name);}

    virtual oms_status_enu_t describe() { return compositeModel->describe(); }
    oms_status_enu_t initialize();
    oms_status_enu_t reset();
    oms_status_enu_t terminate();
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

  private:
    oms2::ssd::SystemGeometry systemGeometry;
    CompositeModel* compositeModel = nullptr;

    double startTime = 0.0;                 ///< experiment, default 0.0
    double stopTime = 1.0;                  ///< experiment, default 1.0
    double tolerance = 1.0e-4;              ///< experiment, default 1.0e-4
    double communicationInterval = 1.0e-2;  ///< experiment, default 1.0e-2
    std::string resultFilename;             ///< experiment, default <name>_res.mat
    ResultWriter *resultFile = nullptr;
    MasterAlgorithm masterAlgorithm = MasterAlgorithm::STANDARD;  ///< master algorithm for FMI co-simulation, default MasterAlgorithm::STANDARD

    oms_modelState_enu_t modelState;  ///< internal model state, e.g. initialization state
  };
}

#endif
