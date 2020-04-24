#CAPTION#
addConnector
------------

Adds a connector to a given component.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnector(cref, causality, type)
  
  The second argument "causality", should be any of the following,
  
  oms_causality_input
  oms_causality_output 
  oms_causality_parameter
  oms_causality_bidir
  oms_causality_undefined
  
  The third argument "type", should be any of the following,
  
  oms_signal_type_real
  oms_signal_type_integer
  oms_signal_type_boolean
  oms_signal_type_string
  oms_signal_type_enum
  oms_signal_type_bus


#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnector(cref, causality, type)
  
  The second argument "causality", should be any of the following,
  
  oms.input
  oms.output 
  oms.parameter
  oms.bidir
  oms.undefined
  
  The third argument "type", should be any of the following,
  
  oms.signal_type_real
  oms.signal_type_integer
  oms.signal_type_boolean
  oms.signal_type_string
  oms.signal_type_enum
  oms.signal_type_bus
  

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addConnector(const char* cref, oms_causality_enu_t causality, oms_signal_type_enu_t type);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addConnector(cref, causality, type);
  
  The second argument "causality", should be any of the following,
  
  "OpenModelica.Scripting.oms_causality.oms_causality_input" 
  "OpenModelica.Scripting.oms_causality.oms_causality_output" 
  "OpenModelica.Scripting.oms_causality.oms_causality_parameter" 
  "OpenModelica.Scripting.oms_causality.oms_causality_bidir" 
  "OpenModelica.Scripting.oms_causality.oms_causality_undefined"
  
  The third argument type, should be any of the following,
  
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_real"
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_integer"
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_boolean"
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_string"
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_enum"
  "OpenModelica.Scripting.oms_signal_type.oms_signal_type_bus"

#END#

#DESCRIPTION#
#END#
