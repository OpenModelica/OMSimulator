#include "TLMWrapper.h"

oms3::TLMWrapper *oms3::TLMWrapper::NewTLMWrapperFromSystem(oms3::SystemWC *system)
{
  TLMWrapper* wrapper = new TLMWrapper(system);
  return wrapper;
}

oms3::TLMWrapper *oms3::TLMWrapper::NewTLMWrapperFromExternalModel(oms3::ExternalModel *externalmodel)
{
  TLMWrapper* wrapper = new TLMWrapper(externalmodel);
  return wrapper;
}

oms3::TLMWrapper::TLMWrapper(oms3::SystemWC *system)
{
  this->system = system;
}

oms3::TLMWrapper::TLMWrapper(oms3::ExternalModel *externalmodel)
{
  this->externalmodel = externalmodel;
}
