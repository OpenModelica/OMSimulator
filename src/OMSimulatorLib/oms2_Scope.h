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

#ifndef _OMS_SCOPE_H_
#define _OMS_SCOPE_H_

#include "oms2_ComRef.h"
#include "oms2_Model.h"
#include "oms2_FMICompositeModel.h"
#include "oms2_TLMCompositeModel.h"
#include "Types.h"

#include <map>
#include <string>

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

namespace oms2
{
  class Scope
  {
  public:
    static oms_status_t newFMIModel(const ComRef& name);
    static oms_status_t newTLMModel(const ComRef& name);
    static oms_status_t unloadModel(const ComRef& name);
    static oms_status_t renameModel(const ComRef& identOld, const ComRef& identNew);
    Model* getModel(const ComRef& name);
    static Model* loadModel(const std::string& filename);
    static oms_status_t saveModel(const std::string& filename, const ComRef& name);

    static oms_status_t SetTempDirectory(const std::string& newTempDir);
    static const std::string& GetTempDirectory() {Scope &scope = getInstance(); return scope.tempDir;}

    static oms_status_t getElementGeometry(const ComRef& cref, const oms_element_geometry_t** geometry);
    static oms_status_t setElementGeometry(const ComRef& cref, const oms_element_geometry_t* geometry);

    static oms_status_t getComponents(const ComRef& cref, oms_component_t*** components);
  private:
    Scope();
    ~Scope();

    // stop the compiler generating methods copying the object
    Scope(Scope const& copy);            // not implemented
    Scope& operator=(Scope const& copy); // not implemented

    static Scope& getInstance();

    static Model* loadFMIModel(const pugi::xml_node& xml);
    static Model* loadTLMModel(const pugi::xml_node& xml);

    oms_status_t saveFMIModel(oms2::FMICompositeModel* model, const std::string& filename);
    oms_status_t saveTLMModel(oms2::TLMCompositeModel* model, const std::string& filename);

  private:
    std::map<ComRef, Model*> models;
    std::string tempDir;
  };
}

#endif
