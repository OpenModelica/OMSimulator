#CAPTION#
addConnector
------------

Adds a connector to a given component.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnector(cref, causality, type)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnector(cref, causality, type)

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
