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

#include "OMSysIdent.h"
#include "FitModel.h"
#include "OMSimulatorLib/oms2/Logging.h"

void* omsi_newSysIdentModel(const char* ident)
{
  logTrace();
  FitModel* pFitModel = new FitModel(ident);
  return (void*) pFitModel;
}

void omsi_freeSysIdentModel(void* simodel)
{
  logTrace();
  if (!simodel) {
    logError("omsi_freeSysIdentModel: invalid pointer");
    return;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  delete pFitModel;
}

oms_status_enu_t omsi_initialize(void* simodel, size_t nSeries,
  const double* time, size_t nTime,
  char const* const* inputvars, size_t nInputvars,
  char const* const* measurementvars, size_t nMeasurementvars)
{
  logTrace();
  if (!simodel) {
    logError("omsi_initialize: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  return pFitModel->initialize(nSeries, time, nTime, inputvars, nInputvars, measurementvars, nMeasurementvars);
}

oms_status_enu_t omsi_describe(void* simodel)
{
  logTrace();
  if (!simodel) {
    logError("omsi_describe: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  std::cout << pFitModel->toString() << std::endl;
  return oms_status_ok;
}

oms_status_enu_t omsi_addParameter(void* simodel, const char* var, double startvalue)
{
  logTrace();
  if (!simodel || !var) {
    logError("omsi_addParameter: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  pFitModel->addParameter(var, startvalue);
  return oms_status_ok;
}

oms_status_enu_t omsi_getParameter(void* simodel, const char* var, double* startvalue, double* estimatedvalue)
{
  logTrace();
  if (!simodel || !var || !startvalue || !estimatedvalue) {
    logError("omsi_addParameter: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  ParameterAttributes attr;
  oms_status_enu_t ret = pFitModel->getParameter(var, attr);
  *startvalue = attr.startValue;
  *estimatedvalue = attr.estValue;
  return ret;
}

oms_status_enu_t omsi_addMeasurement(void* simodel, size_t iSeries, const char* var, const double* values, size_t nValues)
{
  logTrace();
  if (!simodel || !var || !values) {
    logError("omsi_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  pFitModel->addMeasurement(iSeries, var, values, nValues);
  return oms_status_ok;
}

oms_status_enu_t omsi_addInput(void* simodel, const char* var, const double* values, size_t nValues)
{
  logTrace();
  if (!simodel || !var || !values) {
    logError("omsi_addInput: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  pFitModel->addInput(var, values, nValues);
  return oms_status_ok;
}

oms_status_enu_t omsi_setOptions_max_num_iterations(void* simodel, size_t max_num_iterations)
{
  logTrace();
  if (!simodel) {
    logError("omsi_addMeasurement: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  pFitModel->setOptions_max_num_iterations(max_num_iterations);
  return oms_status_ok;
}

oms_status_enu_t omsi_solve(void* simodel, const char* reporttype)
{
  logTrace();
  if (!simodel) {
    logError("omsi_solve: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  return pFitModel->solve(reporttype);
}

oms_status_enu_t omsi_getState(void* simodel, omsi_simodelstate_t* state)
{
  logTrace();
  if (!simodel) {
    logError("omsi_getState: invalid pointer");
    return oms_status_error;
  }
  FitModel* pFitModel = (FitModel*) simodel;
  FitModelState modelstate = pFitModel->getState();
  switch (modelstate) {
    case FitModelState::CONSTRUCTED:    *state = omsi_simodelstate_constructed; break;
    case FitModelState::INITIALIZED:    *state = omsi_simodelstate_initialized; break;
    case FitModelState::CONVERGENCE:    *state = omsi_simodelstate_convergence; break;
    case FitModelState::NO_CONVERGENCE: *state = omsi_simodelstate_no_convergence; break;
    case FitModelState::FAILURE:        *state = omsi_simodelstate_failure; break;
  }

  return oms_status_ok;
}
