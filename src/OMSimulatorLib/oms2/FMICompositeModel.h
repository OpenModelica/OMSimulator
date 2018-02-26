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

#ifndef _OMS_FMI_COMPOSITE_MODEL_H_
#define _OMS_FMI_COMPOSITE_MODEL_H_

#include "ComRef.h"
#include "SignalRef.h"
#include "FMISubModel.h"
#include "Model.h"
#include "Connection.h"
#include "../Types.h"

#include <map>
#include <deque>

namespace oms2
{
  class FMICompositeModel : public Model
  {
  public:
    static FMICompositeModel* newModel(const ComRef& name);

    oms_component_type_t getType() {return oms_component_fmi;}
    oms_status_t instantiateFMU(const std::string& filename, const ComRef& cref);
    oms_status_t instantiateTable(const std::string& filename, const ComRef& cref);

    oms_status_t setRealParameter(const oms2::SignalRef& sr, double value);

    oms_status_t addConnection(const oms2::Connection& connection);
    oms_status_t addConnection(const SignalRef& signalA, const SignalRef& signalB);

    FMISubModel* getSubModel(const ComRef& cref);
    const std::map<ComRef, FMISubModel*>& getSubModels() {return subModels;}
    const std::deque<oms2::Connection>& getConnections() {return connections;}
    oms_connection_t** getOMSConnections();
    oms2::Connection* getConnection(const oms2::SignalRef& signalA, const oms2::SignalRef& signalB);

    void updateComponents();

  private:
    FMICompositeModel(const ComRef& name);
    ~FMICompositeModel();

    // stop the compiler generating methods copying the object
    FMICompositeModel(FMICompositeModel const& copy);            // not implemented
    FMICompositeModel& operator=(FMICompositeModel const& copy); // not implemented

  private:
    std::map<ComRef, FMISubModel*> subModels;
    std::deque<oms2::Connection> connections;
    oms_connection_t** oms_connections;
  };
}

#endif
