#ifndef TLMBUSCONNECTOR_H
#define TLMBUSCONNECTOR_H


#include "ComRef.h"
#include "SignalRef.h"
#include "Types.h"
#include "Connector.h"
#include "ssd/ConnectorGeometry.h"

#include <string>
#include <vector>
#include <map>

#include <pugixml.hpp>

namespace oms3
{
  /**
   * \brief TLMBusConnector
   */
  class TLMBusConnector : protected oms3_tlmbusconnector_t
  {
  public:
    TLMBusConnector(const oms3::ComRef& name, const std::string domain, const int dimensions, const oms_tlm_interpolation_t interpolation);
    ~TLMBusConnector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    TLMBusConnector(const TLMBusConnector& rhs);
    TLMBusConnector& operator=(const TLMBusConnector& rhs);

    void setName(const oms3::ComRef& name);
    void setGeometry(const oms2::ssd::ConnectorGeometry* newGeometry);

    const oms3::ComRef getName() const {return oms3::ComRef(std::string(name));}
    const std::string getDomain() const {return domain;}
    const int getDimensions() const {return dimensions;}
    const oms_tlm_interpolation_t getInterpolation() const {return interpolation;}
    const oms2::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(geometry);}

    oms_status_enu_t addConnector(const oms3::ComRef& cref, std::string vartype);

  private:
    void updateVariableTypes();

    std::map<oms3::ComRef, std::string> connectors;
    std::vector<std::string> variableTypes; //Used to keep track of TLM variable types
  };
}

#endif // TLMBUSCONNECTOR_H
