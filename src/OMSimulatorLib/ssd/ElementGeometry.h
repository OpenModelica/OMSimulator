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

#ifndef _SSD_ELEMENT_GEOMETRY_H_
#define _SSD_ELEMENT_GEOMETRY_H_

#include "../Types.h"

#include <pugixml.hpp>

#include <string>

namespace oms3
{
  namespace ssd
  {
    class ElementGeometry : protected ssd_element_geometry_t
    {
    public:
      ElementGeometry();
      ElementGeometry(const ElementGeometry& rhs);
      ~ElementGeometry();

      ElementGeometry& operator=(ElementGeometry const& rhs);

      void setX1(double x1) {this->x1 = x1;}
      void setY1(double y1) {this->y1 = y1;}
      void setX2(double x2) {this->x2 = x2;}
      void setY2(double y2) {this->y2 = y2;}
      void setSizePosition(double x1, double y1, double x2, double y2) {this->x1 = x1; this->y1 = y1; this->x2 = x2; this->y2 = y2;}
      void setRotation(double rotation) {this->rotation = rotation;}
      void setIconSource(std::string iconSource);
      void setIconRotation(double iconRotation) {this->iconRotation = iconRotation;}
      void setIconFlip(bool iconFlip) {this->iconFlip = iconFlip;}
      void setIconFixedAspectRatio(bool iconFixedAspectRatio) {this->iconFixedAspectRatio = iconFixedAspectRatio;}

      double getX1() const {return x1;}
      double getY1() const {return y1;}
      double getX2() const {return x2;}
      double getY2() const {return y2;}
      double getRotation() const {return rotation;}
      std::string getIconSource() const {return std::string(iconSource);}
      bool hasIconSource() const {return (NULL != iconSource);}
      double getIconRotation() const {return iconRotation;}
      bool getIconFlip() const {return iconFlip;}
      bool getIconFixedAspectRatio() const {return iconFixedAspectRatio;}

      oms_status_enu_t exportToSSD(pugi::xml_node& root) const;
    };
  }
}

namespace oms2
{
  namespace ssd
  {
    class ElementGeometry : protected ssd_element_geometry_t
    {
    public:
      ElementGeometry();
      ElementGeometry(const ElementGeometry& rhs);
      ~ElementGeometry();

      ElementGeometry& operator=(ElementGeometry const& rhs);

      void setX1(double x1) {this->x1 = x1;}
      void setY1(double y1) {this->y1 = y1;}
      void setX2(double x2) {this->x2 = x2;}
      void setY2(double y2) {this->y2 = y2;}
      void setSizePosition(double x1, double y1, double x2, double y2) {this->x1 = x1; this->y1 = y1; this->x2 = x2; this->y2 = y2;}
      void setRotation(double rotation) {this->rotation = rotation;}
      void setIconSource(std::string iconSource);
      void setIconRotation(double iconRotation) {this->iconRotation = iconRotation;}
      void setIconFlip(bool iconFlip) {this->iconFlip = iconFlip;}
      void setIconFixedAspectRatio(bool iconFixedAspectRatio) {this->iconFixedAspectRatio = iconFixedAspectRatio;}

      double getX1() const {return x1;}
      double getY1() const {return y1;}
      double getX2() const {return x2;}
      double getY2() const {return y2;}
      double getRotation() const {return rotation;}
      std::string getIconSource() const {return std::string(iconSource);}
      bool hasIconSource() const {return (NULL != iconSource);}
      double getIconRotation() const {return iconRotation;}
      bool getIconFlip() const {return iconFlip;}
      bool getIconFixedAspectRatio() const {return iconFixedAspectRatio;}

      oms_status_enu_t exportToSSD(pugi::xml_node& root) const;
    };
  }
}

#endif
