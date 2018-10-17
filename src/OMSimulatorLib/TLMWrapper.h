#ifndef TLMWRAPPER_H
#define TLMWRAPPER_H

#include "Types.h"
#include "ExternalModel.h"

namespace oms3 {

class SystemWC;

class TLMWrapper
{
public:
  static TLMWrapper* NewTLMWrapperFromSystem(SystemWC *system);
  static TLMWrapper* NewTLMWrapperFromExternalModel(ExternalModel *externalmodel);
private:
  TLMWrapper(SystemWC *system);
  TLMWrapper(ExternalModel *externalmodel);

  SystemWC *system = nullptr;
  ExternalModel *externalmodel = nullptr;
};

}

#endif // TLMWRAPPER_H
