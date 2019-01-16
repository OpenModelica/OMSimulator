#ifndef BUSCONNECTOR_H
#define BUSCONNECTOR_H


#include "ComRef.h"
#include "Types.h"
#include "Connector.h"
#include "ssd/ConnectorGeometry.h"

#include <string>
#include <vector>

#include <pugixml.hpp>

namespace oms3
{
  /**
   * \brief BusConnector
   */
  class BusConnector : protected oms3_busconnector_t
  {
  public:
    BusConnector(const oms3::ComRef& name);
    ~BusConnector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    BusConnector(const BusConnector& rhs);
    BusConnector& operator=(const BusConnector& rhs);

    void setName(const oms3::ComRef& name);
    void setGeometry(const oms3::ssd::ConnectorGeometry* newGeometry);

    const oms3::ComRef getName() const {return oms3::ComRef(name);}
    const oms3::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms3::ssd::ConnectorGeometry*>(geometry);}

    oms_status_enu_t addConnector(const oms3::ComRef& cref);
    oms_status_enu_t deleteConnector(const oms3::ComRef& cref);
    std::vector<oms3::ComRef> getConnectors() const {return conrefs;}

    void updateConnectors();

  private:
    std::vector<oms3::ComRef> conrefs;
  };
}

#endif // BUSCONNECTOR_H
