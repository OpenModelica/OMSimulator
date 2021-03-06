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
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_SNAPSHOT_H_
#define _OMS_SNAPSHOT_H_

#include "ComRef.h"
#include "OMSFileSystem.h"
#include "ssd/Tags.h"
#include "Types.h"

#include <pugixml.hpp>
#include <string>
#include <vector>

namespace oms
{
  class Snapshot
  {
  public:
    Snapshot(bool partial=false);
    ~Snapshot();

    oms_status_enu_t import(const char* snapshot);
    oms_status_enu_t importResourceFile(const filesystem::path& filename, const filesystem::path& root);
    oms_status_enu_t importResourceMemory(const filesystem::path& filename, const char* contents);
    oms_status_enu_t importResourceNode(const filesystem::path& filename, const pugi::xml_node& node);

    bool isPartialSnapshot() const;

    pugi::xml_node newResourceNode(const filesystem::path& filename);
    pugi::xml_node operator[](const filesystem::path& filename);

    void getResources(std::vector<std::string>& resources) const;
    pugi::xml_node getResourceNode(const filesystem::path& filename) const;

    pugi::xml_node getTemplateResourceNodeSSD(const filesystem::path& filename);
    pugi::xml_node getTemplateResourceNodeSSV(const filesystem::path& filename);
    oms_status_enu_t exportPartialSnapshot(const ComRef& cref, Snapshot& partialSnapshot);
    oms_status_enu_t importPartialSnapshot(const ComRef& cref, const char* fullsnapshot);

    void debugPrintNode(const filesystem::path& filename) const;
    void debugPrintAll() const;

    oms_status_enu_t writeDocument(char** contents);

  private:
    oms_status_enu_t importPartialResourceNode(const filesystem::path& filename, const filesystem::path& nodename, const pugi::xml_node& node);

  private:
    // stop the compiler generating methods copying the object
    Snapshot(Snapshot const& copy);            ///< not implemented
    Snapshot& operator=(Snapshot const& copy); ///< not implemented

  private:
   pugi::xml_document doc; // snapshot document
  };
}

#endif
