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

#include "SystemSC3.h"

#include "Component.h"
#include "ComponentFMU3ME.h"
#include "ComponentTable.h"
#include "Flags.h"
#include "Model.h"
#include "ssd/Tags.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include <iostream>

int oms::cvode_rhs3(realtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SystemSC3* system = (SystemSC3*)user_data;
  oms_status_enu_t status;
  fmi3Status fmistatus;

  // update states in FMUs
  for (size_t i=0, j_y=0, j_ydot=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 == system->nStates[i])
      continue;

    for (size_t k = 0; k < system->nStates[i]; k++, j_y++)
      system->states[i][k] = NV_Ith_S(y, j_y);

    // set states
    status = system->fmus[i]->setContinuousStates(system->states[i]);
    if (oms_status_ok != status) return status;

    // get state derivatives
    status = system->fmus[i]->getDerivatives(system->states_der[i]);
    if (oms_status_ok != status) return status;

    for (size_t k=0; k < system->nStates[i]; ++k, ++j_ydot)
      NV_Ith_S(ydot, j_ydot) = system->states_der[i][k];
  }

  return 0;
}

int oms::cvode_rhs_algebraic3(realtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SystemSC3* system = (SystemSC3*)user_data;

  for (size_t i=0; i < system->fmus.size(); ++i)
    system->fmus[i]->setTime(t);

  NV_Ith_S(ydot, 0) = 0.0;

  return 0;
}

int oms::cvode_roots3(realtype t, N_Vector y, realtype *gout, void *user_data)
{
  logDebug("cvode_roots at time " + std::to_string(t));
  SystemSC3* system = (SystemSC3*)user_data;
  oms_status_enu_t status;
  fmi3Status fmistatus;

  for (size_t i=0, j_y=0, j_gout=0; i < system->fmus.size(); ++i)
  {
    system->fmus[i]->setTime(t);

    if (0 != system->nStates[i])
    {
      for (size_t k = 0; k < system->nStates[i]; k++, j_y++)
        system->states[i][k] = NV_Ith_S(y, j_y);

      // set states
      status = system->fmus[i]->setContinuousStates(system->states[i]);
      if (oms_status_ok != status) return status;
    }

    fmistatus = fmi3_getEventIndicators(system->fmus[i]->getFMU(), system->event_indicators[i], 1);
    if (fmi3OK != fmistatus) logError_FMUCall("fmi3_getEventIndicators", system->fmus[i]);

    for (size_t k=0; k < system->nEventIndicators[i]; k++, j_gout++)
      gout[j_gout] = system->event_indicators[i][k];
  }

  return 0;
}

oms::SystemSC3::SystemSC3(const ComRef& cref, Model* parentModel, System* parentSystem)
  : oms::System(cref, oms_system_sc, parentModel, parentSystem, oms_solver_sc_cvode)
{
}

oms::SystemSC3::~SystemSC3()
{
}

oms::System* oms::SystemSC3::NewSystem(const oms::ComRef& cref, oms::Model* parentModel, oms::System* parentSystem)
{
  std::cout << "Creating SystemSC3: " << cref.c_str() << std::endl;
  if (!cref.isValidIdent())
  {
    logError_InvalidIdent(cref);
    return NULL;
  }

  if ((parentModel && parentSystem) || (!parentModel && !parentSystem))
  {
    logError_InternalError;
    return NULL;
  }

  System* system = new SystemSC3(cref, parentModel, parentSystem);
  return system;
}

std::string oms::SystemSC3::getSolverName() const
{
  switch (solverMethod)
  {
    case oms_solver_sc_explicit_euler:
      return std::string("euler");
    case oms_solver_sc_cvode:
      return std::string("cvode");
    default:
      return std::string("unknown");
  }
}

oms_status_enu_t oms::SystemSC3::setSolverMethod(std::string solver)
{
  if (std::string("euler") == solver)
    solverMethod = oms_solver_sc_explicit_euler;
  else if (std::string("cvode") == solver)
    solverMethod = oms_solver_sc_cvode;
  else
    return oms_status_error;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::exportToSSD_SimulationInformation(pugi::xml_node& node) const
{
  std::ostringstream ssAbsoluteTolerance, ssRelativeTolerance, ssMinimumStepSize, ssMaximumStepSize, ssInitialStepSize;
  ssRelativeTolerance << relativeTolerance;
  ssMinimumStepSize << minimumStepSize;
  ssMaximumStepSize << maximumStepSize;
  ssInitialStepSize << initialStepSize;

  /* ssd:SimulationInformation should be added as vendor specific annotations from Version 1.0 */
  pugi::xml_node node_simulation_information = node.append_child(oms::ssp::Version1_0::simulation_information);

  pugi::xml_node node_solver = node_simulation_information.append_child(oms::ssp::Version1_0::VariableStepSolver);
  node_solver.append_attribute("description") = getSolverName().c_str();
  node_solver.append_attribute("relativeTolerance") = ssRelativeTolerance.str().c_str();
  node_solver.append_attribute("minimumStepSize") = ssMinimumStepSize.str().c_str();
  node_solver.append_attribute("maximumStepSize") = ssMaximumStepSize.str().c_str();
  node_solver.append_attribute("initialStepSize") = ssInitialStepSize.str().c_str();

  return oms_status_ok;
}


oms_status_enu_t oms::SystemSC3::testFMI3ME(fmiHandle *fmu)
{
    double startTime = 0.0;
    double stepSize  = 0.001;
    double stopTime  = 4.0;
    double tolerance = 1e-6;

    fmi3Status status;

    /*------------------------------------------------------------
     * Setup & Initialization
     *------------------------------------------------------------*/
    // status = fmi3_setupExperiment(
    //     fmu,
    //     fmi3True,
    //     tolerance,
    //     startTime,
    //     fmi3True,
    //     stopTime
    // );
    // if (status != fmi3OK) return oms_status_error;

    // status = fmi3_enterInitializationMode(fmu);
    // if (status != fmi3OK) return oms_status_error;

    status = fmi3_exitInitializationMode(fmu);
    if (status != fmi3OK) return oms_status_error;

    printf("FMU successfully initialized.\n");

    /*------------------------------------------------------------
     * Query sizes
     *------------------------------------------------------------*/
    size_t nStates_;
    size_t nEventIndicators_;

    fmi3_getNumberOfContinuousStates(fmu, &nStates_);
    fmi3_getNumberOfEventIndicators(fmu, &nEventIndicators_);
    std::cout << "\n Number of States fmi4c : " << nStates_ << std::endl;
    std::cout << "\n Number of Event Indicators fmi4c : " << nEventIndicators_ << std::endl;
    fmi3Float64 *states_              = (fmi3Float64*) malloc(nStates_ * sizeof(fmi3Float64));
    fmi3Float64 *derivatives_         = (fmi3Float64*) malloc(nStates_ * sizeof(fmi3Float64));
    fmi3Float64 *eventIndicators_     = (fmi3Float64*) malloc(nEventIndicators_ * sizeof(fmi3Float64));
    fmi3Float64 *eventIndicatorsPrev_ = (fmi3Float64*) malloc(nEventIndicators_ * sizeof(fmi3Float64));

    fmi3Int32  *rootsFound = (fmi3Int32*) malloc(nEventIndicators_ * sizeof(fmi3Int32));

    for (size_t i = 0; i < nEventIndicators_; ++i) {
        eventIndicatorsPrev_[i] = 0.0;
        rootsFound[i] = 0;
    }

    /*------------------------------------------------------------
     * Enter Continuous-Time Mode
     *------------------------------------------------------------*/
    fmi3_enterContinuousTimeMode(fmu);

    fmi3_getContinuousStates(fmu, states_, nStates_);
    fmi3_getEventIndicators(fmu, eventIndicatorsPrev_, nEventIndicators_);

    FILE *outputFile = NULL;
    //Prepare output file
    const char* outputCsvPath = "fmi3me_output.csv";
    if(outputCsvPath != NULL) {
        outputFile = fopen(outputCsvPath, "w");
        if(outputFile != NULL) {
            fprintf(outputFile,"time");
            for(int i=0; i<1; ++i) {
                fprintf(outputFile,",%s",fmi3_getVariableName(fmi3_getVariableByValueReference(fmu, 1)));
                fprintf(outputFile,",%s",fmi3_getVariableName(fmi3_getVariableByValueReference(fmu, 5)));
                fprintf(outputFile,",%s",fmi3_getVariableName(fmi3_getVariableByValueReference(fmu, 6)));

            }
            fprintf(outputFile,"\n");
        }
    }

    printf("  Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);

    /*------------------------------------------------------------
     * Simulation loop
     *------------------------------------------------------------*/
    fmi3Float64 time = startTime;
    fmi3Boolean terminateSimulation = fmi3False;
    fmi3Boolean stepEvent, stateEvent;

    while (time <= stopTime && !terminateSimulation)
    {
        /* 1. Set time FIRST */
        fmi3_setTime(fmu, time);

        /* 2. Get derivatives */
        fmi3_getContinuousStateDerivatives(fmu, derivatives_, nStates_);

        /* 3. Explicit Euler */
        for (size_t i = 0; i < nStates_; ++i)
            states_[i] += stepSize * derivatives_[i];

        fmi3_setContinuousStates(fmu, states_, nStates_);

        /* 4. Tell FMU step completed */
        fmi3_completedIntegratorStep(
            fmu,
            fmi3True,
            &stepEvent,
            &terminateSimulation
        );

        /* 5. Event indicators */
        fmi3_getEventIndicators(fmu, eventIndicators_, nEventIndicators_);

        stateEvent = fmi3False;
        for (size_t i = 0; i < nEventIndicators_; ++i)
        {
            if ((eventIndicatorsPrev_[i] < 0 && eventIndicators_[i] >= 0) ||
                (eventIndicatorsPrev_[i] > 0 && eventIndicators_[i] <= 0))
            {
                rootsFound[i] = 1;
                stateEvent = fmi3True;
            }
            else
            {
                rootsFound[i] = 0;
            }

            eventIndicatorsPrev_[i] = eventIndicators_[i];
        }

        /*--------------------------------------------------------
         * 6. EVENT HANDLING (THIS WAS MISSING)
         *--------------------------------------------------------*/
        if (stateEvent || stepEvent)
        {
            fmi3Boolean discreteStatesNeedUpdate = fmi3True;
            fmi3Boolean nominalsChanged;
            fmi3Boolean valuesChanged;
            fmi3Boolean nextEventTimeDefined;
            fmi3Float64 nextEventTime;

            fmi3_enterEventMode(fmu);

            while (discreteStatesNeedUpdate)
            {
                fmi3_updateDiscreteStates(
                    fmu,
                    &discreteStatesNeedUpdate,
                    &terminateSimulation,
                    &nominalsChanged,
                    &valuesChanged,
                    &nextEventTimeDefined,
                    &nextEventTime
                );
            }

            fmi3_enterContinuousTimeMode(fmu);

            /* CRITICAL: get updated states after bounce */
            fmi3_getContinuousStates(fmu, states_, nStates_);
            fmi3_getEventIndicators(fmu, eventIndicatorsPrev_, nEventIndicators_);
        }

         /*--------------------------------------------------------
         * WRITE OUTPUT (after events!)
         *--------------------------------------------------------*/
        // Print all output variables to CSV file
        double value;
        if (outputFile != NULL)
        {
          fprintf(outputFile, "%f", time);
          fmi3ValueReference vr1 = 1;
          fmi3ValueReference vr2 = 5;
          fmi3ValueReference vr3 = 6;

          for (int i = 0; i < 1; ++i)
          {
            fmi3_getFloat64(fmu, &vr1, 1, &value, 1);
            fprintf(outputFile, ",%f", value);
            fmi3_getFloat64(fmu, &vr2, 1, &value, 1);
            fprintf(outputFile, ",%f", value);
            fmi3_getFloat64(fmu, &vr3, 1, &value, 1);
            // std::cout << "info: time=" << time << ", var" << i << "=" << value << std::endl;
            fprintf(outputFile, ",%f", value);
          }
          fprintf(outputFile, "\n");
        }

        time += stepSize;
    }

    if (outputFile != NULL) {
        fclose(outputFile);
    }

    /*------------------------------------------------------------
     * Cleanup
     *------------------------------------------------------------*/
    //fmi3_terminate(fmu);

    free(states_);
    free(derivatives_);
    free(eventIndicators_);
    free(eventIndicatorsPrev_);
    free(rootsFound);

    printf("Simulation finished.\n");
    return oms_status_ok;
}


oms_status_enu_t oms::SystemSC3::importFromSSD_SimulationInformation(const pugi::xml_node& node, const std::string& sspVersion)
{
  std::string solverName = "";
  const char* VariableStepSolver = "";

  pugi::xml_node variableStepSolver = node.child(oms::ssp::Version1_0::VariableStepSolver);
  if (variableStepSolver)
  {
    solverName = variableStepSolver.attribute("description").as_string();
    VariableStepSolver = oms::ssp::Version1_0::VariableStepSolver;
  }
  else
  {
    solverName = node.child("VariableStepSolver").attribute("description").as_string();
    VariableStepSolver = "VariableStepSolver";
  }

  if (oms_status_ok != setSolverMethod(solverName))
    return oms_status_error;

  relativeTolerance = node.child(VariableStepSolver).attribute("relativeTolerance").as_double();
  minimumStepSize = node.child(VariableStepSolver).attribute("minimumStepSize").as_double();
  maximumStepSize = node.child(VariableStepSolver).attribute("maximumStepSize").as_double();
  initialStepSize = node.child(VariableStepSolver).attribute("initialStepSize").as_double();
  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::instantiate()
{
  time = getModel().getStartTime();

  // there shouldn't be any subsystem
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->instantiate())
      return oms_status_error;

  size_t n_states = 0;
  int count = 0;
  for (const auto& component : getComponents())
  {
    if (oms_status_ok != component.second->instantiate())
      return oms_status_error;

    if (component.second->getType() == oms_component_fmu3)
    {
      callEventUpdate[count] = fmi3False;
      terminateSimulation[count] = fmi3False;
      fmus.push_back(dynamic_cast<ComponentFMU3ME*>(component.second));
      nStates.push_back(fmus.back()->getNumberOfContinuousStates());
      n_states += nStates.back();
      nEventIndicators.push_back(fmus.back()->getNumberOfEventIndicators());
      states.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_der.push_back((double*)calloc(nStates.back(), sizeof(double)));
      states_nominal.push_back((double*)calloc(nStates.back(), sizeof(double)));
      event_indicators.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
      event_indicators_prev.push_back((double*)calloc(nEventIndicators.back(), sizeof(double)));
    }
    count++;
  }

  if (n_states == 0)
    logInfo("model doesn't contain any continuous state");

  if (oms_solver_sc_explicit_euler == solverMethod)
    ;
  else if (oms_solver_sc_cvode == solverMethod)
    solverData.cvode.mem = nullptr;
  else
    return logError_InternalError;

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::initialize()
{
  // testFMI3ME(fmus[0]->getFMU());
  // exit(0);
  clock.reset();
  CallClock callClock(clock);

  if (oms_status_ok != updateDependencyGraphs())
    return oms_status_error;

  if (oms_status_ok != updateInputs(initializationGraph))
    return oms_status_error;

  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->initialize())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->initialize())
      return oms_status_error;

  oms_status_enu_t status;
  size_t n_event_indicators = 0;
  for (size_t i=0; i<fmus.size(); ++i)
  {
    // get states and state derivatives
    if (fmus[i]->getNumberOfContinuousStates() > 0)
    {
      status = fmus[i]->getContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getDerivatives(states_der[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getNominalsOfContinuousStates(states_nominal[i]);
      if (oms_status_ok != status) return status;

      // Check if nominals are greater 0
      bool illegalNominals = false;
      for(size_t l=0; l<nStates[i]; l++)
      {
        if (states_nominal[i][l] <= 0)
        {
          if (Flags::ZeroNominal())
          {
            if (!illegalNominals)
              logWarning(std::string(fmus[i]->getFullCref()) + ": Illegal nominal value will be replaced with 1.0 because the flag --zeroNominal is used");
            illegalNominals = true;
            states_nominal[i][l] = 1.0;
          }
          else
            return logError(std::string(fmus[i]->getFullCref()) + ": Illegal nominal value provided by the FMU.");
        }
      }
    }
    n_event_indicators += nEventIndicators[i];
    if (fmus[i]->getNumberOfEventIndicators() > 0)
    {
      status = fmus[i]->getEventindicators(event_indicators[i], 1);
      if (oms_status_ok != status) return status;
    }
  }

  if (oms_solver_sc_cvode == solverMethod)
  {
    size_t n_states = 0;
    for (size_t i=0; i < fmus.size(); ++i)
      n_states += nStates[i];

    algebraic = n_states == 0;
    if (algebraic)
      n_states = 1;

    solverData.cvode.y = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.y) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");

    if (algebraic)
      NV_Ith_S(solverData.cvode.y, 0) = 0.0;
    else
      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.y, k) = states[j][i];
    //N_VPrint_Serial(solverData.cvode.y);

    solverData.cvode.abstol = N_VNew_Serial(static_cast<long>(n_states));
    if (!solverData.cvode.abstol) logError("SUNDIALS_ERROR: N_VNew_Serial() failed - returned NULL pointer");

    if (algebraic)
      NV_Ith_S(solverData.cvode.abstol, 0) = relativeTolerance;
    else
      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.abstol, k) = relativeTolerance*states_nominal[j][i];
    //N_VPrint_Serial(solverData.cvode.abstol);

    // Call CVodeCreate to create the solver memory and specify the
    // Backward Differentiation Formula and the use of a Newton iteration
    solverData.cvode.mem = CVodeCreate(CV_BDF);
    if (!solverData.cvode.mem) logError("SUNDIALS_ERROR: CVodeCreate() failed - returned NULL pointer");

    int flag = CVodeSetUserData(solverData.cvode.mem, (void*)this);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetUserData() failed with flag = " + std::to_string(flag));

    // Call CVodeInit to initialize the integrator memory and specify the
    // user's right hand side function in y'=cvode_rhs(t,y), the inital time T0, and
    // the initial dependent variable vector y.
    flag = CVodeInit(solverData.cvode.mem, algebraic ? cvode_rhs_algebraic3 : cvode_rhs3, time, solverData.cvode.y);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeInit() failed with flag = " + std::to_string(flag));

    flag = CVodeRootInit(solverData.cvode.mem, n_event_indicators, cvode_roots3);
    if (flag != CV_SUCCESS) logError("SUNDIALS_ERROR: CVodeRootInit() failed with flag = " + std::to_string(flag));

    // Call CVodeSVtolerances to specify the scalar relative tolerance
    // and vector absolute tolerances
    flag = CVodeSVtolerances(solverData.cvode.mem, relativeTolerance, solverData.cvode.abstol);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSVtolerances() failed with flag = " + std::to_string(flag));

    // Call N_VNew_Serial and SUNDenseMatrix to generate dense vector abd natrix for lin. solver module
    solverData.cvode.liny = N_VNew_Serial(n_states);
    if (solverData.cvode.liny == NULL) logError("SUNDIALS_ERROR: N_VNew_Serial() failed");
    solverData.cvode.J = SUNDenseMatrix(n_states, n_states);
    if (solverData.cvode.J == NULL) logError("SUNDIALS_ERROR: N_VNew_Serial() failed");

    // Call SUNLinSol_Dense to creat linear solver object
    solverData.cvode.linSol = SUNLinSol_Dense(solverData.cvode.liny, solverData.cvode.J);
    if (solverData.cvode.linSol == NULL) logError("SUNDIALS_ERROR: SUNLinSol_Dense() failed");

    // Call CVodeSetLinearSolver to set the dense linear solver */
    flag = CVodeSetLinearSolver(solverData.cvode.mem, solverData.cvode.linSol, solverData.cvode.J);
    if (flag < 0) logError("SUNDIALS_ERROR: CVDense() failed with flag = " + std::to_string(flag));

    logInfo("maximum step size for '" + std::string(getFullCref()) + "': " + std::to_string(maximumStepSize));
    flag = CVodeSetMaxStep(solverData.cvode.mem, maximumStepSize);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxStep() failed with flag = " + std::to_string(flag));

    // further settings from cpp runtime
    flag = CVodeSetInitStep(solverData.cvode.mem, initialStepSize);                      // INITIAL STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetInitStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxOrd(solverData.cvode.mem, 5);                                      // MAXIMUM ORDER
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxOrd() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxConvFails(solverData.cvode.mem, Flags::CVODEMaxNLSFailures());     // MAXIMUM NUMBER OF NONLINEAR CONVERGENCE FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxConvFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetStabLimDet(solverData.cvode.mem, true);                               // STABILITY DETECTION
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetStabLimDet() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMinStep(solverData.cvode.mem, minimumStepSize);                       // MINIMUM STEPSIZE
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMinStep() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNonlinIters(solverData.cvode.mem, Flags::CVODEMaxNLSIterations()); // MAXIMUM NUMBER OF ITERATIONS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNonlinIters() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxErrTestFails(solverData.cvode.mem, Flags::CVODEMaxErrTestFails()); // MAXIMUM NUMBER OF ERROR TEST FAILURES
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxErrTestFails() failed with flag = " + std::to_string(flag));
    flag = CVodeSetMaxNumSteps(solverData.cvode.mem, Flags::CVODEMaxSteps());            // MAXIMUM NUMBER OF STEPS
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeSetMaxNumSteps() failed with flag = " + std::to_string(flag));
  }

  // Mark algebraic loops to be updated on next call
  forceLoopsToBeUpdated();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::terminate()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->terminate())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->terminate())
      return oms_status_error;

  if (oms_solver_sc_cvode == solverMethod && solverData.cvode.mem)
  {
    long int nst, nfe, nsetups, nni, ncfn, netf;
    int flag;

    flag = CVodeGetNumSteps(solverData.cvode.mem, &nst);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumSteps() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumRhsEvals(solverData.cvode.mem, &nfe);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumRhsEvals() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumLinSolvSetups(solverData.cvode.mem, &nsetups);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumLinSolvSetups() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumErrTestFails(solverData.cvode.mem, &netf);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumErrTestFails() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumNonlinSolvIters(solverData.cvode.mem, &nni);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvIters() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumNonlinSolvConvFails(solverData.cvode.mem, &ncfn);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvConvFails() failed with flag = " + std::to_string(flag));

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumRhsEvals  = " + std::to_string(nfe) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    SUNMatDestroy(solverData.cvode.J);
    N_VDestroy_Serial(solverData.cvode.liny);
    SUNLinSolFree(solverData.cvode.linSol);
    N_VDestroy_Serial(solverData.cvode.y);
    N_VDestroy_Serial(solverData.cvode.abstol);
    CVodeFree(&(solverData.cvode.mem));
    solverData.cvode.mem = NULL;
  }

  for (size_t i=0; i<fmus.size(); ++i)
  {
    free(states[i]);
    free(states_der[i]);
    free(states_nominal[i]);
    free(event_indicators[i]);
    free(event_indicators_prev[i]);
  }

  fmus.clear();
  // callEventUpdate.clear();
  // terminateSimulation.clear();
  delete[] callEventUpdate;
  delete[] terminateSimulation;
  nStates.clear();
  nEventIndicators.clear();
  states.clear();
  states_der.clear();
  states_nominal.clear();
  event_indicators.clear();
  event_indicators_prev.clear();

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::reset()
{
  for (const auto& subsystem : getSubSystems())
    if (oms_status_ok != subsystem.second->reset())
      return oms_status_error;

  for (const auto& component : getComponents())
    if (oms_status_ok != component.second->reset())
      return oms_status_error;

  time = getModel().getStartTime();

  if (oms_solver_sc_cvode == solverMethod && solverData.cvode.mem)
  {
    long int nst, nfe, nsetups, nni, ncfn, netf;
    int flag;

    flag = CVodeGetNumSteps(solverData.cvode.mem, &nst);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumSteps() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumRhsEvals(solverData.cvode.mem, &nfe);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumRhsEvals() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumLinSolvSetups(solverData.cvode.mem, &nsetups);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumLinSolvSetups() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumErrTestFails(solverData.cvode.mem, &netf);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumErrTestFails() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumNonlinSolvIters(solverData.cvode.mem, &nni);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvIters() failed with flag = " + std::to_string(flag));
    flag = CVodeGetNumNonlinSolvConvFails(solverData.cvode.mem, &ncfn);
    if (flag < 0) logError("SUNDIALS_ERROR: CVodeGetNumNonlinSolvConvFails() failed with flag = " + std::to_string(flag));

    std::string msg = "Final Statistics for '" + std::string(getFullCref()) + "':\n";
    msg += "NumSteps = " + std::to_string(nst) + " NumRhsEvals  = " + std::to_string(nfe) + " NumLinSolvSetups = " + std::to_string(nsetups) + "\n";
    msg += "NumNonlinSolvIters = " + std::to_string(nni) + " NumNonlinSolvConvFails = " + std::to_string(ncfn) + " NumErrTestFails = " + std::to_string(netf);
    logInfo(msg);

    SUNMatDestroy(solverData.cvode.J);
    N_VDestroy_Serial(solverData.cvode.liny);
    SUNLinSolFree(solverData.cvode.linSol);
    N_VDestroy_Serial(solverData.cvode.y);
    N_VDestroy_Serial(solverData.cvode.abstol);
    CVodeFree(&(solverData.cvode.mem));
    solverData.cvode.mem = nullptr;
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::doStep()
{
  switch(solverMethod)
  {
    case oms_solver_sc_explicit_euler:
      return doStepEuler();

    case oms_solver_sc_cvode:
      return doStepCVODE();

    default:
      return logError_InternalError;
  }
}

oms_status_enu_t oms::SystemSC3::doStepEuler()
{
  fmi3Status fmistatus;
  oms_status_enu_t status;

  // Step 1: Initialize state variables and time
  const fmi3Float64 end_time = std::min(time + maximumStepSize, getModel().getStopTime());
  const fmi3Float64 event_time_tolerance = 1e-4;

  logDebug("doStepEuler: " + std::to_string(time) + " -> " + std::to_string(end_time));

  // Step 2: Backup states for potential rollback
  std::vector<double *> states_backup;
  std::vector<double *> states_der_backup;

  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      states_backup.push_back((double*)calloc(nStates[i], sizeof(double)));
      states_der_backup.push_back((double*)calloc(nStates[i], sizeof(double)));
    }
    else
    {
      states_backup.push_back(nullptr);
      states_der_backup.push_back(nullptr);
    }
  }
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      status = fmus[i]->getContinuousStates(states_backup[i]);
      if (oms_status_ok != status) return status;
      status = fmus[i]->getDerivatives(states_der_backup[i]);
      if (oms_status_ok != status) return status;
    }
    fmistatus = fmi3_getEventIndicators(fmus[i]->getFMU(), event_indicators_prev[i], nEventIndicators[i]);
    if (fmi3OK != fmistatus) logError_FMUCall("fmi3_getEventIndicators", fmus[i]);
  }

  fmi3Float64 step_size_adjustment = maximumStepSize;
  fmi3Float64 event_time = end_time;
  bool event_detected = false;

  fmi3Float64 tnext = end_time + 1.0;
  bool terminated = false;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    if (fmus[i]->nextEventTimeDefined && (tnext > fmus[i]->nextEventTime) && (time < fmus[i]->nextEventTime))
      tnext = fmus[i]->nextEventTime;

    if(fmus[i]->terminateSimulation)
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      terminated = true;
    }

  }

  // Step 3: Main integration loop
  while (time < end_time && !terminated)
  {
    if(tnext < event_time)
      event_time = tnext;

    step_size_adjustment *= 0.5; // reduce the step size in each iteration

    // a. Evaluate derivatives for each FMU
    const fmi3Float64 step_size = event_time - time;  // Substep size, do one step from current time to the event
    logDebug("step_size: " + std::to_string(step_size) + " | " + std::to_string(time) + " -> " + std::to_string(event_time));

    for (size_t i = 0; i < fmus.size(); ++i)
    {
      if (0 == nStates[i])
        continue;

      for (size_t k = 0; k < nStates[i]; ++k)
        states[i][k] = states_backup[i][k] + step_size * states_der_backup[i][k];

      status = fmus[i]->setContinuousStates(states[i]);
      //status = fmi3_setContinuousStates(fmus[i]->getFMU(), states[i], nStates[i]);
      if (oms_status_ok != status) return status;
    }

    // b. Event Detection
    event_detected = event_time == tnext;
    logDebug("Event detected: " + std::to_string(event_detected));
    for (size_t i = 0; i < fmus.size() && !event_detected; ++i)
    {
      fmistatus = fmi3_getEventIndicators(fmus[i]->getFMU(), event_indicators[i], nEventIndicators[i]);
      if (fmi3OK != fmistatus) logError_FMUCall("fmi3_getEventIndicators", fmus[i]);

      for (size_t k=0; k < nEventIndicators[i]; k++)
      {
        if ((event_indicators[i][k] > 0) != (event_indicators_prev[i][k] > 0))
        {
          logDebug("event detected");
          event_detected = true;
          break;
        }
      }
    }

    // c. Event handling and step advancement
    if (!event_detected)
    {
      logDebug("no event detected");

      if (event_time == end_time)
      {
        // Integrate normally to the end time if no events are ahead
        time = event_time;
        step_size_adjustment = maximumStepSize;

        // set time
        for (const auto& component : getComponents())
          component.second->setTime(time);

        for (size_t i = 0; i < fmus.size(); ++i)
        {
          fmistatus = fmi3_completedIntegratorStep(fmus[i]->getFMU(), fmi3True, &callEventUpdate[i], &terminateSimulation[i]);
          if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_completedIntegratorStep", fmus[i]);
        }

        // emit the left limit of the event (if it hasn't already been emitted)
        updateInputs(simulationGraph); //pass the continuousTimeMode dependency graph which involves only connections of type Real
        if (isTopLevelSystem())
          getModel().emit(time, false);

        logDebug("integrate normally to the end time if no events are ahead");
      }
      else
      {
        // Advance to the tentative event time and check again
        event_time += step_size_adjustment;
        logDebug("advance to the tentative event time and check again");
      }
    }
    else
    {
      if (event_time == tnext || step_size_adjustment < event_time_tolerance)
      {
        logDebug("event found!!! " + std::to_string(event_time));

        // Event detected: Restore to last "safe" state and integrate directly to event time
        time = event_time;
        step_size_adjustment = maximumStepSize;
        event_time = end_time;

        // emit the left limit of the event (if it hasn't already been emitted)
        if (isTopLevelSystem())
          getModel().emit(time, false);

        // set time
        for (const auto& component : getComponents())
          component.second->setTime(time);

        // Enter event mode and handle discrete state updates for each FMU
        for (size_t i = 0; i < fmus.size(); ++i)
        {
          fmistatus = fmi3_completedIntegratorStep(fmus[i]->getFMU(), fmi3True, &callEventUpdate[i], &terminateSimulation[i]);
          if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_completedIntegratorStep", fmus[i]);
          fmistatus = fmi3_enterEventMode(fmus[i]->getFMU());
          if (fmi3OK != fmistatus) logError_FMUCall("fmi3_enterEventMode", fmus[i]);
          fmus[i]->doEventIteration();
          fmistatus = fmi3_enterContinuousTimeMode(fmus[i]->getFMU());
          if (fmi3OK != fmistatus) logError_FMUCall("fmi3_enterContinuousTimeMode", fmus[i]);
          if (nStates[i] > 0)
          {
            status = fmus[i]->getContinuousStates(states_backup[i]);
            if (oms_status_ok != status) return status;
            status = fmus[i]->getDerivatives(states_der_backup[i]);
            if (oms_status_ok != status) return status;
          }

          status = fmus[i]->getEventindicators(event_indicators_prev[i], nEventIndicators[i]);
          if (oms_status_ok != status) return status;
        }
        // find next time event
        tnext = end_time + 1.234;
        for (size_t i = 0; i < fmus.size(); ++i)
        {
          if (fmus[i]->nextEventTimeDefined && (tnext > fmus[i]->nextEventTime) && (time < fmus[i]->nextEventTime))
            tnext = fmus[i]->nextEventTime;

            if(fmus[i]->terminateSimulation)
          {
            logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
            getModel().setStopTime(time);
            terminated = true;
          }
        }

        // emit the right limit of the event
        updateInputs(eventGraph);
        if (isTopLevelSystem())
          getModel().emit(time, true);
      }
      else
      {
        // Ok, event must be little earlier
        event_time -= step_size_adjustment;
      }
    }
    // Print all output variables to CSV file
    double value;
    if (outputFile != NULL)
    {
      fprintf(outputFile, "%f", time);
      fmi3ValueReference vr1 = 1;
      fmi3ValueReference vr2 = 5;
      fmi3ValueReference vr3 = 6;
      for (int i = 0; i < 1; ++i)
      {
        fmi3_getFloat64(fmus[0]->getFMU(), &vr1, 1, &value, 1);
        fprintf(outputFile, ",%f", value);
        fmi3_getFloat64(fmus[0]->getFMU(), &vr2, 1, &value, 1);
        fprintf(outputFile, ",%f", value);
        fmi3_getFloat64(fmus[0]->getFMU(), &vr3, 1, &value, 1);
        // std::cout << "info: time=" << time << ", var" << i << "=" << value << std::endl;
        fprintf(outputFile, ",%f", value);
      }
      fprintf(outputFile, "\n");
    }
  }

  for (size_t i=0; i < fmus.size(); ++i)
  {
    if (nStates[i] > 0)
    {
      free(states_backup[i]);
      free(states_der_backup[i]);
    }
  }

  return oms_status_ok;
}

oms_status_enu_t oms::SystemSC3::doStepCVODE()
{
  fmi3Status fmistatus;
  oms_status_enu_t status;
  int flag;

  std::cout << "Cvode1" << std::endl;
  const fmi3Float64 end_time = std::min(time + maximumStepSize, getModel().getStopTime());

  // find next time event
  fmi3Float64 tnext = end_time+1.0;
  for (size_t i = 0; i < fmus.size(); ++i)
  {
    std::cout << "Cvode2" << std::endl;
    if (fmus[i]->nextEventTimeDefined && (tnext > fmus[i]->nextEventTime))
      tnext = fmus[i]->nextEventTime;

    if(fmus[i]->terminateSimulation)
    {
      logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
      getModel().setStopTime(time);
      time = end_time;
    }
  }
  std::cout << "Cvode3 : " << std::to_string(tnext) << std::endl;
  logDebug("tnext: " + std::to_string(tnext));

  while (time < end_time)
  {
    logDebug("CVode: " + std::to_string(time) + " -> " + std::to_string(end_time));
    for (size_t j=0, k=0; j < fmus.size(); ++j)
      for (size_t i=0; i < nStates[j]; ++i, ++k)
        NV_Ith_S(solverData.cvode.y, k) = states[j][i];

    flag = CVode(solverData.cvode.mem, std::min(tnext, end_time), solverData.cvode.y, &time, CV_NORMAL);

    for (size_t i = 0, j=0; i < fmus.size(); ++i)
    {
      for (size_t k = 0; k < nStates[i]; k++, j++)
        states[i][k] = NV_Ith_S(solverData.cvode.y, j);

      // set states
      status = fmus[i]->setContinuousStates(states[i]);
      if (oms_status_ok != status) return status;
    }

    if (flag == CV_ROOT_RETURN || time == tnext)
    {
      logDebug("event found!!! " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi3_completedIntegratorStep(fmus[i]->getFMU(), fmi3True, &callEventUpdate[i], &terminateSimulation[i]);
        if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_completedIntegratorStep", fmus[i]);
      }

      // emit the left limit of the event (if it hasn't already been emitted)
      if (isTopLevelSystem())
        getModel().emit(time, false);

      // Enter event mode and handle discrete state updates for each FMU
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi3_enterEventMode(fmus[i]->getFMU());
        if (fmi3OK != fmistatus) logError_FMUCall("fmi3_enterEventMode", fmus[i]);

        fmus[i]->doEventIteration();
      }

      updateInputs(eventGraph);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi3_enterContinuousTimeMode(fmus[i]->getFMU());
        if (fmi3OK != fmistatus) logError_FMUCall("fmi3_enterContinuousTimeMode", fmus[i]);
      }

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      // find next time event
      tnext = end_time+1.0;
      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (fmus[i]->getEventInfo()->nextEventTimeDefined && (tnext > fmus[i]->getEventInfo()->nextEventTime))
          tnext = fmus[i]->getEventInfo()->nextEventTime;

        if(fmus[i]->getEventInfo()->terminateSimulation)
        {
          logInfo("Simulation terminated by FMU " + std::string(fmus[i]->getFullCref()) + " at time " + std::to_string(time));
          getModel().setStopTime(time);
          time = end_time;
        }
      }
      logDebug("tnext: " + std::to_string(tnext));

      // emit the right limit of the event
      updateInputs(eventGraph);
      if (isTopLevelSystem())
        getModel().emit(time, true);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      for (size_t j=0, k=0; j < fmus.size(); ++j)
        for (size_t i=0; i < nStates[j]; ++i, ++k)
          NV_Ith_S(solverData.cvode.y, k) = states[j][i];

      flag = CVodeReInit(solverData.cvode.mem, time, solverData.cvode.y);
      if (flag < 0) return logError("SUNDIALS_ERROR: CVodeReInit() failed with flag = " + std::to_string(flag));

      continue;
    }

    if (flag == CV_SUCCESS)
    {
      logDebug("CVode completed successfully at t = " + std::to_string(time));

      // set time
      for (const auto& component : getComponents())
        component.second->setTime(time);

      for (size_t i = 0; i < fmus.size(); ++i)
      {
        fmistatus = fmi3_completedIntegratorStep(fmus[i]->getFMU(), fmi3True, &callEventUpdate[i], &terminateSimulation[i]);
        if (fmi3OK != fmistatus) return logError_FMUCall("fmi3_completedIntegratorStep", fmus[i]);

        if (0 == nStates[i])
          continue;

        status = fmus[i]->getContinuousStates(states[i]);
        if (oms_status_ok != status) return status;
      }

      updateInputs(simulationGraph);
      if (isTopLevelSystem())
        getModel().emit(time, false);
    }
    else
      return logError("CVode failed with flag = " + std::to_string(flag));
  }

  return oms_status_ok;

}

oms_status_enu_t oms::SystemSC3::stepUntil(double stopTime)
{
  CallClock callClock(clock);
  const double startTime=time;

  if (Flags::ProgressBar())
    logInfo("step SC system [" + std::to_string(startTime) + "; " + std::to_string(stopTime) + "] with step size [" + std::to_string(initialStepSize) + "; " + std::to_string(minimumStepSize) + "; " + std::to_string(maximumStepSize) + "]");

  //outputFile = NULL;
  // Prepare output file
  const char *outputCsvPath = "omsimulatortest.csv";
  if (outputCsvPath != NULL)
  {
    outputFile = fopen(outputCsvPath, "w");
    if (outputFile != NULL)
    {
      fprintf(outputFile, "time");
      for (int i = 0; i < 1; ++i)
      {
        fprintf(outputFile, ",%s", fmi3_getVariableName(fmi3_getVariableByValueReference(fmus[0]->getFMU(), 1)));
        fprintf(outputFile, ",%s", fmi3_getVariableName(fmi3_getVariableByValueReference(fmus[0]->getFMU(), 5)));
        fprintf(outputFile, ",%s", fmi3_getVariableName(fmi3_getVariableByValueReference(fmus[0]->getFMU(), 6)));
      }
      fprintf(outputFile, "\n");
    }
  }
  // main simulation loop
  oms_status_enu_t status = oms_status_ok;
  while (time < std::min(stopTime, getModel().getStopTime()) && oms_status_ok == status)
  {
    status = doStep();
    if (status != oms_status_ok)
      logWarning("Bad return code at time " + std::to_string(time));

    if (isTopLevelSystem() && Flags::ProgressBar())
      Log::ProgressBar(startTime, stopTime, time);
  }

  if (outputFile != NULL)
    fclose(outputFile);

  if (isTopLevelSystem() && Flags::ProgressBar())
    Log::TerminateBar();

  return status;
}

oms_status_enu_t oms::SystemSC3::updateInputs(DirectedGraph& graph)
{
  CallClock callClock(clock);
  oms_status_enu_t status;
  int loopNum = 0;

  // input := output
  const std::vector< scc_t >& sortedConnections = graph.getSortedConnections();
  updateAlgebraicLoops(sortedConnections, graph);

  for(size_t i=0; i<sortedConnections.size(); i++)
  {
    if (!sortedConnections[i].thisIsALoop)
    {
      int output = sortedConnections[i].connections[0].first;
      int input = sortedConnections[i].connections[0].second;

      if (graph.getNodes()[input].getType() == oms_signal_type_real)
      {
        double value = 0.0;
        if (oms_status_ok != getReal(graph.getNodes()[output].getName(), value)) return oms_status_error;

        // Check for unit conversion and suppressUnitConversion. Set the value multiplied by factor.
        // By default, factor = 1.0. For example, mm to m will be (factor * value) => (1e-3 * value).
        if (sortedConnections[i].suppressUnitConversion)
          value = value;
        else
          value = sortedConnections[i].factor*value;

        if (oms_status_ok != setReal(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else if (graph.getNodes()[input].getType() == oms_signal_type_integer || graph.getNodes()[input].getType() == oms_signal_type_enum)
      {
        int value = 0.0;
        if (oms_status_ok != getInteger(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setInteger(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else if (graph.getNodes()[input].getType() == oms_signal_type_boolean)
      {
        bool value = 0.0;
        if (oms_status_ok != getBoolean(graph.getNodes()[output].getName(), value)) return oms_status_error;
        if (oms_status_ok != setBoolean(graph.getNodes()[input].getName(), value)) return oms_status_error;
      }
      else
        return logError_InternalError;
    }
    else
    {
      status = solveAlgLoop(graph, loopNum);
      if (oms_status_ok != status)
      {
        forceLoopsToBeUpdated();
        return status;
      }
      loopNum++;
    }
  }
  return oms_status_ok;
}
