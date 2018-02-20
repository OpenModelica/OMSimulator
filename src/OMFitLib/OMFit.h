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

#ifndef _OMFIT_H_
#define _OMFIT_H_

#include <stddef.h>
#include "Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief State of fitting model.
 */
typedef enum {
  omsfit_fitmodelstate_constructed,    //!< After omsfit_newFitModel
  omsfit_fitmodelstate_initialized,    //!< After omsfit_initialize
  omsfit_fitmodelstate_convergence,    //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::CONVERGENCE
  omsfit_fitmodelstate_no_convergence, //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::NO_CONVERGENCE
  omsfit_fitmodelstate_failure         //!< After omsfit_solve if Ceres minimizer returned with ceres::TerminationType::FAILURE
} omsfit_fitmodelstate_t;

/**
 * \brief Creates an empty model for parameter estimation / curve fitting.
 *
 * @param model [inout] OMS (composite) model as opaque pointer.
 * @return Fitting model instance as opaque pointer.
 */
void* omsfit_newFitModel(void* model);

/**
 * \brief Unloads a model.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 */
void omsfit_freeFitModel(void* fitmodel);

/**
 * \brief Initializes fitting model.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @nSeries [in] Number of measurement series.
 * @param time [in] Array of measurement time instants.
 * @param nTime [in] Length of time array.
 * @inputvars [in] Names of input variables (NULL if none).
 * @nInputvars [in] Number of external inputs stimulating the system.
 * @measurmentvars [in] names of observed measurement variables.
 * @nMeasurmentvars  [in] Number of observed measurement variables.
 * @return Error status.
 */
oms_status_t omsfit_initialize(void* fitmodel, size_t nSeries,
  const double* time, size_t nTime,
  char const* const* inputvars, size_t nInputvars,
  char const* const* measurementvars, size_t nMeasurementvars);

/**
 * \brief Print summary of fitting model.
 *
 * @param fitmodel [in] Fitting model as opaque pointer.
 */
oms_status_t omsfit_describe(void* fitmodel);

/**
 * \brief Add measurement values for a fittting variable.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param iSeries [in] Index of measurement series.
 * @param var [in] Name of variable.
 * @param values [in] Array of measured values for respective time instants.
 * @param nValues [in] Length of values array.
 * @return Error status.
 */
oms_status_t omsfit_addMeasurement(void* fitmodel, size_t iSeries, const char* var, const double* values, size_t nValues);
// Alternatively??
// void omsfit_addMeasurementSeries(void *fitmodel, int iseries,
//   double const* const* inputs, size_t dim1Inputs, size_t dim2Inputs,
//   double const* const* measurements, size_t dim1Measurements, size_t dim2Measurements);

/**
 * \brief Add parameter that should be estimated.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param var [in] Name of parameter.
 * @param startvalue [in] Start value of parameter.
 * @return Error status.
 */
oms_status_t omsfit_addParameter(void* fitmodel, const char* var, double startvalue);

/**
 * \brief Get parameter that should be estimated.
 *
 * \warning Unless omsfit_solve returned succesfully, estimatedvalue will be garbage
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param var [in] Name of parameter.
 * @param startvalue [out] start value of parameter.
 * @param estimatedvalue [out] Estimated value of parameter.
 * @return Error status.
 */
oms_status_t omsfit_getParameter(void* fitmodel, const char* var, double* startvalue, double* estimatedvalue);

/**
 * \brief Solve parameter estimation problem.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param reporttype [in] Print report and progress information after call to Ceres solver.
 *                        Supported report types (NULL or "" denotes no output): {NULL, "", "BriefReport", "FullReport"}.
 * @return Error status.
 */
oms_status_t omsfit_solve(void* fitmodel, const char* reporttype);

/**
 * \brief Set Ceres solver option 'Solver::Options::max_num_iterations'.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param max_num_iterations [in] Maximum number of iterations for which the solver should run (default: 25).
 * @return Error status.
 */
oms_status_t omsfit_setOptions_max_num_iterations(void* fitmodel, size_t max_num_iterations);

/**
 * \brief Get state of fitting model object.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param state [out] State of fitting model.
 * @return Error status.
 */
oms_status_t omsfit_getState(void* fitmodel, omsfit_fitmodelstate_t* state);

/**
 * \brief TODO Add input values for a model input.
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param var [in] Name of input variable.
 * @param time [in] Array of input time instants.
 * @param values [in] Array of input values for respective time instants.
 * @param nvalues [in] Length of values array.
 * @return Error status.
 */
// oms_status_t omsfit_addInput(void* fitmodel, const char* var, const double* time, const double* values, int nvalues);

/**
 * \brief TODO? Provide convenience function to "Load reference measurement data from file"?.
 *
 * The format of the csv file should be similar to
 * "time","varname1","varname2", ... "varnameX"
 * 0,0.1,0.2, ..., 0
 * 0.1,0.11,0.21, ..., 0
 * 0.2,0.111,0.211, ..., 0
 *
 * @param fitmodel [inout] Fitting model as opaque pointer.
 * @param filename [in]  Path to file with data in csv format.
 * @param varname [in] Variable name.
 * @return Error status.
 */
// oms_status_t omsfit_loadMeasurement(void* model, const void* filename, const char* varname);


#ifdef __cplusplus
}
#endif

#endif /* _OMFIT_H_ */
