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

#ifndef _SSD_SYSTEM_GEOMETRY_H_
#define _SSD_SYSTEM_GEOMETRY_H_

#include "OMSimulator/Types.h"

#include <string>

namespace oms
{
  namespace ssd
  {
    class SystemGeometry : protected ssd_system_geometry_t
    {
    public:
      SystemGeometry();
      SystemGeometry(const SystemGeometry& rhs);
      ~SystemGeometry();

      SystemGeometry& operator=(SystemGeometry const& rhs);

      void setSizePosition(double x1, double y1, double x2, double y2) {this->x1 = x1; this->y1 = y1; this->x2 = x2; this->y2 = y2;}

      double getX1() const {return x1;}
      double getY1() const {return y1;}
      double getX2() const {return x2;}
      double getY2() const {return y2;}
    };
  }
}

#endif
