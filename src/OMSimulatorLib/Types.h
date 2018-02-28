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

#ifndef _OMSIMULATOR_TYPES_H_
#define _OMSIMULATOR_TYPES_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** API status codes */
typedef enum {
  oms_status_ok,
  oms_status_warning,
  oms_status_discard,
  oms_status_error,
  oms_status_fatal,
  oms_status_pending
} oms_status_enu_t;

typedef enum {
  oms_modelState_instantiated,
  oms_modelState_initialization,
  oms_modelState_simulation
} oms_modelState_enu_t;

typedef enum {
  oms_causality_input,
  oms_causality_output,
  oms_causality_parameter,
  oms_causality_undefined,
} oms_causality_enu_t;

/* ************************************ */
/* OMSimulator 2.0                      */
/* ************************************ */

typedef enum {
  oms_component_none,
  oms_component_tlm,  ///< TLM model
  oms_component_fmi,  ///< FMI model
  oms_component_fmu,  ///< FMU
  oms_component_port  ///< port
} oms_component_type_enu_t;

typedef enum {
  oms_signal_type_real,
  oms_signal_type_integer,
  oms_signal_type_boolean,
  oms_signal_type_string,
  oms_signal_type_enum
} oms_signal_type_enu_t;

typedef struct {
  oms_causality_enu_t causality;
  oms_signal_type_enu_t type;
  char* name;
} oms_signal_t;

typedef struct {
  oms_component_type_enu_t type;
  char* name;
  oms_signal_t** interfaces;
} oms_component_t;

typedef enum {
  oms_connection_fmi,
  oms_connection_tlm
} oms_connection_type_enu_t;

typedef struct {
  oms_connection_type_enu_t type;
  char* from;
  char* to;
  /// \todo optional TLM attributes
} oms_connection_t;

/**
 * \brief Message types used by the Logging mechanism.
 */
typedef enum {
  oms_message_info,    ///< Informative message.
  oms_message_warning, ///< Warning; Non-critical issues.
  oms_message_error,   ///< Error; Potentially critical issues.
  oms_message_debug,   ///< Debug message. (Only enabled if library is configured with OMS_DEBUG_LOGGING.)
  oms_message_trace    ///< Trace message for detailed debug logging. (Only enabled if library is configured with OMS_DEBUG_LOGGING.)
} oms_message_type_enu_t;

/**
 * \brief 5.2.1.1 ssd:ConnectorGeometry
 *
 * This optional XML element gives the geometry information of the connector.
 * Note that x and y coordinates are in a special coordinate system, where 0,0
 * is the lower-left corner of the containing model element, and 1,1 is the
 * upper-right corner of the model element, regardless of aspect ratio.
 *
 * For systems the placement of connectors for the inside and outside view of
 * the system is identical, the special coordinate system is just translated to
 * different actual coordinate systems, namely the one determined by the
 * ElementGeometry for the outside view, and the one determined by
 * SystemGeometry for the inside view.
 */
typedef struct {
  /*
   * Required attribute giving the x coordinate of the connector inside the
   * special coordinate system.
   */
  double x;
  /*
   * Required attribute giving the y coordinate of the connector inside the
   * special coordinate system.
   */
  double y;
} ssd_connector_geometry_t;

/**
 * \brief 5.2.2 ssd:ElementGeometry
 *
 * This optional XML element defines the geometry information of the model
 * element, where (x1,y1) and (x2,y2) define the positions of the lower-left
 * and upper-right corners of the model element in the coordinate system of its
 * parent system. If x1>x2 this indicates horizontal flipping, y1>y2 indicates
 * vertical flipping.
 * The optional attribute rotation (in degrees) defines an additional rotation
 * (applied after flipping), where positive numbers indicate left rotation
 * (x->y). The coordinate system is oriented: x -> right, y -> up.
 * The optional attribute iconSource defines an icon URI with the same
 * semantics as for the source attribute of the Component element. If defined,
 * this icon overrides any icon that may be defined e.g. in an .fmu file (as
 * defined in the relevant FMI standards). The optional attribute iconRotation
 * defines the rotation (in degrees) of the icon. The optional attribute
 * iconFixedAspectRatio defines whether the icon shall be fit into the extent
 * defined by (x1,y1), (x2,y2) and iconRotation with fixed aspect ratio. The
 * optional attribute iconFlip defines whether any flipping indicated by
 * (x1,y1), (x2,y2) shall be applied to the icon graphics, too.
 */
typedef struct {
  /**
   * Required attribute giving the x coordinate of the lower left corner of the
   * model element inside the coordinate system of its parent system.
   */
  double x1;
  /**
   * Required attribute giving the y coordinate of the lower left corner of the
   * model element inside the coordinate system of its parent system.
   */
  double y1;
  /**
   * Required attribute giving the x coordinate of the upper right corner of the
   * model element inside the coordinate system of its parent system.
   */
  double x2;
  /**
   * Required attribute giving the y coordinate of the upper right corner of the
   * model element inside the coordinate system of its parent system.
   */
  double y2;
  /**
   * Optional attribute defines an additional rotation in degrees that is to be
   * applied after any flipping, where positive numbers indicate left rotation
   * (x->y).
   */
  double rotation;
  /**
   * Optional attribute defines an icon URI with the same semantics as for the
   * source attribute of the Component element. If defined, this icon overrides
   * any icon that may be defined e.g. in an .fmu file (as specified in the
   * relevant FMI standards).
   */
  char* iconSource;
  /**
   * Optional attribute defines the rotation (in degrees) of the icon, where
   * positive numbers indicate left rotation (x->y).
   */
  double iconRotation;
  /**
   * Optional attribute defines whether any flipping indicated by (x1,y1),
   * (x2,y2) shall be applied to the icon graphics, too, or not.
   */
  bool iconFlip;
  /**
   * Optional attribute defines whether the icon shall be fit into the extent
   * defined by (x1,y1), (x2,y2) with a fixed aspect ratio, or without keeping
   * the aspect ratio fixed.
   */
  bool iconFixedAspectRatio;
} ssd_element_geometry_t;

/**
 * \brief 5.3.2.1 ssd:ConnectionGeometry
 *
 * This optional element defines the geometry information of the connection.
 * The start and end coordinates of the connection are derived automatically
 * through the coordinates of the corresponding connectors. The only relevant
 * geometry information provided by the connection geometry is a, by default
 * empty, list of intermediate waypoint coordinates, which are to be interpreted
 * as for the svg:polyline primitive, i.e. as waypoints for straight line
 * segments, with the first and last points added automatically based on the
 * translated coordinates of the start and end connectors. Note that x and y
 * coordinates are in the coordinate system of the enclosing system.
 */
typedef struct {
  /*
   * Required attribute giving a list of x coordinates of the intermediate
   * waypoints.
   */
  double* pointsX;
  /*
   * Required attribute giving a list of y coordinates of the intermediate
   * waypoints.
   */
  double* pointsY;
  /*
   * Number of intermediate waypoints.
   */
  unsigned int n;
} ssd_connection_geometry_t;

/**
 * \brief 5.3.4 ssd:SystemGeometry
 *
 * This element defines the extent of the system canvas. (x1,y1) and (x2,y2)
 * define the lower-left and upper-right corner, respectively. Different from
 * ElementGeometry, where x1 > x2 and y1 > y2 indicate flipping, x1 < x2 and
 * y1 < y2 MUST hold here.
 * If undefined, the system canvas extent defaults to the bounding box of all
 * ElementGeometry elements of the child elements of the system.
 * When displaying the content of a sub-system together with the enclosing
 * parent system, the transformation of co-coordinates inside the sub-system to
 * co-ordinates in the parent system is defined by the transformation from
 * SystemGeometry.{x1,y1,x2,y2} to ElementGeometry.{x1',y1',x2',y2'}, where
 * ElementGeometry.z' is the respective coordinate of the sub-system when
 * instantiated in the parent system after rotation.
 */
typedef struct {
  /*
   * Required attribute giving the x coordinate of the lower-left corner of the
   * system canvas.
   */
  double x1;
  /*
   * Required attribute giving the y coordinate of the lower-left corner of the
   * system canvas.
   */
  double y1;
  /*
   * Required attribute giving the x coordinate of the upper-right corner of the
   * system canvas.
   */
  double x2;
  /*
   * Required attribute giving the y coordinate of the upper-right corner of the
   * system canvas.
   */
  double y2;
} ssd_system_geometry_t;

#ifdef __cplusplus
}
#endif

#endif
