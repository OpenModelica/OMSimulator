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

#ifndef _OMS2_SCOPE_H_
#define _OMS2_SCOPE_H_

#include "../Types.h"
#include "CompositeModel.h"
#include "ComRef.h"
#include "Connection.h"
#include "FMICompositeModel.h"
#include "FMUInfo.h"
#include "Model.h"
#include "ssd/ConnectionGeometry.h"
#include "ssd/ElementGeometry.h"
#include "TLMCompositeModel.h"

#include <map>
#include <string>

namespace oms2
{
  class Scope
  {
  public:
    /**
     * This is instance is used as global scope. It would also be possible to
     * have several independent local scopes.
     */
    static Scope& GetInstance();

    oms_status_enu_t newFMIModel(const ComRef& name);
    oms_status_enu_t newTLMModel(const ComRef& name);
    oms_status_enu_t unloadModel(const ComRef& name);
    oms_status_enu_t addFMU(const ComRef& modelIdent, const std::string& fmuPath, const ComRef& fmuIdent);
    oms_status_enu_t deleteSubModel(const ComRef& modelIdent, const ComRef& subModelIdent);
    oms_status_enu_t rename(const ComRef& identOld, const ComRef& identNew);
    Model* loadModel(const std::string& filename);
    oms_status_enu_t saveModel(const std::string& filename, const ComRef& name);
    oms_status_enu_t getElement(const ComRef& cref, oms2::Element** element);
    oms_status_enu_t setElementGeometry(const ComRef& cref, const oms2::ssd::ElementGeometry* geometry);
    oms_status_enu_t getElements(const ComRef& cref, oms2::Element*** elements);
    oms_status_enu_t getFMUPath(const ComRef& cref, char** path);
    oms_status_enu_t getFMUInfo(const ComRef& cref, const oms2::FMUInfo** fmuInfo);
    oms_status_enu_t getConnections(const ComRef& cref, oms2::Connection*** connections);
    oms_status_enu_t addConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);
    oms_status_enu_t deleteConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);
    oms_status_enu_t updateConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB, const oms2::Connection* connection);
    oms_status_enu_t getRealParameter(const oms2::SignalRef& signal, double& value);
    oms_status_enu_t setRealParameter(const oms2::SignalRef& signal, double value);
    oms_status_enu_t getIntegerParameter(const oms2::SignalRef& signal, int& value);
    oms_status_enu_t setIntegerParameter(const oms2::SignalRef& signal, int value);
    oms_status_enu_t getBooleanParameter(const oms2::SignalRef& signal, bool& value);
    oms_status_enu_t setBooleanParameter(const oms2::SignalRef& signal, bool value);
    oms_status_enu_t setTempDirectory(const std::string& newTempDir);
    oms_status_enu_t setWorkingDirectory(const std::string& path);

    const std::string& getTempDirectory() {return GetInstance().tempDir;}
    const std::string& getWorkingDirectory() {return GetInstance().workingDir;}

    oms_status_enu_t renameModel(const ComRef& identOld, const ComRef& identNew);
    Model* getModel(const ComRef& name);
    oms2::Connection* getConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);

    FMICompositeModel* getFMICompositeModel(const ComRef& name);
    TLMCompositeModel* getTLMCompositeModel(const ComRef& name);

  private:
    Scope();
    ~Scope();

    // stop the compiler generating methods copying the object
    Scope(Scope const& copy);            // not implemented
    Scope& operator=(Scope const& copy); // not implemented

  private:
    std::map<oms2::ComRef, oms2::Model*> models;
    std::string tempDir;
    std::string workingDir;
  };
}

#endif
