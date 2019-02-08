#include "StepSizeConfiguration.h"

oms_status_enu_t oms::StepSizeConfiguration::addEventIndicator(const ComRef& signal)
{
  eventIndicators.push_back(signal);
  return oms_status_ok;
}

oms_status_enu_t oms::StepSizeConfiguration::addTimeIndicator(const ComRef& signal)
{
  timeIndicators.push_back(signal);
  return oms_status_ok;
}

oms_status_enu_t oms::StepSizeConfiguration::addStaticValueIndicator(const ComRef& signal, double lowerBound, double upperBound, double stepSize)
{
  StaticBound bound = {lowerBound, upperBound, stepSize};

  //check if it is the first entry for the signal
  bool isNew = true;
  for (auto& entry: staticIntervals)
  {
    //if not, add the new entry to the others
    if (entry.first == signal)
    {
      isNew=false;
      entry.second.push_back(bound);
    }
  }

  //otherwise add the signal to the others with the interval
  if (isNew)
  {
    std::vector<StaticBound> bounds = {bound};
    bounds.push_back(bound);
    staticIntervals.push_back(std::pair<const ComRef, std::vector<StaticBound>>(signal, bounds));
  }
  return oms_status_ok;
}

oms_status_enu_t oms::StepSizeConfiguration::addDynamicValueIndicator(const ComRef& signal, const ComRef& lower, const ComRef& upper, double stepSize)
{
  DynamicBound bound = {lower, upper, stepSize};

  //check if it is the first entry for the signal
  bool isNew = true;
  for (auto& entry: dynamicIntervals)
  {
    //if not, add the new entry to the others
    if (entry.first == signal)
    {
      isNew = false;
      entry.second.push_back(bound);
    }
  }

  //otherwise add the signal to the others with the interval
  if (isNew)
  {
    std::vector<DynamicBound> bounds;
    bounds.push_back(bound);
    dynamicIntervals.push_back(std::pair<const ComRef, std::vector<DynamicBound>>(signal, bounds));
  }
  return oms_status_ok;
}
