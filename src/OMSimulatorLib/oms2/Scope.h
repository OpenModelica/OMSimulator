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
#include "ComRef.h"
#include "Connection.h"
#include "FMICompositeModel.h"
#include "CompositeModel.h"
#include "ssd/ConnectionGeometry.h"
#include "ssd/ElementGeometry.h"
#include "TLMCompositeModel.h"

#include <map>
#include <string>

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

namespace oms2
{
  class Scope
  {
  public:
    static oms_status_enu_t newFMIModel(const ComRef& name);
    static oms_status_enu_t newTLMModel(const ComRef& name);
    static CompositeModel* loadModel(const std::string& filename);
    static oms_status_enu_t saveModel(const std::string& filename, const ComRef& name);

    static oms_status_enu_t unloadModel(const ComRef& name);

    static oms_status_enu_t addFMU(const ComRef& modelIdent, const std::string& fmuPath, const ComRef& fmuIdent);
    static oms_status_enu_t deleteSubModel(const ComRef& modelIdent, const ComRef& subModelIdent);

    static oms_status_enu_t rename(const ComRef& identOld, const ComRef& identNew);

    static oms_status_enu_t SetTempDirectory(const std::string& newTempDir);
    static const std::string& GetTempDirectory() {Scope &scope = getInstance(); return scope.tempDir;}

    static oms_status_enu_t SetWorkingDirectory(const std::string& path);
    static const std::string& GetWorkingDirectory() {Scope &scope = getInstance(); return scope.workingDir;}

    static oms_status_enu_t getElement(const ComRef& cref, oms2::Element** element);
    static oms_status_enu_t getElements(const ComRef& cref, oms2::Element*** elements);
    static oms_status_enu_t getFMUPath(const ComRef& cref, char** path);
    static oms_status_enu_t getConnections(const ComRef& cref, oms2::Connection*** connections);

    static oms_status_enu_t setElementGeometry(const ComRef& cref, const oms2::ssd::ElementGeometry* geometry);

    static oms_status_enu_t addConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);
    static oms_status_enu_t deleteConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);
    static oms_status_enu_t updateConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB, const oms2::Connection* connection);

    oms_status_enu_t renameModel(const ComRef& identOld, const ComRef& identNew);
    CompositeModel* getModel(const ComRef& name);
    oms2::Connection* getConnection(const ComRef& cref, const SignalRef& conA, const SignalRef& conB);

    static oms_status_enu_t getRealParameter(const oms2::SignalRef& signal, double& value);
    static oms_status_enu_t setRealParameter(const oms2::SignalRef& signal, double value);

  private:
    Scope();
    ~Scope();

    // stop the compiler generating methods copying the object
    Scope(Scope const& copy);            // not implemented
    Scope& operator=(Scope const& copy); // not implemented

    static Scope& getInstance();

    static CompositeModel* loadFMIModel(const pugi::xml_node& xml);
    static CompositeModel* loadTLMModel(const pugi::xml_node& xml);

    oms_status_enu_t saveFMIModel(oms2::FMICompositeModel* model, const std::string& filename);
    oms_status_enu_t saveTLMModel(oms2::TLMCompositeModel* model, const std::string& filename);

  private:
    std::map<ComRef, CompositeModel*> models;
    std::string tempDir;
    std::string workingDir;
  };
}

#endif
