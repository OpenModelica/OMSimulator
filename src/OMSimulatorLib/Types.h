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
  oms_causality_input,      ///< input
  oms_causality_output,     ///< output
  oms_causality_parameter,  ///< REMOVE ME
  oms_causality_bidir,      ///< bidirecitonal
  oms_causality_inherited,  ///< inherited
  oms_causality_undefined
} oms_causality_enu_t;

typedef enum {
  oms_tlm_no_interpolation,
  oms_tlm_coarse_grained,
  oms_tlm_fine_grained
} oms_tlm_interpolation_t;

typedef enum {
  oms_solver_internal,         ///< internal solver; CS-FMU only
  oms_solver_explicit_euler,
  oms_solver_cvode
} oms_solver_enu_t;

typedef enum {
  oms_element_none,
  oms_element_system,
  oms_element_component
} oms3_element_enu_t;

typedef enum {
  oms_component_none,
  oms_component_tlm,      ///< TLM composite model
  oms_component_fmi,      ///< FMI composite model
  oms_component_external, ///< External model
  oms_component_fmu,      ///< FMU
  oms_component_table,    ///< lookup table
  oms_component_port      ///< port
} oms_element_type_enu_t;

typedef enum {
  oms_system_none,
  oms_system_tlm,      ///< TLM System
  oms_system_wc,       ///< Weakly Coupled System
  oms_system_sc        ///< Strongly Coupled System
} oms_system_enu_t;

typedef enum {
  oms3_component_external, ///< External model
  oms3_component_fmu,      ///< FMU
  oms3_component_table     ///< lookup table
} oms3_component_enu_t;

typedef enum {
  oms_signal_type_real,
  oms_signal_type_integer,
  oms_signal_type_boolean,
  oms_signal_type_string,
  oms_signal_type_enum,
  oms_signal_type_bus
} oms_signal_type_enu_t;

/**
 * \brief Connection type
 */
typedef enum {
  oms_connection_fmi, ///< FMI conncection (directed & undelayed signal connection)
  oms_connection_tlm  ///< TLM connection
} oms_connection_type_enu_t;

typedef enum {
  oms3_connection_single, ///< FMI conncection (directed & undelayed signal connection)
  oms3_connection_bus,  ///< Bus connection
  oms3_connection_tlm
} oms3_connection_type_enu_t;

/**
 * \brief Message types used by the logging mechanism.
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
  /**
   * Required attribute giving a list of x coordinates of the intermediate
   * waypoints.
   */
  double* pointsX;
  /**
   * Required attribute giving a list of y coordinates of the intermediate
   * waypoints.
   */
  double* pointsY;
  /**
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
  /**
   * Required attribute giving the x coordinate of the lower-left corner of the
   * system canvas.
   */
  double x1;
  /**
   * Required attribute giving the y coordinate of the lower-left corner of the
   * system canvas.
   */
  double y1;
  /**
   * Required attribute giving the x coordinate of the upper-right corner of the
   * system canvas.
   */
  double x2;
  /**
   * Required attribute giving the y coordinate of the upper-right corner of the
   * system canvas.
   */
  double y2;
} ssd_system_geometry_t;

/**
 * \brief 5.3.6 ssd:SimulationInformation
 *
 * This element provides default information for useful solver/master
 * algorithms and their default settings for processing the given component or
 * (sub-)system. The information is purely an optional indicator of useful
 * settings, any processing tool is free to use, merge, or discard this
 * information in any way it sees fit.
 *
 * Multiple child-elements are allowed here in order to supply different
 * settings for different kinds of solver/master algorithms.
 *
 * Simulation information can be present at multiple hierarchy levels of the
 * overall system, including individual components. It is up to the processing
 * tool to decide how best to integrate this information into its overall
 * solving/simulation strategy. Tools supporting the use of multiple, different
 * solvers/co-simulation master algorithms inside one simulation can make use
 * of multiple different settings at different scopes, however this standard
 * does not presume or require the existence of this functionality.
 */
typedef struct {
  int dummy; ///< just to make msvc happy
} ssd_simulation_information_t;

/**
 * \brief Parameters for TLM connection
 */
typedef struct {
  double delay;
  double alpha;
  double impedance;
  double impedancerot;
} oms3_tlm_connection_parameters_t;

/**
 * \brief Connection between two connectors.
 */
typedef struct {
  oms_connection_type_enu_t type;      ///< Connection type, e.g. TLM
  char* parent;                        ///< Name of parent element
  char* conA;                          ///< Name of connector A
  char* conB;                          ///< Name of connector B
  ssd_connection_geometry_t* geometry; ///< Geometry information of the connection
  /// \todo optional TLM attributes
} oms_connection_t;

typedef struct {
  oms3_connection_type_enu_t type;      ///< Connection type, e.g. TLM
  char* conA;                          ///< Name of connector A
  char* conB;                          ///< Name of connector B
  ssd_connection_geometry_t* geometry; ///< Geometry information of the connection
  oms3_tlm_connection_parameters_t* tlmparameters; ///< TLM parameters (only for TLM connections)
} oms3_connection_t;

/**
 * \brief Signal: input, output, and parameter
 */
typedef struct {
  oms_causality_enu_t causality;      ///< input/output/parameter
  oms_signal_type_enu_t type;         ///< Type of the connector
  char* name;                         ///< Name of the connector
  ssd_connector_geometry_t* geometry; ///< Geometry information of the connector
} oms_connector_t;

/**
 * /brief Bus connector
 */
typedef struct {
  char* name;
  char** connectors;
  ssd_connector_geometry_t* geometry;
} oms3_busconnector_t;

/**
 * /brief TLM Bus connector
 */
typedef struct {
  char* name;
  ssd_connector_geometry_t* geometry;
  char* domain;
  int dimensions;
  oms_tlm_interpolation_t interpolation;
} oms3_tlmbusconnector_t;

/**
 * \brief Element (aka ssd:Component)
 */
typedef struct _oms3_element_t{
  oms3_element_enu_t type;           ///< Element type, i.e. system or component
  char* name;                        ///< Name of the element
  struct _oms3_element_t** elements; ///< List (null-terminated array) of all sub-elements
  oms_connector_t** connectors;      ///< List (null-terminated array) of all interface variables: inputs, outputs, and parameters.
  oms3_busconnector_t** busconnectors;  ///< List (null-terminated array) of all bus connectors
  oms3_tlmbusconnector_t** tlmbusconnectors;  ///< List (null-terminated array) of all TLM bus connectors
  ssd_element_geometry_t* geometry;  ///< Geometry information of the element
} oms3_element_t;

typedef struct {
  oms_element_type_enu_t type;      ///< Element type, e.g. FMU
  char* name;                       ///< Name of the element
  oms_connector_t** connectors;     ///< List (null-terminated array) of all interface variables: inputs, outputs, and parameters.
  ssd_element_geometry_t* geometry; ///< Geometry information of the element
} oms_element_t;

typedef enum {
  oms_fmi_kind_unknown = 0,
  oms_fmi_kind_me = 1,       ///< FMI for Model Exchange
  oms_fmi_kind_cs = 2,       ///< FMI for Co-Simulation
  oms_fmi_kind_me_and_cs = 3 ///< defined to be equal to (oms_fmi_kind_me|oms_fmi_kind_cs)
} oms_fmi_kind_enu_t;

/**
 * \brief FMU specific attributes
 */
typedef struct {
  /**
   * Optional string with the name and organization of the model author.
   */
  char* author;
  /**
   * Optional information on the intellectual property copyright for this FMU.
   * [Example: copyright = "© My Company 2011"].
   */
  char* copyright;
  /**
   * Optional string with a brief description of the model.
   */
  char* description;
  /**
   * FMI for Model Exchange or Co-Simulation.
   */
  oms_fmi_kind_enu_t fmiKind;
  /**
   * Version of "FMI for Model Exchange or Co-Simulation" that was used to
   * generate the XML file. The value for this version is "2.0". Future minor
   * revisions are denoted as "2.0.1", "2.0.2", ...
   */
  char* fmiVersion;
  /**
   * Optional date and time when the XML file was generated. The format is a
   * subset of "xs:dateTime" and should be: "YYYY-MM-DDThh:mm:ssZ" (with one
   * "T" between date and time; "Z" characterizes the Zulu time zone, in other
   * words Greenwich meantime).
   * [Example: "2009-12-08T14:33:22Z"].
   */
  char* generationDateAndTime;
  /**
   * Optional name of the tool that generated the XML file.
   */
  char* generationTool;
  /**
   * The "Globally Unique IDentifier" is a string that is used to check that
   * the XML file is compatible with the C functions of the FMU. Typically when
   * generating the XML file, a fingerprint of the "relevant" information is
   * stored as guid and in the generated C-function.
   */
  char* guid;
  /**
   * Optional information on the intellectual property licensing for this FMU.
   * [Example: license = "BSD license <license text or link to license>"].
   */
  char* license;
  /**
   * The name of the model as used in the modeling environment that generated the
   * XML file, such as "Modelica.Mechanics.Rotational.Examples.CoupledClutches".
   */
  char* modelName;
  /**
   * Path to the FMU.
   */
  char* path;
  /**
   * Optional version of the model, for example "1.0".
   */
  char* version;
  /**
   * This flag indicates cases (especially for embedded code), where only one
   * instance per FMU is possible (multiple instantiation is default = false;
   * if multiple instances are needed and this flag = true, the FMUs must be
   * instantiated in different processes).
   */
  bool canBeInstantiatedOnlyOncePerProcess;
  /**
   * If true, the environment can inquire the internal FMU state and can
   * restore it. That is, functions fmi2GetFMUstate, fmi2SetFMUstate, and
   * fmi2FreeFMUstate are supported by the FMU.
   */
  bool canGetAndSetFMUstate;
  /**
   * If true, the FMU uses its own functions for memory allocation and freeing
   * only. The callback functions allocateMemory and freeMemory given in
   * fmi2Instantiate are ignored.
   */
  bool canNotUseMemoryManagementFunctions;
  /**
   * If true, the environment can serialize the internal FMU state, in other
   * words functions fmi2SerializedFMUstateSize, fmi2SerializeFMUstate,
   * fmi2DeSerializeFMUstate are supported by the FMU. If this is the case,
   * then flag canGetAndSetFMUstate must be true as well.
   */
  bool canSerializeFMUstate;
  /**
   * If true, function fmi2CompletedIntegratorStep need not to be called (which
   * gives a slightly more efficient integration). If it is called, it has no
   * effect.
   * If false (the default), the function must be called after every completed
   * integrator step, see section 3.2.2.
   */
  bool completedIntegratorStepNotNeeded;
  /**
   * If true, a tool is needed to execute the model and the FMU just contains
   * the communication to this tool.
   * [Typically, this information is only utilized for information purposes.
   * For example when loading an FMU with needsExecutionTool = true, the
   * environment can inform the user that a tool has to be available on the
   * computer where the model is instantiated. The name of the tool can be
   * taken from attribute generationTool of fmiModelDescription.]
   */
  bool needsExecutionTool;
  /**
   * If true, the directional derivative of the equations can be computed with
   * fmi2GetDirectionalDerivative(..).
   */
  bool providesDirectionalDerivative;
  /**
   * The slave is able to interpolate continuous inputs. Calling of
   * fmi2SetRealInputDerivatives(...) has an effect for the slave.
   */
  bool canInterpolateInputs;
} oms_fmu_info_t;

#ifdef __cplusplus
}
#endif

#endif
