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
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _FIT_MODEL_
#define _FIT_MODEL_

#include <string>
#include <set>
#include <vector>
#include <map>
#include <memory>
#include <OMSimulator.h>
#include "ceres/ceres.h"

// TODO? upper and lower bounds, confidence intervals?
struct ParameterAttributes {
  double startValue;
  double estValue;
};
typedef std::map<std::string, ParameterAttributes> ParameterMap;
typedef std::map<std::string, std::vector<double> > SeriesMap;
// TODO? variances for each measurement noise?
struct MeasurementData
{
  std::vector<double> time;
  std::set<std::string> inputVars;
  std::set<std::string> measurementVars;
  size_t nSeries;
  std::vector<SeriesMap> inputSeries; //!< So far there is only support for one input series, i.e., inputSeries.size() <= 1
  // [iSeries](varname -> mes[i]), iSeries \in [0..nSeries], i \in [0..time.size])
  std::vector<SeriesMap> measurementSeries;
};

typedef std::vector<double> InstantSamples; //!< 1 .. nSeries (measurement) values that correspond to the same logical sampling instant time[i], i \in [0..time.size)
typedef std::vector<InstantSamples> TimeSeries; //!< ordered series of (measurement) instants i = (0 .. time.size-1)
typedef std::map<std::string,double> VarValueMap;

enum class FitModelState
{
  CONSTRUCTED, //!< State after calling class constructor
  INITIALIZED, //!< State after calling FitModel::initialize
  CONVERGENCE, //!< After calling FitModel::solve if minimizer returned with ceres::TerminationType::CONVERGENCE
  NO_CONVERGENCE, //!< After calling FitModel::solve if minimizer returned with ceres::TerminationType::NO_CONVERGENCE
  FAILURE //!< After calling FitModel::solve if minimizer returned with ceres::TerminationType::FAILURE
};


/**
 * \brief Utility function to rearrange representation of sampled mesurement/input data
 *
 * \input series The data series in the format as saved in MeasurementData
 * \input nTimeSample The number of measurement/input instants (e.g., MeasurementData.time.size())
 * \return Data structure of the form (varname -> data[i][iSeries], i \in [0..time.size], iSeries \in [0..nSeries])
 */
std::map<std::string, TimeSeries> to_varname_TimeSeries_Map(std::vector<SeriesMap> series, size_t nTimeSamples);


/**
 * \brief Class for setting up a fitting model.
 */
class FitModel
{
public:
  explicit FitModel(const char* oms_modelIdent);
  ~FitModel() noexcept {}
  /** Copy constructor */
  FitModel(const FitModel& other) = delete;
  /** Move constructor */
  FitModel (FitModel&& other) noexcept = delete; /* noexcept needed to enable optimizations in containers */
  /** Copy assignment operator */
  FitModel& operator= (const FitModel& other) = delete;
  /** Move assignment operator */
  FitModel& operator= (FitModel&& other) noexcept = delete;

  std::string toString() const;
  oms_status_enu_t initialize(size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars);
  oms_status_enu_t addParameter(const char* var, double startvalue);
  oms_status_enu_t addMeasurement(size_t iSeries, const char* var, const double* values, size_t nValues);
  oms_status_enu_t addInput(const char* var, const double* values, size_t nValues);
  FitModelState getState();
  oms_status_enu_t getParameter(const char* var, ParameterAttributes& attributes);
  bool isDataComplete() const;
  void setOptions_max_num_iterations(size_t max_num_iterations=25);
  oms_status_enu_t solve(const char* reporttype="BriefReport");

private:
  friend std::ostream& operator<< (std::ostream& os, const FitModel& a) { return os << a.toString(); }
  const char* oms_modelIdent_;
  FitModelState state_;
  ParameterMap parameters_;
  std::vector<double*> parameter_blocks_; // Ceres parameter blocks, will point to elements from parameters_[].estValue
  MeasurementData mdata_;
  ceres::Problem problem_;
  ceres::Solver::Options options_;
};

#endif // _FIT_MODEL_
