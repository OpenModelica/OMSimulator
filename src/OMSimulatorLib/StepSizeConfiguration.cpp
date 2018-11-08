#include "StepSizeConfiguration.h"

//default bounds are the same as the default communicationInterval of Model
StepSizeConfiguration::StepSizeConfiguration() :minimalStepSize(1.0e-2), maximalStepSize(1.0e-2) {};

StepSizeConfiguration::~StepSizeConfiguration() {};

void StepSizeConfiguration::addEventIndicator(const oms2::SignalRef& signal) 
{
    eventIndicators.push_back(signal);
}

void StepSizeConfiguration::addTimeIndicator(const oms2::SignalRef& signal) 
{
    timeIndicators.push_back(signal);
}

void StepSizeConfiguration::addStaticValueIndicator(const oms2::SignalRef& signal, double lowerBound, double upperBound, double stepSize) 
{
    staticBound bound ={lowerBound,upperBound,stepSize};

    //check if it is the first entry for the signal
    bool isNew=true;
    for (auto& entry: staticIntervals)
    {
        //if not, add the new entry to the others
        if (entry.first == signal) {
            isNew=false;
            entry.second.push_back(bound);
        }
    }
    
    //otherwise add the signal to the others with the interval
    if (isNew) {
        std::vector<staticBound> bounds={bound};
        bounds.push_back(bound);
        staticIntervals.push_back(std::pair<const oms2::SignalRef,std::vector<staticBound>>(signal,bounds));
    }
}

void StepSizeConfiguration::addDynamicValueIndicator(const oms2::SignalRef& signal,const oms2::SignalRef& lower,const oms2::SignalRef& upper,double stepSize)
{
    dynamicBound bound={lower,upper,stepSize};
    
    //check if it is the first entry for the signal
    bool isNew=true;
    for (auto& entry: dynamicIntervals)
    {
        //if not, add the new entry to the others
        if (entry.first == signal) {
            isNew=false;
            entry.second.push_back(bound);
        }
    }

    //otherwise add the signal to the others with the interval
    if (isNew) {
        std::vector<dynamicBound> bounds;
        bounds.push_back(bound);
        dynamicIntervals.push_back(std::pair<const oms2::SignalRef,std::vector<dynamicBound>>(signal,bounds));
    }

}
