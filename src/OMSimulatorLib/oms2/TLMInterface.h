/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS2_TLM_INTERFACE_H_
#define _OMS2_TLM_INTERFACE_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "../Types.h"
#include "../../OMTLMSimulator/common/Plugin/PluginImplementer.h"

namespace oms2
{
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
    int Z = 24;
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
  int Z = 88;
} oms_tlm_sigrefs_3d_fg_t;

  class TLMInterface
  {
  public:
    TLMInterface(const oms2::ComRef& cref,
                 const std::string name,
                 oms_causality_enu_t causality,
                 const std::string domain,
                 const int dimensions,
                 oms_tlm_interpolation_t interpolation,
                 const std::vector<SignalRef> &sigrefs);
    ~TLMInterface() {}

    oms2::SignalRef getSignal() { return sig; }
    oms2::ComRef getSubModelName() { return cref; }
    std::string getName() { return name; }
    oms_causality_enu_t getCausality() { return causality; }
    std::string getDomain() { return domain; }
    int getDimensions() { return dimensions; }
    oms_tlm_interpolation_t getInterpolationMethod() { return interpolationMethod; }
    void setDelay(double delay) { this->delay = delay; }
    double getDelay() { return this->delay; }
    std::vector<SignalRef> getSubSignals() { return sigrefs; }
    std::vector<SignalRef> getSubSignalSet(std::vector<int> ids);
    SignalRef getSubSignal(int i) { return sigrefs[i]; }
    oms_status_enu_t doRegister(TLMPlugin *plugin);
    int getId() { return this->id; }

    friend bool operator==(const TLMInterface& lhs, const TLMInterface& rhs);
    friend bool operator!=(const TLMInterface& lhs, const TLMInterface& rhs);

  private:
    SignalRef sig;
    oms2::ComRef cref;
    std::string name;
    oms_causality_enu_t causality;
    std::string domain;
    int dimensions;
    std::vector<SignalRef> sigrefs;
    oms_tlm_interpolation_t interpolationMethod;
    double delay;
    int id;
  };

  inline bool operator==(const TLMInterface& lhs, const TLMInterface& rhs) {return (lhs.cref.c_str() == rhs.cref.c_str() && lhs.name == rhs.name);}
  inline bool operator!=(const TLMInterface& lhs, const TLMInterface& rhs) {return !(lhs == rhs);}
}

#endif
