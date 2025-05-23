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

#ifndef _OMS_SSD_TAGS_H_
#define _OMS_SSD_TAGS_H_

namespace oms
{
  namespace ssp
  {
    namespace Version1_0
    {
      extern const char* simulation_information;
      extern const char* FixedStepMaster;
      extern const char* VariableStepMaster;
      extern const char* VariableStepSolver;
      extern const char* snap_shot;
      extern const char* oms_file;
      extern const char* oms_annotations;
      extern const char* oms_buses;
      extern const char* oms_signalFilter;
      extern const char* oms_Variable;

      namespace ssd
      {
        extern const char* parameter_bindings;
        extern const char* parameter_binding;
        extern const char* parameter_values;
        extern const char* parameter_mapping;
      }

      namespace ssm
      {
        extern const char* parameter_mapping;
        extern const char* parameter_mapping_entry;
      }

      namespace ssv
      {
        extern const char* parameter_set;
        extern const char* parameters;
        extern const char* parameter;

        extern const char* units;

        extern const char* real_type;
        extern const char* integer_type;
        extern const char* boolean_type;
        extern const char* string_type;
        extern const char* enumeration_type;
        extern const char* binary_type;
      }

      namespace ssc
      {
        extern const char* real_type;
        extern const char* integer_type;
        extern const char* boolean_type;
        extern const char* string_type;
        extern const char* enumeration_type;
        extern const char* binary_type;
        extern const char* annotation;
        extern const char* unit;
        extern const char* base_unit;
        extern const char* enum_item;
      }
    }

    namespace Draft20180219
    {
      extern const char* annotation_type;
      extern const char* bus;
      extern const char* signals;
      extern const char* signal;
      extern const char* bus_connections;
      extern const char* bus_connection;
      extern const char* external_model;

      namespace ssd
      {
        extern const char* annotation;
        extern const char* annotations;
        extern const char* component;
        extern const char* connection_geometry;
        extern const char* connection;
        extern const char* connections;
        extern const char* connector_geometry;
        extern const char* connector;
        extern const char* connectors;
        extern const char* default_experiment;
        extern const char* element_geometry;
        extern const char* elements;
        extern const char* enumerations;
        extern const char* simulation_information;
        extern const char* system_structure_description;
        extern const char* system;
        extern const char* units;
      }

    }
  }
}

#endif
