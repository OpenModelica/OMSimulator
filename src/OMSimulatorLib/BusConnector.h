#ifndef BUSCONNECTOR_H
#define BUSCONNECTOR_H


#include "ComRef.h"
#include "OMSimulator/Types.h"
#include "Connector.h"
#include "ssd/ConnectorGeometry.h"

#include <string>
#include <vector>

#include <pugixml.hpp>

namespace oms
{
  /**
   * \brief BusConnector
   */
  class BusConnector : protected oms_busconnector_t
  {
  public:
    BusConnector(const oms::ComRef& name);
    ~BusConnector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    BusConnector(const BusConnector& rhs);
    BusConnector& operator=(const BusConnector& rhs);

    void setName(const oms::ComRef& name);
    void setGeometry(const oms::ssd::ConnectorGeometry* newGeometry);

    const oms::ComRef getName() const {return oms::ComRef(name);}
    const oms::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms::ssd::ConnectorGeometry*>(geometry);}

    oms_status_enu_t addConnector(const oms::ComRef& cref);
    oms_status_enu_t deleteConnector(const oms::ComRef& cref);
    std::vector<oms::ComRef> getConnectors() const {return conrefs;}

    void updateConnectors();

  private:
    std::vector<oms::ComRef> conrefs;
  };
}

#endif // BUSCONNECTOR_H
