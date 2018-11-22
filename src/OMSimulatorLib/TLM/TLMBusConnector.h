#ifndef TLMBUSCONNECTOR_H
#define TLMBUSCONNECTOR_H


#include "ComRef.h"
#include "SignalRef.h"
#include "Types.h"
#include "Connector.h"
#include "ssd/ConnectorGeometry.h"
#include "../../OMTLMSimulator/common/Plugin/PluginImplementer.h"

#include <string>
#include <vector>
#include <map>

#include <pugixml.hpp>

namespace oms3
{
class Component;
class System;

typedef struct  {
    int y = 0;
} oms_tlm_sigrefs_signal_t;

typedef struct  {
    int x = 0;
    int v = 1;
    int f = 2;
} oms_tlm_sigrefs_1d_t;

typedef struct  {
    int x = 0;
    int v = 1;
    int c = 2;
    int Z = 3;
} oms_tlm_sigrefs_1d_cg_t;

typedef struct  {
    int x = 0;
    int v = 1;
    std::vector<int> c = {2,3,4,5,6,7,8,9,10,11};
    std::vector<int> t = {12,13,14,15,16,17,18,19,20,21};
    int Z = 22;
} oms_tlm_sigrefs_1d_fg_t;

typedef struct  {
    std::vector<int> x   = {0,1};
    std::vector<int> phi = {2};
    std::vector<int> v   = {3,4};
    std::vector<int> w   = {5};
    std::vector<int> f   = {6,7,8};
} oms_tlm_sigrefs_2d_t;

typedef struct  {
    std::vector<int> x = {0,1,2};
    std::vector<int> A = {3,4,5,6,7,8,9,10,11};
    std::vector<int> v = {12,13,14};
    std::vector<int> w = {15,16,17};
    std::vector<int> f = {18,19,20,21,22,23};
} oms_tlm_sigrefs_3d_t;

typedef struct  {
    std::vector<int> x = {0,1,2};
    std::vector<int> A = {3,4,5,6,7,8,9,10,11};
    std::vector<int> v = {12,13,14};
    std::vector<int> w = {15,16,17};
    std::vector<int> c = {18,19,20,21,22,23};
    int Zt = 24;
    int Zr = 25;
} oms_tlm_sigrefs_3d_cg_t;

typedef struct  {
    std::vector<int> x =  {0,1,2};
    std::vector<int> A =  {3,4,5,6,7,8,9,10,11};
    std::vector<int> v =  {12,13,14};
    std::vector<int> w =  {15,16,17};
    std::vector< std::vector<int> > c = { {18,28,38,48,58,68},
                                          {19,29,39,49,59,69},
                                          {20,30,40,50,60,70},
                                          {21,31,41,51,61,71},
                                          {22,32,42,52,62,72},
                                          {23,33,43,53,63,73},
                                          {24,34,44,54,64,74},
                                          {25,35,45,55,65,75},
                                          {26,36,46,56,66,76},
                                          {27,37,47,57,67,77} };
    std::vector<int> t = { 78,79,80,81,82,83,84,85,86,87};
    int Zt = 88;
    int Zr = 89;
} oms_tlm_sigrefs_3d_fg_t;

  /**
   * \brief TLMBusConnector
   */
  class TLMBusConnector : protected oms3_tlmbusconnector_t
  {
  public:
    TLMBusConnector(const oms3::ComRef& name, oms_tlm_domain_t domain, const int dimensions, const oms_tlm_interpolation_t interpolation, System* parentSystem=nullptr, Component* component=nullptr);
    ~TLMBusConnector();

    oms_status_enu_t exportToSSD(pugi::xml_node& root) const;

    // methods to copy the object
    TLMBusConnector(const TLMBusConnector& rhs);
    TLMBusConnector& operator=(const TLMBusConnector& rhs);

    void setName(const oms3::ComRef& name);
    void setGeometry(const oms2::ssd::ConnectorGeometry* newGeometry);

    void setReal(int i, double value);
    void getReal(int i, double &value);
    void setReals(std::vector<int> i, std::vector<double> values);
    void getReals(std::vector<int> i, std::vector<double>& values);
    void setRealInputDerivatives(int i, int order, double value);

    const oms3::ComRef getName() const {return oms3::ComRef(name);}
    oms_tlm_domain_t getDomain() const {return domain;}
    const std::string getDomainString() const;
    const int getDimensions() const {return dimensions;}
    const oms_causality_enu_t getCausality() const {return causality;}
    const oms_tlm_interpolation_t getInterpolation() const {return interpolation;}
    void setDelay(double delay) { this->delay = delay; }
    double getDelay() { return this->delay; }
    oms3::ComRef getConnector(int id) const;
    std::vector<oms3::ComRef> getConnectors(std::vector<int> ids) const;
    const oms2::ssd::ConnectorGeometry* getGeometry() const {return reinterpret_cast<oms2::ssd::ConnectorGeometry*>(geometry);}

    oms_status_enu_t addConnector(const oms3::ComRef& cref, std::string vartype);
    oms_status_enu_t deleteConnector(const oms3::ComRef& cref);
    void updateConnectors();
    void sortConnectors();
    oms_status_enu_t registerToSockets(TLMPlugin *plugin);
    int getId() const {return id;}

    oms3::Component* getComponent();
    oms3::TLMBusConnector* getActualBus();

    static std::vector<std::string>  getVariableTypes(oms_tlm_domain_t domain, int dimensions, oms_tlm_interpolation_t interpolation);
    static std::vector<std::string>  getVariableDescriptions(oms_tlm_domain_t domain, int dimensions, oms_tlm_interpolation_t interpolation);

  private:
    oms3::Component* getComponent(const ComRef &conA, System *system) const;
    oms3::TLMBusConnector* getActualBus(ComRef cref, System *system);

    oms_causality_enu_t causality;
    std::map<std::string, oms3::ComRef> connectors;
    std::vector<oms3::ComRef> sortedConnectors;
    std::vector<std::string> variableTypes; //Used to keep track of TLM variable types
    System* parentSystem;
    Component* component = nullptr;
    TLMBusConnector *actualBus = nullptr;

    int id;
  };
}

#endif // TLMBUSCONNECTOR_H
