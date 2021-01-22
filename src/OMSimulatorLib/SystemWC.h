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

#ifndef _OMS_SYSTEM_WC_H_
#define _OMS_SYSTEM_WC_H_

#include "ComRef.h"
#include "DirectedGraph.h"
#include "SignalDerivative.h"
#include "StepSizeConfiguration.h"
#include "System.h"
#include "Types.h"
#include <vector>

namespace oms
{
  class Model;

  class SystemWC : public System
  {
  public:
    ~SystemWC();

    static System* NewSystem(const ComRef& cref, Model* parentModel, System* parentSystem);
    oms_status_enu_t exportToSSD_SimulationInformation(pugi::xml_node& node) const;
    oms_status_enu_t importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion);

    oms_status_enu_t instantiate();
    oms_status_enu_t initialize();
    oms_status_enu_t terminate();
    oms_status_enu_t reset();
    oms_status_enu_t stepUntil(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status));
    oms_status_enu_t stepUntilASSC(double stopTime, void (*cb)(const char* ident, double time, oms_status_enu_t status));

    std::string getSolverName() const;
    oms_status_enu_t setSolverMethod(std::string);
    oms_status_enu_t setSolver(oms_solver_enu_t solver) {if (solver > oms_solver_wc_min && solver < oms_solver_wc_max) {solverMethod=solver; return oms_status_ok;} return oms_status_error;}

    oms_status_enu_t getInputs(DirectedGraph& graph, std::vector<double>& inputs);
    oms_status_enu_t setInputsDer(oms::DirectedGraph& graph, const std::vector<double>& inputsDer);
    oms_status_enu_t getInputAndOutput(DirectedGraph& graph, std::vector<double>& inputVect,std::vector<double>& outputVect,std::map<ComRef, Component*> FMUcomponents);
    oms_status_enu_t updateInputs(DirectedGraph& graph);

    oms_status_enu_t getRealOutputDerivative(const ComRef& cref, SignalDerivative& der);
    oms_status_enu_t setRealInputDerivative(const ComRef& cref, const SignalDerivative& der);
    unsigned int getMaxOutputDerivativeOrder();

    // Functions for configuring assc
    oms_status_enu_t addEventIndicator(const ComRef& signal) {return stepSizeConfiguration.addEventIndicator(signal);}
    oms_status_enu_t addTimeIndicator(const ComRef& signal) {return stepSizeConfiguration.addTimeIndicator(signal);}
    oms_status_enu_t addStaticValueIndicator(const ComRef& signal, double lowerBound, double upperBound, double stepSize) {return stepSizeConfiguration.addStaticValueIndicator(signal, lowerBound, upperBound, stepSize);}
    oms_status_enu_t addDynamicValueIndicator(const ComRef& signal,const ComRef& lower,const ComRef& upper,double stepSize) {return stepSizeConfiguration.addDynamicValueIndicator(signal, lower, upper, stepSize);}

    oms_status_enu_t registerSignalsForResultFile(ResultWriter& resultFile);
    oms_status_enu_t updateSignals(ResultWriter& resultFile);

  protected:
    SystemWC(const ComRef& cref, Model* parentModel, System* parentSystem);

    // stop the compiler generating methods copying the object
    SystemWC(SystemWC const& copy);            ///< not implemented
    SystemWC& operator=(SystemWC const& copy); ///< not implemented

  private:
    StepSizeConfiguration stepSizeConfiguration;  ///< Configuration data structure for assc

    unsigned int h_id;
    unsigned int roll_iter_id;
    unsigned int max_error_id;
    unsigned int error_id;
    double stepSize = 1e-4; // Default system stepsize. So first logging step works.
    double maxError = 0.0;
    double normError = 0.0;
    unsigned int rollBackIt = 0;
  };
}

#endif
