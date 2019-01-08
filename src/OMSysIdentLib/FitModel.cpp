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
#include <OMSimulator.h>
#include <Logging.h>

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
  OdeResidual(std::vector<double> t, const std::map<std::string, InstantSamples> mes,
    const std::map<std::string, std::vector<double>> input, std::set<std::string> params, const char* oms_modelIdent)
      : t_(t), mes_(mes), input_(input), params_(params), oms_modelIdent_(oms_modelIdent) {}

  bool operator()(double const* const* parameters, double* residual) const {
    {
      int i = 0;
      // precondition: order in 'parameters' corresponds to params_ set order
      for (auto varname: params_) {
        oms2_setReal(varname.c_str(), parameters[i][0]);
        // std::cout << "OdeResidual: " << varname << " = " << parameters[i][0] << std::endl;
        i++;
      }
    }

    // oms2_setStopTime(oms_modelIdent_, t_.back()); // needed?

    oms2_initialize(oms_modelIdent_);

    if (!input_.empty()) {
      // Set inputs if inputs are available
      const int num = t_.size();
      for (int i=0; i < num; ++i) {
        for (auto& varData : input_) {
          std::string varname = varData.first;
          std::vector<double> data = varData.second;
          oms2_setReal(varname.c_str(), data[i]);
          // std::cout << "OdeResidual oms2_setReal i="<<i<<", varname="<<varname<<", data[i]="<<data[i]<<", t_[i]="<<t_[i]<<std::endl;
        }
        oms2_stepUntil(oms_modelIdent_, t_[i]);
      }
    } else {
      oms2_stepUntil(oms_modelIdent_, t_.back());
    }

    int nMesVars = mes_.size();
    // std::cout << "nMesVars: " << nMesVars << std::endl;
    // get simulation values of observed variables
    std::vector<double> x(nMesVars);
    {
      int i = 0;
      //for (auto& varValue: mes_.at(0)) {
      for (auto& varValues: mes_) {
        oms2_getReal(varValues.first.c_str(), &(x[i]));
        i++;
      }
    }

    // Compute residual by subtracting simulation value from measured value
    {
      int i = 0;
      for (auto& varValues: mes_) {
        // std::cout << "Residual time=" << t_.back() << ", i=" << i << ", varname: " << varValues.first << std::endl;
        for (int j=0; j < varValues.second.size(); ++j) {
          // std::cout << "Residual time=" << t_.back() << ", j=" << j << ", varValues.second[j]=" << varValues.second[j] << ", x.at(i)=" << x.at(i) << std::endl;
          // std::cout << "Residual residual[i + (j*nMesVars)] = residual[" << i + (j*nMesVars) << "] = " << varValues.second[j] - x.at(i) << std::endl;
          residual[i + j*nMesVars] = varValues.second[j] - x.at(i);
        }
        i++;
      }
    }

    oms2_reset(oms_modelIdent_);
    return true;
  }

 private:
  const std::vector<double> t_;
  // (varname -> mes[iSeries],  iSeries \in [0..nSeries])
  const std::map<std::string, InstantSamples> mes_;
  // (varname -> input[i],  i \in [0..t_size-1])
  const std::map<std::string, std::vector<double>> input_;
  const std::set<std::string> params_;
  const char* oms_modelIdent_;
};



std::map<std::string, TimeSeries> to_varname_TimeSeries_Map(std::vector<SeriesMap> series, size_t nTimeSamples) {
  std::map<std::string, TimeSeries> res;
  size_t nSeries = series.size();
  if (nSeries > 0) {
    // initialize sizes of res
    for (auto it = series[0].begin(); it != series[0].end(); ++it) {
      std::string varname = it->first;
      res[varname].resize(nTimeSamples);
      for (int i=0; i < nTimeSamples; ++i) {
        res[varname][i].resize(nSeries);
      }
    }
    // assgin values
    for (int i=0; i < nSeries; ++i) {
      for (auto it = series[i].begin(); it != series[i].end(); ++it) {
        std::string varname = it->first;
        std::vector<double> series = it->second;
        for (int j=0; j < series.size(); ++j) {
          res[varname][j][i] = series[j];
        }
      }
    }
  }
  return res;
}

oms_status_enu_t FitModel::solve(const char* reporttype)
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
  std::map<std::string, TimeSeries> var_mesData_map = to_varname_TimeSeries_Map(mdata_.measurementSeries, size);
  for (int i=0; i < size; ++i) {

    std::vector<double> timeSlice(mdata_.time.begin(), mdata_.time.begin()+(i+1));

    // (varname -> data[i], i \in [0..i]
    std::map<std::string, std::vector<double>> input;
    if (!mdata_.inputSeries.empty()) {
      // So far, only 1 input series is supported. Hence, several measurements must share the same input series
      for (auto varValues : mdata_.inputSeries[0]) {
        std::string varname = varValues.first;
        input[varname].resize(i+1);
        std::copy_n(varValues.second.begin(), i+1, input[varname].begin());
      }
    }

    // (varname -> data[iSeries],  iSeries \in [0..nSeries])
    std::map<std::string, std::vector<double>> mes;
    for (auto& varValues : var_mesData_map) {
      std::string varname = varValues.first;
      std::vector<std::vector<double>> data = varValues.second;
      mes[varValues.first] = data[i];
    }

    // use numeric differentiation to obtain the derivative (jacobian).
    DynamicNumericDiffCostFunction<OdeResidual>* cost_function =
      new DynamicNumericDiffCostFunction<OdeResidual>(
        new OdeResidual(timeSlice, mes, input, params, oms_modelIdent_)
      );
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

FitModel::FitModel(const char* oms_modelIdent) : oms_modelIdent_(oms_modelIdent)
{
  logTrace();
  options_.max_num_iterations = 25;
  options_.linear_solver_type = ceres::DENSE_QR;
  options_.minimizer_progress_to_stdout = true;
  state_ = FitModelState::CONSTRUCTED;
}

oms_status_enu_t FitModel::initialize(size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars)
{
  logTrace();
  oms_status_enu_t status = oms_status_ok;
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

oms_status_enu_t FitModel::addParameter(const char* var, double startvalue)
{
  logTrace();
  oms_status_enu_t status = oms_status_ok;
  if (state_ < FitModelState::INITIALIZED) {
    logError("FitModel::addParameter:  Calling method on uninitialized object.");
    return oms_status_error;
  }
  oms2_setRealParameter(var, startvalue); // FIXME superfluous?
  if (parameters_.find(var) != parameters_.end()) {
    std::string message = std::string("FitModel::addParameter: ") + var
      +  "already exists in parameter set. Overwriting entry.\n";
    logWarning(message);
    status = oms_status_warning;
  }
  parameters_[var] = ParameterAttributes {startvalue, startvalue};

  return status;
}

oms_status_enu_t FitModel::addMeasurement(size_t iSeries, const char* var, const double* values, size_t nValues)
{
  logTrace();
  oms_status_enu_t status = oms_status_ok;
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

oms_status_enu_t FitModel::addInput(const char* var, const double* values, size_t nValues)
{
  logTrace();
  oms_status_enu_t status = oms_status_ok;
  if (state_ < FitModelState::INITIALIZED) {
    logError("FitModel::addInput:  Calling method on uninitialized object.");
    return oms_status_error;
  }

  auto it = mdata_.inputSeries[0].find(var);
  if (it != mdata_.inputSeries[0].end()) {
    logWarning(std::string("FitModel::addInput: Input series for variable ")
      + var + std::string(" already exists. Overwriting!"));
    it->second.resize(nValues);
    std::copy(values, values+nValues, it->second.begin());
    status = oms_status_warning;
  } else {
    mdata_.inputSeries[0][var] = std::vector<double>(values, values+nValues);
  }
  return status;
}

FitModelState FitModel::getState()
{
  logTrace();
  return state_;
}

oms_status_enu_t FitModel::getParameter(const char* var, ParameterAttributes& attributes)
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

// Forget about below ...

struct OdeResidual_Backup {
  OdeResidual_Backup(std::vector<double> t, const std::map<std::string, std::vector<double>> mes /* std::vector<VarValueMap> mes */,
    std::vector<VarValueMap> input, std::set<std::string> params, const char* oms_modelIdent)
      : t_(t), mes_(mes), input_(input), params_(params), oms_modelIdent_(oms_modelIdent) {}

  bool operator()(double const* const* parameters, double* residual) const {
    {
      int i = 0;
      // precondition: order in 'parameters' corresponds to params_ set order
      for (auto varname: params_) {
        oms2_setReal(varname.c_str(), parameters[i][0]);
        // std::cout << "OdeResidual: " << varname << " = " << parameters[i][0] << std::endl;
        i++;
      }
    }
    oms2_setStopTime(oms_modelIdent_, t_.back()); // needed?

    oms2_initialize(oms_modelIdent_);
    // if (!input_.back().empty()) {
    //     // Set inputs if inputs are available
    //     const int num = t_.size();
    //     for (int i=0; i < num; ++i) {
    //         // set inputs at t[i]
    //         // FIXME Problem: We would need to run the simulation for each
    //         // input series individually. Current approach only has one simulation run
    //         // and computes residuals from all measurement series in one go.
    //         // This is efficient, but breaks down if one assumes that every measurement
    //         // series corresponds to a (different) input series
    //         oms2_stepUntil(oms_modelIdent_, t_[i]);
    //     }
    //
    // } else {
    //     oms2_stepUntil(oms_modelIdent_, t_.back());
    // }
    oms2_stepUntil(oms_modelIdent_, t_.back());

    // int nMesVars = mes_.at(0).size();
    int nMesVars = mes_.size();
    // std::cout << "nMesVars: " << nMesVars << std::endl;
    // get simulation values of observed variables
    std::vector<double> x(nMesVars);
    {
      int i = 0;
      //for (auto& varValue: mes_.at(0)) {
      for (auto& varValues: mes_) {
        oms2_getReal(varValues.first.c_str(), &(x[i]));
        i++;
      }
    }
    // Compute residual by subtracting simulation value from measured value
    // for (int i=0; i < mes_.size(); ++i) {
    //   int j = 0;
    //   for (auto& varValue : mes_.at(i)) {
    //     residual[i*nMesVars + j] = varValue.second - x.at(j);
    //     j++;
    //   }
    // }
    {
      int i = 0;
      for (auto& varValues: mes_) {
        // std::cout << "var: " << varValues.first << std::endl;
        for (int j=0; j < varValues.second.size(); ++j) {
          // std::cout << "Computing residual j=" << j << std::endl;
          // std::cout << "residual[i + (j*nMesVars)] = residual[" << i + (j*nMesVars) << "] = " << varValues.second[j] - x.at(i) << std::endl;
          residual[i + (j*nMesVars)] = varValues.second[j] - x.at(i);
          //std::cout << "Computing residual: done\n";
        }
        i++;
      }
    }

    oms2_reset(oms_modelIdent_);
    return true;
  }

 private:
  const std::vector<double> t_;
  // (varname -> input[iSeries],  iSeries \in [0..nSeries])
  const std::map<std::string, std::vector<double>> mes_;
  // const std::vector<VarValueMap> mes_;
  const std::vector<VarValueMap> input_;
  const std::set<std::string> params_;
  const char* oms_modelIdent_;
};
