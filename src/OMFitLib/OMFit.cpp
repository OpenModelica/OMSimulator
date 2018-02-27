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

#include "OMFit.h"
#include "FitModel.h"
#include "OMSimulatorLib/oms2/Logging.h"

void* omsfit_newFitModel(void* model)
{
  logTrace();
  FitModel* pFitModel = new FitModel(model);
  return (void*) pFitModel;
}

void omsfit_freeFitModel(void* fitmodel)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_freeFitModel: invalid pointer");
    return;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  delete pFitModel;
}

oms_status_enu_t omsfit_initialize(void* fitmodel, size_t nSeries,
  const double* time, size_t nTime,
  char const* const* inputvars, size_t nInputvars,
  char const* const* measurementvars, size_t nMeasurementvars)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_freeFitModel: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  return pFitModel->initialize(nSeries, time, nTime, inputvars, nInputvars, measurementvars, nMeasurementvars);
}

oms_status_enu_t omsfit_describe(void* fitmodel)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_toString: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  std::cout << pFitModel->toString() << std::endl;
  return oms_status_ok;
}

oms_status_enu_t omsfit_addParameter(void* fitmodel, const char* var, double startvalue)
{
  logTrace();
  if (!fitmodel || !var) {
    logError("omsfit_addParameter: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  pFitModel->addParameter(var, startvalue);
  return oms_status_ok;
}

oms_status_enu_t omsfit_getParameter(void* fitmodel, const char* var, double* startvalue, double* estimatedvalue)
{
  logTrace();
  if (!fitmodel || !var || !startvalue || !estimatedvalue) {
    logError("omsfit_addParameter: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  ParameterAttributes attr;
  oms_status_enu_t ret = pFitModel->getParameter(var, attr);
  *startvalue = attr.startValue;
  *estimatedvalue = attr.estValue;
  return ret;
}

oms_status_enu_t omsfit_addMeasurement(void* fitmodel, size_t iSeries, const char* var, const double* values, size_t nValues)
{
  logTrace();
  if (!fitmodel || !var || !values) {
    logError("omsfit_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  pFitModel->addMeasurement(iSeries, var, values, nValues);
  return oms_status_ok;
}

oms_status_enu_t omsfit_setOptions_max_num_iterations(void* fitmodel, size_t max_num_iterations)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  pFitModel->setOptions_max_num_iterations(max_num_iterations);
  return oms_status_ok;
}

oms_status_enu_t omsfit_solve(void* fitmodel, const char* reporttype)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  return pFitModel->solve(reporttype);
}

oms_status_enu_t omsfit_getState(void* fitmodel, omsfit_fitmodelstate_t* state)
{
  logTrace();
  if (!fitmodel) {
    logError("omsfit_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) fitmodel;
  FitModelState modelstate = pFitModel->getState();
  switch (modelstate) {
    case FitModelState::CONSTRUCTED:    *state = omsfit_fitmodelstate_constructed; break;
    case FitModelState::INITIALIZED:    *state = omsfit_fitmodelstate_initialized; break;
    case FitModelState::CONVERGENCE:    *state = omsfit_fitmodelstate_convergence; break;
    case FitModelState::NO_CONVERGENCE: *state = omsfit_fitmodelstate_no_convergence; break;
    case FitModelState::FAILURE:        *state = omsfit_fitmodelstate_failure; break;
  }

  return oms_status_ok;
}
