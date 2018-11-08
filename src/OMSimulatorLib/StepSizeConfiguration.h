#ifndef _OMS_STEP_SIZE_CONFIGURATION_H_
#define _OMS_STEP_SIZE_CONFIGURATION_H_

#include "SignalRef.h"

#include <vector>

//auxiliary data structures
struct staticBound {
  double lower;
  double upper;
  double stepSize;
};

struct dynamicBound {
  const oms2::SignalRef lower;
  const oms2::SignalRef upper;
  double stepSize;
};

class StepSizeConfiguration {
  public:
    StepSizeConfiguration();
    ~StepSizeConfiguration();
        
    //data structure class - only getters and setters
    void setMinimalStepSize(double min) {minimalStepSize=min;};
    double getMinimalStepSize() {return minimalStepSize;};
    void setMaximalStepSize(double max) {maximalStepSize=max;};
    double getMaximalStepSize() {return maximalStepSize;};
    void addEventIndicator(const oms2::SignalRef& signal);
    std::vector<oms2::SignalRef> getEventIndicators() {return eventIndicators;};
    void addTimeIndicator(const oms2::SignalRef& signal);
    std::vector<oms2::SignalRef> getTimeIndicators() {return timeIndicators;};
    void addStaticValueIndicator(const oms2::SignalRef& signal, double lowerBound, double upperBound, double stepSize);
    void addDynamicValueIndicator(const oms2::SignalRef& signal,const oms2::SignalRef& lower,const oms2::SignalRef& upper,double stepSize);
    std::vector<std::pair<const oms2::SignalRef,std::vector<staticBound>>> getStaticThresholds() {return staticIntervals;};
    std::vector<std::pair<const oms2::SignalRef,std::vector<dynamicBound>>> getDynamicThresholds(){return dynamicIntervals;};

  private:
    double minimalStepSize;
    double maximalStepSize;
    std::vector<oms2::SignalRef> eventIndicators;
    std::vector<oms2::SignalRef> timeIndicators;
    std::vector<std::pair<const oms2::SignalRef,std::vector<staticBound>>> staticIntervals;
    std::vector<std::pair<const oms2::SignalRef,std::vector<dynamicBound>>> dynamicIntervals;
};

#endif