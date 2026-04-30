/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_SCOPE_H_
#define _OMS_SCOPE_H_

#include "ComRef.h"
#include "Model.h"
#include "OMSimulator/Types.h"

#include <map>
#include <string>
#include <vector>

namespace oms
{
  class Scope
  {
  private:
    Scope();
    ~Scope();

    // stop the compiler generating methods copying the object
    Scope(Scope const&);            ///< not implemented
    Scope& operator=(Scope const&); ///< not implemented

  public:
    /**
     * This is instance is used as global scope. It would also be possible to
     * have several independent local scopes.
     */
    static Scope& GetInstance();

    Model* newModel(const ComRef& cref);
    oms_status_enu_t deleteModel(const ComRef& cref);
    oms_status_enu_t renameModel(const ComRef& cref, const ComRef& newCref);
    oms_status_enu_t exportModel(const ComRef& cref, const std::string& filename);
    oms_status_enu_t importModel(const std::string& filename, char** cref);
    oms_status_enu_t setTempDirectory(const std::string& newTempDir);
    oms_status_enu_t setWorkingDirectory(const std::string& newWorkingDir);
    oms_status_enu_t getElement(const ComRef& cref, oms::Element** element);
    oms_status_enu_t getElements(const ComRef& cref, oms::Element*** elements);
    Model* getModel(const ComRef& cref);

    oms_status_enu_t loadSnapshot(const ComRef& cref, const char* snapshot, char** newCref);
    oms_status_enu_t importSnapshot(const ComRef& cref, const char* snapshot, char** newCref);

    const std::string& getTempDirectory() const {return GetInstance().tempDir;}
    const std::string& getWorkingDirectory();

    static oms_status_enu_t miniunz(const std::string& filename, const std::string& extractdir);

  private:
    std::vector<Model*> models; ///< last element is always NULL
    std::map<ComRef, unsigned int> models_map;

    std::string tempDir;
    std::string workDir;
  };
}

#endif
