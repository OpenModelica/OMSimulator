#ifndef _OMS_STEP_SIZE_CONFIGURATION_H_
#define _OMS_STEP_SIZE_CONFIGURATION_H_

#include "ComRef.h"
#include "Types.h"
#include <vector>

namespace oms
{
  class StepSizeConfiguration
  {
    //auxiliary data structures
    struct StaticBound
    {
      double lower;
      double upper;
      double stepSize;
    };

    struct DynamicBound
    {
      const ComRef lower;
      const ComRef upper;
      double stepSize;
    };

  public:
    StepSizeConfiguration() {}
    ~StepSizeConfiguration() {}

    // data structure class - only getters and setters
    oms_status_enu_t addDynamicValueIndicator(const ComRef& signal,const ComRef& lower,const ComRef& upper,double stepSize);
    oms_status_enu_t addEventIndicator(const ComRef& signal);
    oms_status_enu_t addStaticValueIndicator(const ComRef& signal, double lowerBound, double upperBound, double stepSize);
    oms_status_enu_t addTimeIndicator(const ComRef& signal);
    std::vector<ComRef> getEventIndicators() {return eventIndicators;}
    std::vector<ComRef> getTimeIndicators() {return timeIndicators;}
    std::vector<std::pair<const ComRef,std::vector<DynamicBound>>> getDynamicThresholds(){return dynamicIntervals;}
    std::vector<std::pair<const ComRef,std::vector<StaticBound>>> getStaticThresholds() {return staticIntervals;}

  private:
    std::vector<ComRef> eventIndicators;
    std::vector<ComRef> timeIndicators;
    std::vector<std::pair<const ComRef,std::vector<StaticBound>>> staticIntervals;
    std::vector<std::pair<const ComRef,std::vector<DynamicBound>>> dynamicIntervals;
  };
}

#endif
