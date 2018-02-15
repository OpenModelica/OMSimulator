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
#include <string>
#include <chrono>

#include "FitModel.h"
#include "OMSimulatorLib/OMSimulator.h"
#include "OMSimulatorLib/Logging.h"

#include "ceres/ceres.h"
#include "glog/logging.h"
using ceres::DynamicNumericDiffCostFunction;
// using ceres::NumericDiffCostFunction;
using ceres::CENTRAL;
using ceres::CostFunction;
using namespace std::chrono;
using Eigen::Dynamic;
using Eigen::RowMajor;
typedef Eigen::Matrix<double, Dynamic, 1> Vector;
typedef Eigen::Matrix<double, Dynamic, Dynamic, RowMajor> Matrix;

std::string seriesPropertiesString(std::string var, std::vector<SeriesMap> series) {
  logTrace();
  std::stringstream ss;
  ss << "indexSizeMap=[";
  int size = series.size();
  for (int i=0; i < size; ++i) {
    if (i != 0) {
      ss << ", ";
    }
    ss << i << "->";
    SeriesMap s = series[i];
    auto it = s.find(var);
    if (it == s.end()) {
      ss << "empty";
    } else {
      ss << it->second.size();
    }
  }
  ss << "]";
  return ss.str();
}

struct OdeResidual {
  OdeResidual(double t, std::vector<VarValueMap> mes,
    std::vector<VarValueMap> input, std::set<std::string> params, void* model)
      : t_(t), mes_(mes), input_(input), params_(params), model_(model) {}

  bool operator()(double const* const* parameters, double* residual) const {
    {
      int i = 0;
      // precondition: order in 'parameters' corresponds to params_ set order
      for (auto varname: params_) {
        oms_setReal(model_, varname.c_str(), parameters[i][0]);
        i++;
      }
    }
    oms_setStopTime(model_, t_); // needed?

    oms_initialize(model_);
    // TODO Replace below by a loop that sets inputs if inputs are available
    oms_stepUntil(model_, t_);

    int nMesVars = mes_.at(0).size();
    // get simulation values of observed variables
    std::vector<double> x(nMesVars);
    {
      int i = 0;
      for (auto& varValue: mes_.at(0)) {
        x[i] = oms_getReal(model_, varValue.first.c_str());
        i++;
      }
    }
    // Compute residual by subtracting simulation value from measured value
    for (int i=0; i < mes_.size(); ++i) {
      int j = 0;
      for (auto& varValue : mes_.at(i)) {
        residual[i*nMesVars + j] = varValue.second - x.at(j);
        j++;
      }
    }

    oms_reset(model_);
    return true;
  }

 private:
  const double t_;
  const std::vector<VarValueMap> mes_;
  const std::vector<VarValueMap> input_;
  const std::set<std::string> params_;
  void* model_;
};

oms_status_t FitModel::solve(const char* reporttype)
{
  logTrace();
  if (!(this->isDataComplete())) {
    logError("FitModel::solve: Incomplete data, please check measurements etc.");
    return oms_status_error;
  }
  std::string report = reporttype ? std::string(reporttype) : std::string("");
  if ( !(report == "" || report == "FullReport" || report == "BriefReport") ) {
    logError(std::string("FitModel::solve: Invalid argument reporttype=")+report);
    return oms_status_error;
  }
  if (report == "") {
    options_.logging_type = ceres::SILENT;
  } else {
    options_.logging_type = ceres::PER_MINIMIZER_ITERATION;
  }

  std::set<std::string> params;
  {
    int i = 0;
    parameter_blocks_.clear();
    for (auto& p: parameters_) {
      params.insert(p.first);
      // Each parameter gets its own parameter block
      // (alternatively, but maybe less idiomatic for Ceres, all parameters could be collected in one block).
      parameter_blocks_.push_back(&(p.second.estValue));
      i++;
    }
  }

  int size = mdata_.time.size();
  for (int i=0; i < size; ++i) {
    std::vector<VarValueMap> mes(mdata_.nSeries);
    std::vector<VarValueMap> input(0); // Dummy. TODO support inputs
    for (int j=0; j < mdata_.nSeries; ++j) {
      for (auto& varValues : mdata_.measurementSeries[j]) {
        mes[j][varValues.first] = varValues.second[i];
      }
    }
    // use numeric differentiation to obtain the derivative (jacobian).
    DynamicNumericDiffCostFunction<OdeResidual>* cost_function =
      new DynamicNumericDiffCostFunction<OdeResidual>(
        new OdeResidual(mdata_.time[i], mes, input, params, model_));
    for (int i=0; i < parameters_.size(); ++i) {
      cost_function->AddParameterBlock(1);
    }
    cost_function->SetNumResiduals(mdata_.measurementVars.size());

    problem_.AddResidualBlock(
        cost_function,
        NULL,
        parameter_blocks_);
  }

  ceres::Solver::Summary summary;
  // Run the solver and measure the needed time
  auto t0 = high_resolution_clock::now();
  Solve(options_, &problem_, &summary);
  auto t1 = high_resolution_clock::now();

  // Report results
  if (report == "BriefReport")
    std::cout << summary.BriefReport() << "\n";
  if (report == "FullReport")
    std::cout << summary.FullReport() << "\n";
  if (report != "") {
    std::cout << "\n=====================================\n" <<
     "Total duration for parameter estimation: " << duration_cast<milliseconds>(t1-t0).count() << "msec.\n" <<
     "Result of parameter estimation (check 'Termination' status above whether solver converged):\n\n";
    for (auto& p: parameters_) {
      std::cout << p.first << "(start=" << p.second.startValue << ", *estimate*=" <<
        p.second.estValue << ")\n";
    }
    std::cout << "\n=====================================\n";
  }

  switch (summary.termination_type) {
    case ceres::TerminationType::CONVERGENCE: state_=FitModelState::CONVERGENCE; break;
    case ceres::TerminationType::NO_CONVERGENCE: state_=FitModelState::NO_CONVERGENCE; break;
    case ceres::TerminationType::FAILURE: {
      state_=FitModelState::FAILURE;
      logError("FitModel::solve: Ceres solver returned with TerminationType::FAILURE");
      return oms_status_error;
    }
    default: {
      logError(std::string("FitModel::solve: Ceres solver returned with unhandled summary.termination_type=") +
        std::to_string(summary.termination_type));
      return oms_status_error;
    }
  }
  return oms_status_ok;
}

FitModel::FitModel(void* model) : model_(model)
{
  logTrace();
  if (!model) {
    logError("FitModel::FitModel: Invalid pointer");
	exit(1);
  }
  options_.max_num_iterations = 25;
  options_.linear_solver_type = ceres::DENSE_QR;
  options_.minimizer_progress_to_stdout = true;
  state_ = FitModelState::CONSTRUCTED;
}

oms_status_t FitModel::initialize(size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars)
{
  logTrace();
  oms_status_t status = oms_status_ok;
  if (state_ != FitModelState::CONSTRUCTED) {
    logWarning("FitModel::initialize: Reinitialization of initialized object, clearing existing data.");
    status = oms_status_warning;
  }

  mdata_.time.resize(nTime);
  std::copy(time, time+nTime, mdata_.time.begin());

  mdata_.inputVars.clear();
  for (int i=0; i < nInputvars; ++i) {
    auto ret = mdata_.inputVars.insert(std::string(inputvars[i]));
    if (!ret.second) {
      logError("FitModel::initialize: Duplicate element '"+std::string(inputvars[i])+"' in inputvars.");
      return oms_status_error;
    }
  }

  mdata_.measurementVars.clear();
  for (int i=0; i < nMeasurementvars; ++i) {
    auto ret = mdata_.measurementVars.insert(std::string(measurementvars[i]));
    if (!ret.second) {
      logError("FitModel::initialize: Duplicate element '"+std::string(inputvars[i])+"' in measurementvars.");
      return oms_status_error;
    }
  }

  mdata_.nSeries = nSeries;
  mdata_.inputSeries.clear();
  mdata_.inputSeries.resize(nSeries);
  mdata_.measurementSeries.clear();
  mdata_.measurementSeries.resize(nSeries);
  parameters_.clear();
  state_ = FitModelState::INITIALIZED;
  return status;
}

oms_status_t FitModel::addParameter(const char* var, double startvalue)
{
  logTrace();
  oms_status_t status = oms_status_ok;
  if (state_ < FitModelState::INITIALIZED) {
    logError("FitModel::addParameter:  Calling method on uninitialized object.");
    return oms_status_error;
  }
  oms_setReal(model_, var, startvalue); // FIXME superfluous?
  if (parameters_.find(var) != parameters_.end()) {
    std::string message = std::string("FitModel::addParameter: ") + var
      +  "already exists in parameter set. Overwriting entry.\n";
    logWarning(message);
    status = oms_status_warning;
  }
  parameters_[var] = ParameterAttributes {startvalue, startvalue};

  return status;
}

oms_status_t FitModel::addMeasurement(size_t iSeries, const char* var, const double* values, size_t nValues)
{
  logTrace();
  oms_status_t status = oms_status_ok;
  if (state_ < FitModelState::INITIALIZED) {
    logError("FitModel::addMeasurement:  Calling method on uninitialized object.");
    return oms_status_error;
  }
  if (iSeries >= mdata_.nSeries) {
    logError(std::string("FitModel::addMeasurement: index iSeries="+std::to_string(iSeries)+" out of range."));
    return oms_status_error;
  }

  auto it = mdata_.measurementSeries[iSeries].find(var);
  if (it != mdata_.measurementSeries[iSeries].end()) {
    logWarning(std::string("FitModel::addMeasurement: Measurement series "
      +std::to_string(iSeries) + " for variable " + var + " already exists. Overwriting!"));
    it->second.resize(nValues);
    std::copy(values, values+nValues, it->second.begin());
    status = oms_status_warning;
  } else {
    mdata_.measurementSeries[iSeries][var] = std::vector<double>(values, values+nValues);
  }

  return status;
}

FitModelState FitModel::getState()
{
  logTrace();
  return state_;
}

oms_status_t FitModel::getParameter(const char* var, ParameterAttributes& attributes)
{
  logTrace();
  auto it = parameters_.find(var);
  if (it == parameters_.end()) {
    logError(std::string("FitModel::getParameter: Cannot find parameter '") + var);
    return oms_status_error;
  }
  attributes = it->second;
  return oms_status_ok;
}

void FitModel::setOptions_max_num_iterations(size_t max_num_iterations)
{
  logTrace();
  options_.max_num_iterations = max_num_iterations;
}

bool FitModel::isDataComplete() const
{
  logTrace();
  // Check if input and measurement series entries are available
  size_t nSeries_spec = mdata_.nSeries;
  if (mdata_.measurementSeries.size() != nSeries_spec) return false;
  bool nSeries_input_valid = mdata_.inputVars.size() > 0 ?
    mdata_.inputSeries.size() == nSeries_spec : true;
  if (!nSeries_input_valid) return false;

  size_t seriesSize_spec = mdata_.time.size();

  // check if all measurement vectors are complete
  for (auto it=mdata_.measurementSeries.begin(); it != mdata_.measurementSeries.end(); ++it) {
    for (auto name: mdata_.measurementVars) {
        auto it2 = it->find(name);
        if (it2 == it->end()) return false;
        if (it2->second.size() != seriesSize_spec) return false;
    }
  }

  // Check if all input vectors are complete
  for (auto it=mdata_.inputSeries.begin(); it != mdata_.inputSeries.end(); ++it) {
    for (auto name: mdata_.inputVars) {
        auto it2 = it->find(name);
        if (it2 == it->end()) return false;
        if (it2->second.size() != seriesSize_spec) return false;
    }
  }

  return true;
}

std::string FitModel::toString() const
{
  logTrace();
  std::stringstream ss;
  ss << "FitModel(\nstate = ";
  switch (state_) {
    case FitModelState::CONSTRUCTED: ss << "constructed"; break;
    case FitModelState::INITIALIZED: ss << "initialized"; break;
    case FitModelState::CONVERGENCE: ss << "convergence"; break;
    case FitModelState::NO_CONVERGENCE: ss << "no_convergence"; break;
    case FitModelState::FAILURE: ss << "failure"; break;
  }
  if (state_ == FitModelState::CONSTRUCTED) {
    ss << "\n)";
    return ss.str();
  }
  ss << "\n, parameters = [";
  for (auto i=parameters_.begin(); i != parameters_.end(); ++i) {
    if (i != parameters_.begin()) {
      ss << ", ";
    }
    ss << i->first << "(startValue=" << i->second.startValue
       << ", estValue=" << i->second.estValue << ")";
  }
  ss << "],\nnSeries = " << mdata_.nSeries
     << ",\ntime(size=" << mdata_.time.size() << ") = ["
        << mdata_.time[0] << ".." << mdata_.time[mdata_.time.size() - 1]
     << "],\ninputVars = [";
  for (auto i=mdata_.inputVars.begin(); i != mdata_.inputVars.end(); ++i) {
     if (i != mdata_.inputVars.begin()) {
       ss << ", ";
     }
     ss << *i << "(" << seriesPropertiesString(*i, mdata_.inputSeries) << ")";
  }
  ss << "],\nmeasurementVars = [";
  for (auto i=mdata_.measurementVars.begin(); i != mdata_.measurementVars.end(); ++i) {
    if (i != mdata_.measurementVars.begin()) {
      ss << ", ";
    }
    ss << *i << "(" << seriesPropertiesString(*i, mdata_.measurementSeries) << ")";
  }
  ss << "],\ndataComplete = " << (this->isDataComplete() ? "TRUE" : "FALSE") << "\n)";
  return ss.str();
}
